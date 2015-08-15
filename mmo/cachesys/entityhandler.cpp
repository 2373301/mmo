#include "entityhandler.hpp"


void entityhandler::run(gce::stackful_actor self, std::string& service_name)
{
    try
    {
        gce::register_service(self, service_name.c_str());
        gce::log::logger_t& log = self.get_context().get_logger();
        GCE_INFO(log) << "hello" << "\n";
        
        p::xs2ds_entity_req *req = new p::xs2ds_entity_req;
        gce::aid_t sender = self->match(XS2DS_ENTITY_REQ).recv(req);

        p::cache_req_type type;
        if (type.create == req->req_type)
        {

        }
        else if (type.load == req->req_type)
        {
        }
        else if (type.flush == req->req_type)
        {
        }
        else if (type.query == req->req_type)
        {
        }
        else if (type.update == req->req_type)
        {
        }
        else if (type.remove == req->req_type)
        {
        }
        else if (type.one_off == req->req_type)
        {
        }
        else if (type.create_flush == req->req_type)
        {
        }
        else if (type.unload == req->req_type)
        {
        }


        gce::deregister_service(self, service_name.c_str());
    }
    catch (std::exception& ex)
    {
        std::cerr << "entity handler except: " << ex.what() << std::endl;
    }
}


void entityhandler::handle_create(gce::aid_t& sender, p::xs2ds_entity_req& req)
{

}
