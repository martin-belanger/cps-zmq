#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <errno.h>
#include <string.h>
#include "defs.h"

class nano_subscriber_c : public subscriber_c
{
public:
    virtual ~nano_subscriber_c()
    {
        for (auto & [path, subscription] : subscriptions_m)
        {
            nn_setsockopt(fd_m, NN_SUB, NN_SUB_SUBSCRIBE, &subscription.key_m, cps_api_key_get_len(&subscription.key_m));
        }

        for (auto eid : eid_m)
        {
            nn_shutdown(fd_m, eid);
        }

        eid_m.clear();

        nn_close(fd_m);
        fd_m = -1;
    }

    nano_subscriber_c(const volatile int  & sigterm_r,
                        const str_list_t    & cat_lst_r,
                        const str_list_t    & url_lst_r) : subscriber_c(sigterm_r, cat_lst_r)
    {
        int rc;

        std::cout << "nanomsg " << join(cat_lst_r, "+") << " subscriber. Connect to: \"" << join(url_lst_r, "+") << "\"." << std::endl;

        fd_m = nn_socket(AF_SP, NN_SUB);
        if (fd_m < 0)
        {
            std::cerr << "nn_socket() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }

        for (auto url : url_lst_r)
        {
            int eid = nn_connect(fd_m, url.c_str());

            if (rc != 0)
            {
                std::cerr << "nn_connect(url=\"" << url << "\") failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                nn_close(fd_m);
                exit(EXIT_FAILURE);
            }

            eid_m.push_back(eid);
        }

        for (auto & [path, subscription] : subscriptions_m)
        {
            size_t key_len = cps_api_key_get_len(&subscription.key_m);
            rc = nn_setsockopt(fd_m, NN_SUB, NN_SUB_SUBSCRIBE, &subscription.key_m, key_len);
            if (rc != 0)
            {
                std::cerr << "nn_setsockopt() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                nn_close(fd_m);
                exit(EXIT_FAILURE);
            }
        }
    }

    virtual void main_loop()
    {
        int               size;
        int               rc;
        void            * message = NULL;
        cps_api_object_t  obj = cps_api_object_create();

        do
        {
            /* Block until a message is available to be received from socket or a signal is received */
            size = nn_recv(fd_m, &message, NN_MSG, 0);

            if (sigterm_rm != 0)
            {
                if (message) nn_freemsg(message);
                break;
            }

            if (size == -1)
            {
                if (errno != EINTR)
                {
                    if (message) nn_freemsg(message);
                    std::cerr << "nn_recv() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (!cps_api_array_to_object(message, size, obj))
                    std::cerr << "cps_api_array_to_object() failed" << std::endl;

                process_event(obj);
            }

            nn_freemsg(message);
            message = NULL;

        } while (sigterm_rm == 0);
    }

private:
    int                 fd_m;   // Socket file descriptor
    std::vector<int>    eid_m;  // Endpoint IDs
};

