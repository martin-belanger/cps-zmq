#include <zmq.h>
#include <errno.h>
#include <string.h>
#include "defs.h"

class zeromq_subscriber_context_c : public subscriber_context_c
{
public:
    virtual ~zeromq_subscriber_context_c()
    {
        for (auto cat : cat_lst_rm)
        {
            zmq_setsockopt(subscriber_pm, ZMQ_UNSUBSCRIBE, cat.c_str(), cat.length());
        }

        zmq_close(subscriber_pm);
        subscriber_pm = NULL;

        zmq_ctx_destroy(ctx_pm);
        ctx_pm = NULL;
    }

    zeromq_subscriber_context_c(const volatile int  * sigterm_p,
                                const str_list_t    & cat_lst_r,
                                const str_list_t    & url_lst_r) : subscriber_context_c(sigterm_p, cat_lst_r)
    {
        int rc;

        std::cout << "ZEROMQ " << join(cat_lst_rm, "+") << " subscriber. Connect to: \"" << join(url_lst_r, "+") << "\"." << std::endl;

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

        for (auto cat : cat_lst_rm)
        {
            rc = zmq_setsockopt(subscriber_pm, ZMQ_SUBSCRIBE, cat.c_str(), cat.length());
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

            if ((size == -1) && (errno != EINTR))
            {
                std::cerr << "zmq_recvmsg() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                exit(EXIT_FAILURE);
            }

            if (*sigterm_pm != 0)
            {
                zmq_msg_close(&message);
                break;
            }

            if (size != -1)
            {
                /* Determine if more message parts are to follow */
                rc = zmq_getsockopt(subscriber_pm, ZMQ_RCVMORE, &more, &more_size);
                if (rc != 0)
                {
                    std::cerr << "zmq_getsockopt() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (!more)
                {
                    if (!cps_api_array_to_object(zmq_msg_data(&message), size, obj))
                        std::cerr << "cps_api_array_to_object() failed" << std::endl;

                    process_event(obj);
                }
            }

            zmq_msg_close(&message);

        } while (*sigterm_pm == 0);
    }

private:
    void * ctx_pm;
    void * subscriber_pm;
};

