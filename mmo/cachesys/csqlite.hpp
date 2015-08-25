#pragma once
#include "typedef.hpp"
#include "sqlite3.h"
#include <boost/filesystem.hpp>

struct record
{
    record()
        : guid(0)
        ,stream_size(0)
        ,optype(0)
        ,transaction_id(0)
    {
    }

    ~record()
    {
    }

    uint64_t	            guid;
    uint32_t	            stream_size;
    std::vector<uint8_t>    stream;
    uint32_t	            optype;
    uint64_t                transaction_id;
    std::string             cache_addr;
};

typedef std::vector<record *> record_set_t;

class csqlite
    :boost::noncopyable
{
public:
    csqlite()
        :db_(NULL)
    {
    }

    ~csqlite()
    {
        close();
    }

    bool open(const std::string& file_name)
    {   
        boost::filesystem::path p = boost::filesystem::path(file_name);
        boost::filesystem::path cp = boost::filesystem::system_complete(p);
        int res = sqlite3_open_v2(cp.c_str(), &db_, SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if(SQLITE_OK != res)
            return false;

        res = sqlite3_exec(db_, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
        if(SQLITE_OK != res)
        {
            close();
            return false;
        }

        return true;
    }

    bool invoke(const std::string& command)
    {   
        if( NULL == db_)
            return false;
        char *err_msg = NULL;
        int res = sqlite3_exec(db_, command.c_str(), NULL, NULL, &err_msg);
        if(SQLITE_OK == res)
            return true;

        sqlite3_free(err_msg);
        return false;
    }

    bool read(uint32_t read_number, const std::string& condition, record_set_t& records)
    {
        return false;
    }

    bool begin_transaction()
    {
        return false;
    }

    bool end_transaction()
    {
        return false;
    }

    bool write(const record& rec)
    {
        return false;
    }
    
    void close()
    {
        if(NULL == db_)
            return;
        sqlite3_close(db_);
        db_ = NULL;
    }

    const std::string& file_name()
    {

    }

private:
    std::string file_name_;
    sqlite3* db_;
};