#pragma once
#include "typedef.hpp"
#include "lru.hpp"
#include "counter.hpp"

class entity_handler 
    : public boost::noncopyable
{
public:
    entity_handler() {}

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
                gce::pattern ptn(XS2DS_ENTITY_REQ, DS2DS_ENTITY_LOAD_ACK);
                gce::message msg;
                gce::aid_t sender = self.recv(msg, ptn);
                gce::match_t id = msg.get_type();
                gce::aid_t client;
                if (XS2DS_ENTITY_REQ == id.val_)
                {   
                    // 自个申请, 用值
                    boost::shared_ptr<p::xs2ds_entity_req> req(new p::xs2ds_entity_req);
                    msg >> *req;
                    on_get(self, req, sender);
                }
                else if(DS2DS_ENTITY_LOAD_ACK == id.val_)
                {   
                     // 不需申请, 用指针
                     boost::shared_ptr<p::xs2ds_entity_req> req;
                     msg >> req;
                     msg >> client;
                     on_load(self, req, client);
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
    void on_load(gce::stackful_actor& self, boost::shared_ptr<p::xs2ds_entity_req>& req, gce::aid_t& client)
    {
        p::ds2xs_entity_ack ack;
        ack.req_guid = req->req_guid;
        ack.req_type = req->req_type;
        do 
        {
            if (req->data.empty())
            {    // db 加载失败
                ack.result = 1;
                break;
            }

            entity_map_.insert(std::make_pair(ack.req_guid, req));
            ack.data = req->data; // 复制
       
            uint64_t removed_guid = 0;
            if (!lru_.update(req->req_guid, removed_guid))
                break;

            auto it = entity_map_.find(removed_guid);
            if (it == entity_map_.end())
                break;

            expired_item item;
            item.transcation_id = counter_.now();  // id 用于比较, 以防重复删除,或者误删
            item.data->data.swap(it->second->data);
            entity_map_.erase(it);
            auto expired_it = expired_map_.find(removed_guid);
            if (expired_it != expired_map_.end())
            {
                expired_map_.erase(expired_it);
            }
            expired_map_.insert(std::make_pair(removed_guid, item));

            // 过期的, 发送给保存actor
            self->send(saver_, DS2DS_ENTITY_SERIALIZE_REQ, item.transcation_id, item.data);

        } while (false);

        self->send(client, DS2XS_ENTITY_ACK, ack);
    }

    void on_get(gce::stackful_actor& self, boost::shared_ptr<p::xs2ds_entity_req>& req, gce::aid_t& sender)
    {   
        p::ds2xs_entity_ack ack;
        ack.req_guid = req->req_guid;
        ack.req_type = req->req_type;

        uint64_t req_guid = req->req_guid;
        boost::shared_ptr<p::xs2ds_entity_req> existed;
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
                break;
            }
                
            // 需要 db 加载
            self->send(dbloader_, DS2DS_ENTITY_LOAD_REQ, req, sender);
            return; // 返回

        } while (false);


        // 返回消息
        ack.data = existed->data;
        self->send(sender, DS2XS_ENTITY_ACK, ack);
    }

    void on_set(gce::stackful_actor& self, boost::shared_ptr<p::xs2ds_entity_req>& req, gce::aid_t& sender)
    {
        p::ds2xs_entity_ack ack;
        ack.req_guid = req->req_guid;
        ack.req_type = req->req_type;

        uint64_t req_guid = req->req_guid;
        do 
        {   
            if( req->data.empty())
            {
                ack.result = 1;
                break;
            }

            // cache里有的
            auto it = entity_map_.find(req_guid);
            if (it != entity_map_.end())
            {
                it->second = req;
            }
            else
            {
                entity_map_.insert(std::make_pair(req_guid, req));
            }

            // 保存, 0 代表非过期的数据
            self->send(saver_, DS2DS_ENTITY_SERIALIZE_REQ, 0, req->data);
        } while (false);

        // 返回消息
        self->send(sender, DS2XS_ENTITY_ACK, ack);
    }

private:
    typedef boost::unordered_map<uint64_t, boost::shared_ptr<p::xs2ds_entity_req> > cache_map_t;
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

