#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <errno.h>
#include <string.h>

class nano_publisher_c : public publisher_c
{
public:

    virtual ~nano_publisher_c()
    {
        nn_shutdown(fd_m, eid_m);
        eid_m = -1;

        nn_close(fd_m);
        fd_m = -1;
    }

    nano_publisher_c(unsigned long int    num,
                     volatile int       & sighup_r,
                     const volatile int & sigterm_r,
                     const std::string  & cat_r,
                     const std::string  & url_r) : publisher_c(num, sighup_r, sigterm_r, cat_r)
    {
        int rc;

        std::cout << "nanomsg \"" << cat_r << "\" Publisher. Will be sending " << num << " events over \"" << url_r << "\"" << std::endl;

        fd_m = nn_socket(AF_SP, NN_PUB);
        if (fd_m < 0)
        {
            std::cerr << "nn_socket() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }

        eid_m = nn_bind(fd_m, url_r.c_str());
        if (eid_m == -1)
        {
            std::cerr << "nn_bind() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    virtual void main_loop()
    {
        do
        {
            pause();

            if (sighup_rm)
            {
                sd_notify(0, "RELOADING=1");
                sighup_rm = 0;
                sd_notify(0, "READY=1");

                send_events();
            }

        } while (sigterm_rm == 0);
    }

    virtual void send_one_event(benchmark::Object * obj_p)
    {
        int rc = nn_send(fd_m, obj_p->array(), obj_p->array_len(), 0);
        if (rc == -1)
            std::cerr << "nn_send() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
    }

private:
    int  fd_m;   // Socket file descriptor
    int  eid_m;  // Endpoint ID

};


