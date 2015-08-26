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
    std::string             stream;
    uint32_t	            optype;
    uint64_t                transaction_id;
    std::string             cache_addr;
};

#define  INSERT_RECORD_SQL "insert into record(guid, stream_size, stream, optype, transaction_id) values(?, ?, ?, ?, ?)"

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

    // 有结果才true, 失败或空则为false
    bool read(uint32_t read_number, const std::string& condition, record_set_t& records)
    {   
        if( NULL == db_)
            return false;

        sqlite3_stmt *stmt = NULL;
        std::stringstream oss;
        oss << "select guid, stream_size, stream, optype, transaction_id from record ";
        if(condition.length())
            oss << condition;

        int res = sqlite3_prepare(db_, oss.str().c_str(), -1, &stmt, NULL);
        if(SQLITE_OK != res)
            return false;

        int cur_read = 0;
        for (int s = sqlite3_step(stmt); 
            cur_read < read_number && SQLITE_ROW == s;
            ++cur_read, s = sqlite3_step(stmt)
            )
        {
            record * one = new record;
            one->guid = sqlite3_column_int64(stmt, 0);
            one->stream_size = sqlite3_column_int(stmt, 1);
            one->stream = std::string((char*)sqlite3_column_blob(stmt, 2), one->stream_size);
            one->optype = sqlite3_column_int(stmt, 3);
            one->transaction_id = sqlite3_column_int64(stmt, 4);
            records.push_back(one);
        }

        sqlite3_finalize(stmt);
        return 0 != records.size();
    }

    void begin_transaction()
    {   
        sqlite3_exec(db_, "begin transaction;", NULL, NULL, NULL);
    }

    void end_transaction()
    {
       sqlite3_exec(db_, "commit transaction;", NULL, NULL, NULL);
    }

    bool write(const record& rec)
    {   
        if(NULL == db_)
            return false;

        sqlite3_stmt* stmt = NULL;
        sqlite3_prepare(db_, INSERT_RECORD_SQL, -1, &stmt, 0);
        sqlite3_bind_int64(stmt, 1, rec.guid);
        sqlite3_bind_int(stmt, 2, rec.stream_size);
        sqlite3_bind_blob(stmt, 3, rec.stream.data(), rec.stream_size, 0);
        sqlite3_bind_int(stmt, 4, rec.optype);
        sqlite3_bind_int64(stmt, 5, rec.transaction_id);
        int res = sqlite3_finalize(stmt);
        return SQLITE_DONE == res;
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
        return file_name_;
    }

private:
    std::string file_name_;
    sqlite3* db_;
};