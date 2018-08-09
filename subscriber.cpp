#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#include <sys/types.h>
#include <float.h>
#include <unistd.h>
#include <stdlib.h>             /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>             /* signal(), SIGTERM, SIGHUP */
#include <stdio.h>              /* setvbuf() */
#include <time.h>               /* clock_gettime() */
#include <sys/resource.h>       /* setrlimit() */
#include <systemd/sd-daemon.h>  /* sd_notify() */

#include "defs.h"
#include "cps_class_map.h"
#include "cps_api_events.h"
#include "benchmark.hpp"
#include "benchmark.c"

class stats_c
{
public:
    uint32_t    exp_seq_no;          // Expected Sequence No.
    uint32_t    missed_events_count; // A count of missed events

    double      max_time_ms;         // Maximum measured message transit time in msec
    double      min_time_ms;         // Minimum measured message transit time in msec
    double      total_time_ms;       // Total measured time for all messages received in the burst
    uint32_t    num_rx_msg;          // Total number of messages received

    stats_c()
    {
        max_time_ms         = DBL_MIN;
        min_time_ms         = DBL_MAX;
        total_time_ms       = 0;
        exp_seq_no          = 0;
        missed_events_count = 0;
        num_rx_msg          = 0;
    }

    void processor(uint32_t  seq_no, uint32_t  last_seq_no, uint64_t  timestamp_ns)
    {
        if (seq_no == 1) // This marks the start of a new batch of events
        {
            //std::cout << "Received seq_no=1. Restarting measurements." << std::endl;
            missed_events_count = 0;
            num_rx_msg          = 0;
            total_time_ms       = 0;
            max_time_ms         = DBL_MIN;
            min_time_ms         = DBL_MAX;
        }
        else if (seq_no != exp_seq_no)
        {
            std::cerr << "Unexpected seq_no=" << seq_no << " [" << exp_seq_no << ']' << std::endl;
            missed_events_count += (seq_no - exp_seq_no);
        }

        double time_elapsed_ms = (double)time_elapsed_nsec(timestamp_ns) / 1000000.0;

        if (time_elapsed_ms > max_time_ms)
            max_time_ms = time_elapsed_ms;

        if (time_elapsed_ms < min_time_ms)
            min_time_ms = time_elapsed_ms;

        total_time_ms += time_elapsed_ms;
        num_rx_msg++;

        if (seq_no == last_seq_no)
        {
            std::cout << "@@@@ Avg: " << std::setw(8) << std::setprecision(3) << std::fixed << std::right << avg() <<
                           "   Min: " << std::setw(8) << std::setprecision(3) << std::fixed << std::right << min() <<
                           "   Max: " << std::setw(8) << std::setprecision(3) << std::fixed << std::right << max() << " msec";
            if (missed_events_count != 0)
            {
                std::cout << " >> " << missed_events_count << " missed events.";
            }
            std::cout << std::endl;
        }

        exp_seq_no = seq_no + 1;
    }

private:
    inline double max() const { return max_time_ms; }
    inline double min() const { return min_time_ms; }
    inline double avg() const { return total_time_ms / num_rx_msg; }

    static inline uint64_t time_elapsed_nsec(uint64_t start_time)
    {
        struct timespec  now_time;
        clock_gettime(CLOCK_MONOTONIC, &now_time);

        return (((uint64_t)now_time.tv_sec * 1000000000) + now_time.tv_nsec) - start_time;
    }
};

class subscription_c
{
public:
    std::string         cat;                 // Data category (alarm, connection, telemetry, vlan, weather)
    cps_api_attr_id_t   key_id;              // PUBSUB_ALARM_OBJ, PUBSUB_CONNECTION_OBJ, PUBSUB_TELEMETRY_OBJ, PUBSUB_VLAN_OBJ, PUBSUB_WEATHER_OBJ
    int                 seq_no_attr_id;      // Attr. id for the sequence No. in this burst of events.
    int                 last_seq_no_attr_id; // Attr. id for the last sequence No. in this burst of events.
    int                 timestamp_attr_id;   // Attr. id for the nsec time stamp.
    stats_c             stats;

    subscription_c() {}
    virtual ~subscription_c() {}

