#pragma once
#include "typedef.hpp"
#include "config.hpp"
#include <mysql++.h>

class dbreader
    :boost::noncopyable
{
public:
       void run(gce::stackful_actor self, uint32_t  thd_num, const config& cfg)
       {    
           cfg_ = cfg;
           boost::asio::io_service io_service;
           boost::thread_group threads;
           boost::asio::io_service::work wk(io_service);
           for (int i = 0; i < thd_num; i++)
           {
               threads.create_thread(boost::bind(&dbreader::db_load_thread, this, &io_service));
           }
            
       }

       void db_load_thread(boost::asio::io_service& srv)
       {    
           mysqlpp::Connection conn;
           conn.set_option(new mysqlpp::SetCharsetNameOption("utf8"));
           if (!connect_to_db(conn))
           {
               printf("connect the database failed\n");
               return;
           }

           printf("sql client version is <%s>\n", conn.client_version().c_str());

           boost::system::error_code error;
           srv.run_one(error);
       }

       bool connect_to_db(mysqlpp::Connection& conn)
       {
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
};

