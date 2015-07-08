#ifndef guid_allocator_h__
#define guid_allocator_h__
#include "typedef.hpp"
#define MAX_GUID_INDEX uint64_t(0x0000FFFFFFFFFFFF)

class guid_alloactor
{
public:
    guid_alloactor()
        :seed_(0)
        ,index_(0)
    {}

    guid_alloactor(uint32_t a_seed, uint64_t a_last_index)
        :seed_(a_seed)
        , index_(a_last_index)
    {}

    void init(uint32_t a_seed, uint64_t a_last_index)
    {
        seed_ = a_seed;
        index_ = a_last_index;
    }


    void generate_request(uint32_t a_request_no, uint32_t& a_generate_no, uint64_t& a_start_index)
    {
        a_start_index = index_;
        if((index_ + a_request_no) < MAX_GUID_INDEX)
        {
            a_generate_no = a_request_no;
            index_ += a_request_no;
        }
        else
        {
            index_ = 1;
            a_generate_no = a_request_no;
            index_ += a_request_no;
            ++seed_;
        }
    }

    uint32_t seed_;
    uint64_t index_;
};

#endif // guid_allocator_h__
