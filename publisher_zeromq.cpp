#include <zmq.h>
#include <errno.h>
#include <string.h>

class zeromq_publisher_c : public publisher_c
{
public:

    virtual ~zeromq_publisher_c()
    {
        zmq_close(publisher_pm);
        publisher_pm = NULL;

        zmq_ctx_destroy(ctx_pm);
        ctx_pm = NULL;
    }

    zeromq_publisher_c(unsigned long int    num,
                       volatile int       & sighup_r,
                       const volatile int & sigterm_r,
                       const std::string  & cat_r,
                       const std::string  & url_r) : publisher_c(num, sighup_r, sigterm_r, cat_r)
    {
        int rc;

        std::cout << "ZEROMQ \"" << cat_r << "\" Publisher. Will be sending " << num << " events over \"" << url_r << "\"" << std::endl;

        ctx_pm = zmq_ctx_new();
        if (ctx_pm == NULL)
        {
            std::cerr << "zmq_ctx_new() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }

        publisher_pm = zmq_socket(ctx_pm, ZMQ_PUB);
        if (publisher_pm == NULL)
        {
            std::cerr << "zmq_socket() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
            exit(EXIT_FAILURE);
        }

        rc = zmq_bind(publisher_pm, url_r.c_str());
        if (rc != 0)
        {
            std::cerr << "zmq_bind() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
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
        int rc = zmq_send(publisher_pm, obj_p->array(), obj_p->array_len(), 0);
        if (rc == -1)
            std::cerr << "zmq_send() failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
    }

private:
    void  * ctx_pm;         // zmq context
    void  * publisher_pm;   // zmq socket

};

