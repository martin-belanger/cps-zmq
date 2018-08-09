#include <zmq.h>
#include <errno.h>
#include <string.h>

class zeromq_publisher_context_c : public publisher_context_c
{
public:

    virtual ~zeromq_publisher_context_c()
    {
        zmq_close(publisher_pm);
        publisher_pm = NULL;

        zmq_ctx_destroy(ctx_pm);
        ctx_pm = NULL;
    }

    zeromq_publisher_context_c(unsigned long int    num,
                               volatile int       * sighup_p,
                               const volatile int * sigterm_p,
                               const std::string  & cat_r,
                               const std::string  & url_r) : publisher_context_c(num, sighup_p, sigterm_p, cat_r)
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

        //unsigned int send_hwm = 1000;
        //rc = zmq_setsockopt(publisher_pm, ZMQ_SNDHWM, &send_hwm, sizeof (send_hwm));
        //if (rc != 0)
        //{
        //    std::cerr << "zmq_setsockopt(ZMQ_SNDHWM) failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
        //    exit(EXIT_FAILURE);
        //}

        //int one = 1;
        //rc = zmq_setsockopt(publisher_pm, ZMQ_XPUB_NODROP, &one, sizeof(one));
        //if (rc != 0)
        //{
        //    std::cerr << "zmq_setsockopt(ZMQ_XPUB_NODROP) failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
        //    exit(EXIT_FAILURE);
        //}

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
        int rc;

        rc = zmq_send(publisher_pm, cat_rm.c_str(), cat_rm.length(), ZMQ_SNDMORE);
        if (rc == -1)
            std::cerr << "zmq_send() Part 1 - failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;

        rc = zmq_send(publisher_pm, obj_p->array(), obj_p->array_len(), 0);
        if (rc == -1)
            std::cerr << "zmq_send() Part 2 - failed with errno=" << errno << " (" << strerror(errno) << ')' << std::endl;
    }

private:
    void  * ctx_pm;
    void  * publisher_pm;

};

