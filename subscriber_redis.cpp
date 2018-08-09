#include <stdlib.h>
#include "defs.h"

class redis_subscriber_c : public subscriber_c
{
public:
    virtual ~redis_subscriber_c() {}

    redis_subscriber_c(const volatile int  & sigterm_r,
                       const str_list_t    & cat_lst_r) : subscriber_c(sigterm_r, cat_lst_r)
    {
        std::cout << "REDIS " << join(cat_lst_r) << " subscriber. Connecting to publisher." << std::endl;

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

        for (auto & [path, subscription] : subscriptions_m)
        {
            cps_api_event_reg_t reg;

            reg.priority          = 0;
            reg.objects           = &subscription.key_m;
            reg.number_of_objects = 1;

            rc = cps_api_event_thread_reg(&reg, redis_subscriber_c::callback, this);
            if (rc != cps_api_ret_code_OK) {
                std::cerr << "cps_api_event_thread_reg() failed with rc=" << rc << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    virtual void main_loop()
    {
        do
        {
            pause();

        } while (sigterm_rm == 0);
    }

private:
    static bool callback(cps_api_object_t obj, void * context_p)
    {
        redis_subscriber_c * redis_context_p = (redis_subscriber_c *)context_p;
        redis_context_p->process_event(obj);
        return true;
    }
};

