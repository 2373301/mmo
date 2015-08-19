#pragma once
#include "typedef.hpp"
#include "lru.hpp"

class entityhandler 
    : boost::noncopyable
{
public:
    void run(gce::stackful_actor self, std::string& service_name, gce::aid_t& saver, uint64_t cache_size)
    {   
        stopped_ = false;
        saver_ = saver;
        lru_.set_size(cache_size);

        try
        {
            gce::register_service(self, service_name.c_str());
            log_ = &self.get_context().get_logger();
            GCE_INFO(*log_) << "hello" << "\n";

            while (!stopped_)
            {
                p::xs2ds_entity_req *req = new p::xs2ds_entity_req;
                gce::aid_t sender = self->match(XS2DS_ENTITY_REQ).recv(*req);

                
                if(req->data.empty())
                {
                    // get

                }
                else
                {
                    // set

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
    void on_get(gce::stackful_actor& self, p::xs2ds_entity_req* req, gce::aid_t sender)
    {
        uint64_t removed_guid = 0;
        if (lru_.update(req->req_guid, removed_guid))
        {

        }
    }

    void on_set(gce::stackful_actor& self, p::xs2ds_entity_req* req, gce::aid_t sender)
    {
        
    }

    bool set(p::xs2ds_entity_req* req)
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

    p::xs2ds_entity_req* get(uint64_t guid)
    {

    }

    void flush(uint64_t guid)
    {

    }

private:
    typedef boost::unordered_map<uint64_t, p::xs2ds_entity_req* > cache_map_t;
    cache_map_t entity_map_;
    bool stopped_;
    gce::log::logger_t* log_;
    gce::aid_t saver_;
    lru lru_;
};

