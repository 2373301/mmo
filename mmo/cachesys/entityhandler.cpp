#include "entityhandler.h"


entityhandler::entityhandler()
{
}


entityhandler::~entityhandler()
{
}

void entityhandler::handle_req(gce::aid_t& sender, p::xs2ds_entity_req& req)
{
    gce::aid_t;
    p::cache_req_type type;
    if( type.create == req.req_type)
    {

    }
}

void entityhandler::handle_create(gce::aid_t& sender, p::xs2ds_entity_req& req)
{

}