    subscription_c(const std::string & cat_r)
    {
        cat = cat_r;
        if (cat == "alarm")
        {
            key_id              = PUBSUB_ALARM_OBJ;
            seq_no_attr_id      = PUBSUB_ALARM_SEQ_NO;
            last_seq_no_attr_id = PUBSUB_ALARM_LAST_SEQ_NO;
            timestamp_attr_id   = PUBSUB_ALARM_TIMESTAMP_NS;
        }
        else if (cat == "connection")
        {
            key_id              = PUBSUB_CONNECTION_OBJ;
            seq_no_attr_id      = PUBSUB_CONNECTION_SEQ_NO;
            last_seq_no_attr_id = PUBSUB_CONNECTION_LAST_SEQ_NO;
            timestamp_attr_id   = PUBSUB_CONNECTION_TIMESTAMP_NS;
        }
        else if (cat == "telemetry")
        {
            key_id              = PUBSUB_TELEMETRY_OBJ;
            seq_no_attr_id      = PUBSUB_TELEMETRY_SEQ_NO;
            last_seq_no_attr_id = PUBSUB_TELEMETRY_LAST_SEQ_NO;
            timestamp_attr_id   = PUBSUB_TELEMETRY_TIMESTAMP_NS;
        }
        else if (cat == "vlan")
        {
            key_id              = PUBSUB_VLAN_OBJ;
            seq_no_attr_id      = PUBSUB_VLAN_SEQ_NO;
            last_seq_no_attr_id = PUBSUB_VLAN_LAST_SEQ_NO;
            timestamp_attr_id   = PUBSUB_VLAN_TIMESTAMP_NS;
        }
        else
        {
            key_id              = PUBSUB_WEATHER_OBJ;
            seq_no_attr_id      = PUBSUB_WEATHER_SEQ_NO;
            last_seq_no_attr_id = PUBSUB_WEATHER_LAST_SEQ_NO;
            timestamp_attr_id   = PUBSUB_WEATHER_TIMESTAMP_NS;
        }
    }

};

/******************************************************************************/
class subscriber_context_c
{
protected:
    std::map<std::string, subscription_c>    subscriptions_m;
    const volatile int                     * sigterm_pm;
    const str_list_t                       & cat_lst_rm;

public:
    virtual ~subscriber_context_c() {}

    subscriber_context_c(const volatile int * sigterm_p,
                         const str_list_t   & cat_lst_r) : cat_lst_rm(cat_lst_r), sigterm_pm(sigterm_p)
    {
        int rc = module_init();
        if (rc != 0)
        {
            std::cerr << "module_init() failed with rc=" << rc << std::endl;
            exit(EXIT_FAILURE);
        }

        for (auto cat : cat_lst_rm)
        {
            subscription_c  s(cat);

            cps_api_object_t obj = cps_api_object_create();
            cps_api_key_from_attr_with_qual(cps_api_object_key(obj), s.key_id, cps_api_qualifier_TARGET);
            std::string  path(cps_class_string_from_key(cps_api_object_key(obj), 1));
            cps_api_object_delete(obj);

            subscriptions_m[path] = s;
        }
    }

    void process_event(cps_api_object_t obj)
    {
        cps_api_object_attr_t   attr;
        uint32_t                seq_no;
        uint32_t                last_seq_no;
        uint64_t                timestamp_ns;
        std::string             path(cps_class_string_from_key(cps_api_object_key(obj), 1));

        attr         = cps_api_object_attr_get(obj, subscriptions_m[path].seq_no_attr_id);
        seq_no       = cps_api_object_attr_data_u32(attr);
        attr         = cps_api_object_attr_get(obj, subscriptions_m[path].last_seq_no_attr_id);
        last_seq_no  = cps_api_object_attr_data_u32(attr);
        attr         = cps_api_object_attr_get(obj, subscriptions_m[path].timestamp_attr_id);
        timestamp_ns = cps_api_object_attr_data_u64(attr);

        subscriptions_m[path].stats.processor(seq_no, last_seq_no, timestamp_ns);
    }

    virtual void main_loop() = 0;
};

/******************************************************************************/
static volatile int sigterm = 0;
static void signal_hdlr(int signo)
{
    switch (signo)
    {
    case SIGTERM: sigterm = 1; break;
    default:;
    }
}

#include "args.cpp"
#include "subscriber_redis.cpp"
#include "subscriber_zeromq.cpp"

/******************************************************************************/
static void subscriber(const std::string  & type_r,
                       const str_list_t   & cat_lst_r,
                       const str_list_t   & url_lst_r)
{
    subscriber_context_c  * subscriber_context_p;
    if (type_r == "redis")
        subscriber_context_p = new redis_subscriber_context_c(&sigterm, cat_lst_r);
    else
        subscriber_context_p = new zeromq_subscriber_context_c(&sigterm, cat_lst_r, url_lst_r);

    // Tell systemd that this daemon is ready
    sd_notify(0, "READY=1");
    subscriber_context_p->main_loop();
    sd_notify(0, "STOPPING=1");

    delete subscriber_context_p;
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

    // Specify the signals we want to ignore. In this case we only want SIGTERM.
    sigset_t  sigmsk;
    sigfillset(&sigmsk);
    sigdelset(&sigmsk, SIGTERM); // SIGTERM -> Shutdown daemon
    sigprocmask(SIG_SETMASK, &sigmsk, NULL);

    struct sigaction action;
    action.sa_handler = signal_hdlr;
    action.sa_flags   = 0;
    sigemptyset(&action.sa_mask);

    // Install SIGTERM handler. This will be invoked when systemd wants to
    // stop the daemon. For example, "systemctl stop mydaemon.service"
    sigaction(SIGTERM, &action, NULL);

    arguments_c arguments(argc, argv, "CPS object Subscriber", false);

    subscriber(arguments.type, arguments.cat_lst, arguments.url_lst);

    exit(EXIT_SUCCESS);
}



