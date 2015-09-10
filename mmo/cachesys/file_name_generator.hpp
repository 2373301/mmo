#pragma  once
#include "typedef.hpp"
#include <time.h>
#include <boost/date_time/posix_time/ptime.hpp>

class file_name_generator
    :boost::noncopyable
{
public:
    file_name_generator(uint32_t time_interval)
    {
       internal_ =  0 == time_interval ? ( 5 * 30) : time_interval;
       last_gen_time_ = 0;
    }

    ~file_name_generator()
    {}


    bool expire() // 判断是否过期, 来产生下一个文件名
    {
        int64_t now =  milliNow();
        int64_t diff = now - last_gen_time_;
        return diff >= last_gen_time_ ? true : false;
    }

    std::string gen()
    {
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        std::tm local = boost::posix_time::to_tm(now);
        last_gen_time_ = milliNow();
        char file_name[250] = {0};
        sprintf(file_name, "%d%0.2d%0.2d_%0.2d%0.2d%0.2d.sqlite",
            local.tm_year, local.tm_mon, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
        return std::string(file_name);
    }

    int64_t milliNow()
    {
        boost::posix_time::ptime time_now;
        time_now = boost::posix_time::microsec_clock::universal_time();
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
        boost::posix_time::millisec_posix_time_system_config::time_duration_type time_elapse;
        time_elapse = time_now - epoch;

        return time_elapse.total_milliseconds();
    }

private:
    int internal_;
    int64_t last_gen_time_;
};