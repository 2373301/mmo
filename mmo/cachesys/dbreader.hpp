#pragma once
#include "typedef.hpp"

class dbreader
    :boost::noncopyable
{
public:
       void run(gce::stackful_actor self, uint32_t  thd_num)
       {    
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
           boost::system::error_code error;
           srv.run(error);
       }

};

