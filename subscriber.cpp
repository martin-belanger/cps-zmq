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
#include "stats.cpp"

typedef struct
{
    int key_id;
    int seq_no;
    int last_seq_no;
    int timestamp;

} ids_t;

static const std::map<const std::string, const ids_t> ids =
{
    { "alarm",      {PUBSUB_ALARM_OBJ,      PUBSUB_ALARM_SEQ_NO,      PUBSUB_ALARM_LAST_SEQ_NO,      PUBSUB_ALARM_TIMESTAMP_NS}      },
    { "connection", {PUBSUB_CONNECTION_OBJ, PUBSUB_CONNECTION_SEQ_NO, PUBSUB_CONNECTION_LAST_SEQ_NO, PUBSUB_CONNECTION_TIMESTAMP_NS} },
    { "telemetry",  {PUBSUB_TELEMETRY_OBJ,  PUBSUB_TELEMETRY_SEQ_NO,  PUBSUB_TELEMETRY_LAST_SEQ_NO,  PUBSUB_TELEMETRY_TIMESTAMP_NS}  },
    { "vlan",       {PUBSUB_VLAN_OBJ,       PUBSUB_VLAN_SEQ_NO,       PUBSUB_VLAN_LAST_SEQ_NO,       PUBSUB_VLAN_TIMESTAMP_NS}       },
    { "weather",    {PUBSUB_WEATHER_OBJ,    PUBSUB_WEATHER_SEQ_NO,    PUBSUB_WEATHER_LAST_SEQ_NO,    PUBSUB_WEATHER_TIMESTAMP_NS}    }
};

/******************************************************************************/
class subscription_c
{
public:
    cps_api_key_t   key_m;      // CPS key of the object we're subscribing to
    stats_c         stats_m;    // Statistics object. This is where we keep stats of the received messages (e.g. average transit time...)
    ids_t           yang_ids_m; // YANG derived Ids. This is used to retrieve attributes from the received messages.

    virtual ~subscription_c() {}

    subscription_c() {}

    subscription_c(const std::string & cat_r)
    {
        yang_ids_m = ids.at(cat_r);

        if (!cps_api_key_from_attr_with_qual(&key_m, yang_ids_m.key_id, cps_api_qualifier_TARGET))
        {
            std::cerr << "cps_api_key_from_attr_with_qual() failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
};

/******************************************************************************/
class subscriber_c
{
protected:
    std::map<std::string, subscription_c>    subscriptions_m;
    const volatile int                     & sigterm_rm;

public:
    virtual ~subscriber_c() {}

    /**
     * @param sigterm_r Reference to the variable that gets set when a SIGTERM
     *                  is detected.
     *
     * @param cat_lst_r List of data categories this subscriber is subscribing
     *                  for. This is a vector containing 1 or more of the
     *                  strings: "alarm", "connection", "telemetry", "vlan",
     *                  "weather".
     */
    subscriber_c(const volatile int & sigterm_r,
                         const str_list_t   & cat_lst_r) : sigterm_rm(sigterm_r)
    {
        int rc = module_init();
        if (rc != 0)
        {
            std::cerr << "module_init() failed with rc=" << rc << std::endl;
            exit(EXIT_FAILURE);
        }

        for (auto cat : cat_lst_r)
        {
            subscription_c  s(cat);
            subscriptions_m[cps_class_string_from_key(&s.key_m, 1)] = s;
        }
    }

    /**
     * This is where received messages (i.e. CPS objects) get processed.
     *
     * @author mbelanger (8/9/18)
     *
     * @param obj
     */
    void process_event(cps_api_object_t  obj)
    {
        cps_api_object_attr_t   attr;
        uint32_t                seq_no;
        uint32_t                last_seq_no;
        uint64_t                timestamp_ns;
        std::string             path(cps_class_string_from_key(cps_api_object_key(obj), 1));
        subscription_c        & subscription = subscriptions_m.at(path);

        attr         = cps_api_object_attr_get(obj, subscription.yang_ids_m.seq_no);
        seq_no       = cps_api_object_attr_data_u32(attr);
        attr         = cps_api_object_attr_get(obj, subscription.yang_ids_m.last_seq_no);
        last_seq_no  = cps_api_object_attr_data_u32(attr);
        attr         = cps_api_object_attr_get(obj, subscription.yang_ids_m.timestamp);
        timestamp_ns = cps_api_object_attr_data_u64(attr);

        subscription.stats_m.processor(seq_no, last_seq_no, timestamp_ns);
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
#include "subscriber_nano.cpp"
#include "subscriber_redis.cpp"
#include "subscriber_zeromq.cpp"

/******************************************************************************/
static void subscriber(const std::string  & type_r,
                       const str_list_t   & cat_lst_r,
                       const str_list_t   & url_lst_r)
{
    subscriber_c  * subscriber_p;
    if (type_r == "redis")
        subscriber_p = new redis_subscriber_c(sigterm, cat_lst_r);
    else if (type_r == "zeromq")
        subscriber_p = new zeromq_subscriber_c(sigterm, cat_lst_r, url_lst_r);
    else
        subscriber_p = new nano_subscriber_c(sigterm, cat_lst_r, url_lst_r);

    // Tell systemd that this daemon is ready
    sd_notify(0, "READY=1");
    subscriber_p->main_loop();
    sd_notify(0, "STOPPING=1");

    delete subscriber_p;
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



