#pragma once
#include "typedef.hpp"
#include "config.hpp"
#include <mysql++.h>
#include <boost/thread.hpp>
#include <boost/thread/tss.hpp>

class dbreader
    :public gce::addon_t
    ,public boost::noncopyable
{
    typedef gce::addon_t base_t;
public:
    template <typename Actor>
    dbreader(Actor& a)
        :gce::addon_t(a)
    {
    }

    void run(gce::stackful_actor self, const config& cfg)
    {    
        cfg_ = cfg;
        boost::asio::io_service io_service;
        boost::thread_group threads;
        boost::asio::io_service::work wk(io_service);
        for (uint32_t i = 0; i < cfg_.db_thread_num; i++)
        {
            threads.create_thread(boost::bind(&dbreader::db_load_thread, this, &io_service, self.get_aid()));
        }
           
        boost::shared_ptr<p::xs2ds_entity_req> req;
        while (true)
        {   
            gce::aid_t sender;
            self->match(DS2DS_ENTITY_LOAD_REQ).recv(req, sender);
            io_service.post(boost::bind(&dbreader::load, this, req, sender));
        }

    }

    void load(boost::shared_ptr<p::xs2ds_entity_req> req, gce::aid_t sender)
    {      
           std::string table_name;
           get_table_name(req->req_guid, table_name);
           mysqlpp::Query q = conn_->query();
           q << "select guid, body from " << table_name << " where guid = " << req->req_guid;
           const mysqlpp::StoreQueryResult& res = q.store();
           if(0 == res.size())
            return;

           req->data.insert(0, res.at(0).at(1).data(), res.at(0).at(1).length());
           send_back(req, sender);
    }

    bool get_table_name(uint64_t guid, std::string& name)
    {   // todo 临时测试只用
        name = "temp";
        return true;
    }

private:
    void send_back(boost::shared_ptr<p::xs2ds_entity_req>& req, gce::aid_t sender)
    {
        gce::io_service_t& ios = base_t::get_strand().get_io_service();
        ios.post(boost::bind(&dbreader::pri_send_back, this, req, sender));
    }

    void pri_send_back(boost::shared_ptr<p::xs2ds_entity_req>& req, gce::aid_t sender)
    {
        gce::message m;
        m.set_type(DS2DS_ENTITY_LOAD_ACK);
        m << req;
        m << sender;
        base_t::send2actor(m);
    }

    void db_load_thread(boost::asio::io_service* srv, gce::aid_t)
    {   
        conn_.reset(new mysqlpp::Connection);
        conn_->set_option(new mysqlpp::SetCharsetNameOption("utf8"));
        if (!connect_to_db(*conn_))
        {
            printf("connect the database failed\n");
            return;
        }

        printf("sql client version is <%s>\n", conn_->client_version().c_str());

        boost::system::error_code error;
        while (true)
        {
            srv->run_one(error);
        }
    }

    bool connect_to_db(mysqlpp::Connection& conn)
    {   
        boost::mutex::scoped_lock lock(mu_);    // 还的他吗的加mutex
        return connect_to_db(conn, cfg_.db_ip, cfg_.db_port,
            cfg_.db_user, cfg_.db_pwd, cfg_.db_name);
    }

    bool connect_to_db(mysqlpp::Connection& conn, const std::string& host, uint32_t port,
        const std::string& user, const std::string& password, const std::string& database_name)
    {
        try
        {
            if (host.size() == 0 || user.size() == 0 || password.size() == 0)
                return false;

            if (conn.connected())
                return true;

            conn.connect(database_name.c_str(),
                host.c_str(),
                user.c_str(),
                password.c_str(),
                port);
            printf("Connect to db `%s@%s`\n", database_name.c_str(), host.c_str());
            return true;
        }
        catch (const ::std::exception& er)
        {
            printf("Connect mysql db failed: %s\n", er.what());
        }

        return false;
    }

private:
    config  cfg_;
    boost::mutex mu_;
    boost::thread_specific_ptr<mysqlpp::Connection> conn_;  // 线程本地存储
};

