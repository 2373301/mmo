#pragma once
#include "typedef.hpp"
#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>

class entityhandler 
    : boost::noncopyable
{
public:
    void run(gce::stackful_actor self, std::string& service_name, gce::aid_t& );
 
private:
    void handle_create(gce::aid_t& sender, p::xs2ds_entity_req& req);

private:
    typedef boost::unordered_map<uint64_t, std::vector< uint8_t > > cache_map_t;
    cache_map_t entity_map_;
};

