#pragma once
#include "typedef.hpp"
#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>

class entityhandler 
    : boost::noncopyable
{
public:
    void run(gce::stackful_actor self, std::string& service_name, gce::aid_t& saver)
    {   
        stopped_ = false;
        saver_ = saver;

        try
        {
            gce::register_service(self, service_name.c_str());
            log_ = &self.get_context().get_logger();
            GCE_INFO(*log_) << "hello" << "\n";

            while (!stopped_)
            {
                p::xs2ds_entity_req *req = new p::xs2ds_entity_req;
                gce::aid_t sender = self->match(XS2DS_ENTITY_REQ).recv(req);

                p::cache_req_type type;
                if (type.create == req->req_type)
                {
                    on_create(self, req, sender);
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
            }


            gce::deregister_service(self, service_name.c_str());
        }
        catch (std::exception& ex)
        {
            std::cerr << "entity handler except: " << ex.what() << std::endl;
        }
    }
 
private:
    void on_create(gce::stackful_actor self, p::xs2ds_entity_req* req, gce::aid_t sender)
    {
        if(add_entity(req))
        {
            self->send(saver_)
        }
    }

    bool add_entity(p::xs2ds_entity_req* req)
    {
        if(req->data.empty())
        {
            GCE_INFO(*log_) << "entity size is zero: " << req->req_guid  << "\n";
            return false;
        }

        auto it = entity_map_.find(req->req_guid);
        if( it != entity_map_.end())
        {
            GCE_INFO(*log_) << "failed to add entity with reduplicate guid: " << req->req_guid << "\n";
            return false;
        }

       entity_map_.insert(std::make_pair(req->req_guid, req));
       return true;
    }

private:
    typedef boost::unordered_map<uint64_t, p::xs2ds_entity_req* > cache_map_t;
    cache_map_t entity_map_;
    bool stopped_;
    gce::log::logger_t* log_;
    gce::aid_t saver_;
};

