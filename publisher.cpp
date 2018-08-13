#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>             /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>             /* signal(), SIGTERM, SIGHUP */
#include <stdio.h>              /* setvbuf() */
#include <sys/resource.h>       /* setrlimit() */
#include <systemd/sd-daemon.h>  /* sd_notify() */

#include "cps_api_events.h"
#include "benchmark.hpp"
#include "benchmark.c"

#include "args.cpp"

/******************************************************************************/
class publisher_c
{
protected:
    unsigned long int     num_m;      // Number of messages in a transmit burst.
    volatile int        & sighup_rm;  // Variable that gets changed by SIGHUP and is used to trigger a burst of messages.
    const volatile int  & sigterm_rm; // Variable that gets changed by SIGTERM and is used to terminate the program.
    benchmark::Object   * obj_pm;     // CPS object to be transmitted in each pub/sub message sent.

public:
    virtual ~publisher_c()
    {
        delete obj_pm;
        obj_pm = NULL;
    }

    publisher_c(unsigned long int    num,
                volatile int       & sighup_r,
                const volatile int & sigterm_r,
                const std::string  & cat_r) : num_m(num), sighup_rm(sighup_r), sigterm_rm(sigterm_r)
    {
        int rc = module_init();
        if (rc != 0)
        {
            std::cerr << "module_init() failed with rc=" << rc << std::endl;
            exit(EXIT_FAILURE);
        }

        // Build CPS object that will be sent as an event. We need only
        // create a single object. The same object will be reused for
        // all the transmitted messages.
        if (cat_r == "alarm")
        {
            benchmark::Alarm * alarm_p = new benchmark::Alarm();
            obj_pm = alarm_p;
        }
        else if (cat_r == "connection")
        {
            benchmark::Connection * connection_p = new benchmark::Connection();
            connection_p->set_name("A-to-Z");
            obj_pm = connection_p;
        }
        else if (cat_r == "telemetry")
        {
            benchmark::Telemetry * telemetry_p = new benchmark::Telemetry();
            telemetry_p->set_temperature("High");
            telemetry_p->set_voltage("Low");
            obj_pm = telemetry_p;
        }
        else if (cat_r == "vlan")
        {
            benchmark::Vlan * vlan_p = new benchmark::Vlan();
            vlan_p->set_name("Vlan-1");
            obj_pm = vlan_p;
        }
        else
        {
            benchmark::Weather * weather_p = new benchmark::Weather();
            weather_p->set_name("Sunny");
            obj_pm = weather_p;
        }

        obj_pm->set_last_seq_no((uint32_t)num);
    }

    /**
     * This sends a burst of messages. Note that we need to periodically pause
     * the publisher (using usleep) to allow context switches to take place.
     * Failure to pause the publisher would simply results in queues
     * overflowing and messages being dropped.
     */
    void send_events()
    {
        //std::cout << "Publisher start sending " << num_m << " events." << std::endl;
        unsigned long int seq_no;
        for (seq_no = 1; (sigterm_rm == 0) && (seq_no <= num_m); seq_no++)
        {
            obj_pm->set_seq_no(seq_no);
            obj_pm->set_timestamp_ns(now_nsec());
            send_one_event(obj_pm);
            if ((seq_no % 4) == 0)
            {
                usleep(4); // trigger a context switch so that subscribers get a chance to process messages
            }
        }
        //std::cout << "Publisher Done sending " << num_m << " events." << std::endl;
    }

    /**
     * This method needs to be overloaded. It is used to send a single event
     * over the pub/sub channel.
     *
     * @param obj_pm  Object to be sent as an event.
     */
    virtual void send_one_event(benchmark::Object * obj_pm) = 0;

    virtual void main_loop() = 0;

private:
    static inline uint64_t now_nsec()
    {
        struct timespec  now_time;
        clock_gettime(CLOCK_MONOTONIC, &now_time);

        return ((uint64_t)now_time.tv_sec * 1000000000) + now_time.tv_nsec;
    }
};

#include "publisher_nano.cpp"
#include "publisher_redis.cpp"
#include "publisher_zeromq.cpp"

static volatile int sighup  = 0;
static volatile int sigterm = 0;
static void signal_hdlr(int signo)
{
    switch (signo)
    {
    case SIGHUP:  sighup  = 1; break;
    case SIGTERM: sigterm = 1; break;
    default:;
    }
}

/******************************************************************************/
static void publisher(const std::string & type_r, unsigned long int num, const std::string & cat_r, const std::string & url_r)
{
    publisher_c * publisher_p;

    if (type_r == "redis")
        publisher_p = new redis_publisher_c(num, sighup, sigterm, cat_r);
    else if (type_r == "zeromq")
        publisher_p = new zeromq_publisher_c(num, sighup, sigterm, cat_r, url_r);
    else
        publisher_p = new nano_publisher_c(num, sighup, sigterm, cat_r, url_r);

    struct sigaction  action;
    action.sa_handler = signal_hdlr;
    action.sa_flags   = 0;
    sigemptyset(&action.sa_mask);

    // Install signal handler.
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGHUP,  &action, NULL);

    // Tell systemd that this daemon is ready
    sd_notify(0, "READY=1");
    publisher_p->main_loop();
    sd_notify(0, "STOPPING=1");

    delete publisher_p;
}

/******************************************************************************/
int main(int argc, char **argv)
{
    // Enable core dumps.
    // This could also be done via the service file with:
    //    LimitCORE=infinity
    static const struct rlimit rlim = { .rlim_cur = ~0U, .rlim_max = ~0U };
    setrlimit(RLIMIT_CORE, &rlim);

    // Disable stdout buffering.
    // This could also be done via the service file with:
    //    ExecStart=/usr/bin/stdbuf -o0 /usr/bin/car
    setvbuf(stdout, NULL, _IONBF, 0);

    // Specify the signals we want to ignore.
    // In this case we only want SIGTERM and SIGHUP.
    sigset_t  sigmsk;
    sigfillset(&sigmsk);
    sigdelset(&sigmsk, SIGHUP);  // SIGHUP  -> Start burst of messages
    sigdelset(&sigmsk, SIGTERM); // SIGTERM -> Shutdown daemon
    sigprocmask(SIG_SETMASK, &sigmsk, NULL);

//    struct sigaction  action;
//    action.sa_handler = signal_hdlr;
//    action.sa_flags   = 0;
//    sigemptyset(&action.sa_mask);
//
//    // Install signal handler.
//    sigaction(SIGTERM, &action, NULL);
//    sigaction(SIGHUP,  &action, NULL);

    arguments_c arguments(argc, argv, "CPS object Publisher", true);

    publisher(arguments.type, arguments.num, arguments.cat_lst[0], arguments.url_lst[0]);

    exit(EXIT_SUCCESS);
}


//cps_send_event.py
//cps_trace_events.py
