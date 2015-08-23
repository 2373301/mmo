#pragma once
#include "typedef.hpp"

struct counter
{
    counter()
    {
        time_t t;
        time(&t);
        start_time_ = (uint32_t)t;
        index_ = 0;
    }

    uint64_t now()
    {
        if(UINT_MAX == index_)
        {
            time_t t;
            time(&t);
            start_time_ = (uint32_t)t;
            index_ = 0;
        }

        uint64_t n = 0;
        n = start_time_;
        n << 32;
        n += index_;

        return n;
    }


    uint32_t start_time_;
    uint32_t index_;
};