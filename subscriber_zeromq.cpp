#include <zmq.h>
#include <errno.h>
#include <string.h>
#include "defs.h"

class zeromq_subscriber_c : public subscriber_c
{
public:
    virtual ~zeromq_subscriber_c()
    {
        for (auto & [path, subscription] : subscriptions_m)
        {
            zmq_setsockopt(subscriber_pm, ZMQ_UNSUBSCRIBE, &subscription.key_m, cps_api_key_get_len(&subscription.key_m));
        }

        zmq_close(subscriber_pm);
        subscriber_pm = NULL;

        zmq_ctx_destroy(ctx_pm);
        ctx_pm = NULL;
    }

    zeromq_subscriber_c(const volatile int  & sigterm_r,
                        const str_list_t    & cat_lst_r,
                        const str_list_t    & url_lst_r) : subscriber_c(sigterm_r, cat_lst_r)
    {
        int rc;

        std::cout << "ZEROMQ " << join(cat_lst_r, "+") << " subscriber. Connect to: \"" << join(url_lst_r, "+") << "\"." << std::endl;

        // Initialize ZMQ framework
        ctx_pm = zmq_ctx_new();
        if (ctx_pm == NULL)
        {
            std::cerr << "zmq_ctx_new() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }

        subscriber_pm = zmq_socket(ctx_pm, ZMQ_SUB);
        if (subscriber_pm == NULL)
        {
            std::cerr << "zmq_socket() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }

        for (auto url : url_lst_r)
        {
            rc = zmq_connect(subscriber_pm, url.c_str());
            if (rc != 0)
            {
                std::cerr << "zmq_connect(url=\"" << url << "\") failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        for (auto & [path, subscription] : subscriptions_m)
        {
            size_t key_len = cps_api_key_get_len(&subscription.key_m);
            rc = zmq_setsockopt(subscriber_pm, ZMQ_SUBSCRIBE, &subscription.key_m, key_len);
            if (rc != 0)
            {
                std::cerr << "zmq_setsockopt() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    virtual void main_loop()
    {
        int               size;
        int               rc;
        zmq_msg_t         message;
        int64_t           more;
        size_t            more_size = sizeof more;
        cps_api_object_t  obj = cps_api_object_create();

        do
        {
            /* Block until a message is available to be received from socket or a signal is received */
            zmq_msg_init(&message);
            size = zmq_recvmsg(subscriber_pm, &message, 0);

            if (sigterm_rm != 0)
            {
                zmq_msg_close(&message);
                break;
            }

            if (size == -1)
            {
                if (errno != EINTR)
                {
                    zmq_msg_close(&message);
                    std::cerr << "zmq_recvmsg() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (!cps_api_array_to_object(zmq_msg_data(&message), size, obj))
                    std::cerr << "cps_api_array_to_object() failed" << std::endl;

                process_event(obj);
            }

            zmq_msg_close(&message);

        } while (sigterm_rm == 0);
    }

private:
    void * ctx_pm;
    void * subscriber_pm;
};

