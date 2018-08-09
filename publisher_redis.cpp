class redis_publisher_context_c : public publisher_context_c
{
public:
    virtual ~redis_publisher_context_c()
    {
    }

    redis_publisher_context_c(unsigned long int    num,
                              volatile int       * sighup_p,
                              const volatile int * sigterm_p,
                              const std::string  & cat_r) : publisher_context_c(num, sighup_p, sigterm_p, cat_r)
    {
        std::cout << "REDIS \"" << cat_r << "\" Publisher. Will be sending " << num << " events." << std::endl;

        // Initialize CPS framework
        cps_api_return_code_t rc;
        rc = cps_api_event_service_init();
        if (rc != cps_api_ret_code_OK)
        {
            std::cerr << "cps_api_event_service_init() failed with rc=" << rc << std::endl;
            exit(EXIT_FAILURE);
        }

        rc = cps_api_event_thread_init();
        if (rc != cps_api_ret_code_OK)
        {
            std::cerr << "cps_api_event_thread_init() failed with rc=" << rc << std::endl;
            exit(EXIT_FAILURE);
        }

        cps_api_event_service_handle_t handle;
        rc = cps_api_event_client_connect(&handle);
        if (rc != cps_api_ret_code_OK)
        {
            std::cerr << "cps_api_event_client_connect() failed with rc=" << rc << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    virtual void main_loop()
    {
        do
        {
            pause();

            if (*sigterm_pm != 0)
            {
                break;
            }

            if (*sighup_pm)
            {
                sd_notify(0, "RELOADING=1");
                *sighup_pm = 0;
                sd_notify(0, "READY=1");
                send_events();
            }

        } while (*sigterm_pm == 0);
    }

    virtual void send_one_event(benchmark::Object * obj_p)
    {
        cps_api_event_thread_publish(obj_p->instance());
    }
};



