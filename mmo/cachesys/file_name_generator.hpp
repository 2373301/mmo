#pragma  once
#include "typedef.hpp"
#include <time.h>
#include <cstdlib>
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
        time_t now;
        time(&now);
        uint64_t diff = now - last_gen_time_;
        return diff >= last_gen_time_ ? true : false;
    }

    std::string get_file_name()
    {
        time_t now;
        time(&now);
        struct tm local = {0};
        std::localtime_r(&local, &now);
        last_gen_time_ = now;
        char file_name[250] = {0};
        sprintf(file_name, "%d%0.2d%0.2d_%0.2d%0.2d%0.2d.sqlite",
            local.tm_year, local.tm_mon, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
        return std::string(file_name);
    }

private:
    int internal_;
    time_t last_gen_time_;
};