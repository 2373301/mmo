#pragma once
#include "typedef.hpp"
#include <boost/unordered_map.hpp>

class entityhandler
{
public:
    entityhandler();
    ~entityhandler();
    void handle_req(gce::aid_t& sender, p::xs2ds_entity_req& req);
    
private:
    void handle_create(gce::aid_t& sender, p::xs2ds_entity_req& req);

private:
    typedef boost::unordered_map<uint64_t, std::vector< uint8_t > > cache_map_t;
    cache_map_t entity_map_;
};

