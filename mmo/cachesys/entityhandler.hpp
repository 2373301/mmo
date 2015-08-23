#pragma once
#include "typedef.hpp"
#include "lru.hpp"
#include "counter.hpp"

struct expired_item 
{   
    uint64_t transcation_id;
    std::shared_ptr<p::xs2ds_entity_req> data;
};

class entityhandler 
    : boost::noncopyable
{
public:
    void run(gce::stackful_actor self, std::string& service_name, gce::aid_t& saver, uint64_t cache_size, gce::aid_t& dbloader)
    {   
        stopped_ = false;
        saver_ = saver;
        dbloader_ = dbloader;
        lru_.set_size(cache_size);
        
        try
        {
            gce::register_service(self, service_name.c_str());
            log_ = &self.get_context().get_logger();
            GCE_INFO(*log_) << "hello" << "\n";

            while (!stopped_)
            {
                std::shared_ptr<p::xs2ds_entity_req> req(new p::xs2ds_entity_req);
                gce::aid_t sender = self->match(XS2DS_ENTITY_REQ).recv(*req);

                p::entity_req_type type;
                if(type.get == req->req_type)
                {
                    // get
                    on_get(self, req, sender);

                }
                else if(type.set == req->req_type)
                {
                    // set
                    on_set(self, req, sender);
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
    void on_get(gce::stackful_actor self, std::shared_ptr<p::xs2ds_entity_req> req, gce::aid_t sender)
    {   
        p::ds2xs_entity_ack ack;
        ack.req_guid = req->req_guid;
        ack.req_type = req->req_type;

        uint64_t req_guid = req->req_guid;
        std::shared_ptr<p::xs2ds_entity_req> existed;
        bool need_reg = false;
        do 
        {   
            // cache里有的
            auto it = entity_map_.find(req_guid);
            if( it != entity_map_.end())
            {
                existed = it->second;
                break;
            }

            // 待删除列表里有的
            auto expired_it = expired_map_.find(req_guid);
            if( expired_it != expired_map_.end())
            {
                existed = expired_it->second.data;
                entity_map_.insert(std::make_pair(req_guid, existed));
                expired_map_.erase(expired_it);
                need_reg = true;
                break;
            }

            // 需要 db 加载
            gce::resp_t res = self->request(dbloader_, XS2DS_ENTITY_REQ, *req);
            self->respond(res, *req);
            if(!req->data.empty())
            {
                existed = req;
                entity_map_.insert(std::make_pair(req_guid, existed));
                need_reg = true;
                break;
            }

            // db 加载失败
           ack.result = 1;
           self->send(sender, DS2XS_ENTITY_ACK, ack);
           return;

        } while (false);

        ack.data = req->data; // 复制
        self->send(sender, DS2XS_ENTITY_ACK, ack);
        if( !need_reg)
            return;

        uint64_t removed_guid = 0;
        if (!lru_.update(req_guid, removed_guid))
            return;

        auto it = entity_map_.find(removed_guid);
        if (it == entity_map_.end())
            return;
        
        expired_item item;
        item.transcation_id = counter_.now();
        item.data->data.swap(it->second->data);
        entity_map_.erase(it);
        auto expired_it = expired_map_.find(removed_guid);
        if (expired_it != expired_map_.end())
        {
            expired_map_.erase(expired_it);
        }
        auto inserted = expired_map_.insert(std::make_pair(removed_guid, item));
        inserted->first->second = it->second->data;
    }

    void on_set(gce::stackful_actor self, std::shared_ptr<p::xs2ds_entity_req> req, gce::aid_t sender)
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
    typedef boost::unordered_map<uint64_t, std::shared_ptr<p::xs2ds_entity_req> > cache_map_t;
    typedef boost::unordered_map<uint64_t, expired_item> expired_map_t;
    cache_map_t entity_map_;    // cache
    bool stopped_;
    gce::log::logger_t* log_;
    gce::aid_t saver_;          // 序列化
    gce::aid_t dbloader_;       //
    lru lru_;                   // 固定cache的大小, 多余的flush
    counter counter_;           // 自增 唯一 id
    expired_map_t expired_map_; // 
};

