
/******************************************************************************/
class stats_c
{
public:
    uint32_t    exp_seq_no;          // Expected Sequence No.
    uint32_t    missed_events_count; // A count of missed messages

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
        if (seq_no == 1) // This marks the start of a new batch of messages
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
            std::cout << "@@@@ Min < Avg < Max : "
                      << std::setw(8) << std::setprecision(3) << std::fixed << std::right << min() << " < "
                      << std::setw(8) << std::setprecision(3) << std::fixed << std::right << avg() << " < "
                      << std::setw(8) << std::setprecision(3) << std::fixed << std::right << max() << " msec"
                      << "   num_rx_msg=" << num_rx_msg << " total_time_ms=" << total_time_ms;
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
