#pragma  once
#include "typedef.hpp"
#include "csqlite.hpp"
#include "file_name_generator.hpp"
#include <boost/filesystem.hpp>

#define BINARY_DIR  "./binary/"
#define CREATE_RECORD_TABLE "create table record (guid INTEGER, stream_size INTEGER, stream BLOB, optype INTEGER, transaction_id INTEGER, PRIMARY KEY (\"guid\"))"
#define CREATE_RECORD_INDEX "create index index_guid on record(guid)"

class sqlite_serializer
    :public boost::noncopyable
{
public:
    sqlite_serializer()
        :name_generator_(150)
    {}

    void run(gce::stackful_actor self, const std::string dir)
    {
         if(!create_dir(dir))
             return;

         if( !init_serializer(name_generator_.gen()))
             return;

         sqlite_enginer_.begin_transaction();

         std::string new_file;
         while (true)
         {
             expired_item item;
             gce::aid_t sender = self->match("save").recv(item.transcation_id, item.data);
             if(name_generator_.expire())
             {  
                 sqlite_enginer_.end_transaction();
                 std::string cur_file = sqlite_enginer_.file_name();
                 new_file = name_generator_.gen();
                 init_serializer(new_file);
                 sqlite_enginer_.begin_transaction();

                 dispatch_file(cur_file);
             }

             record rec;
             rec.guid = item.data->req_guid;
             rec.stream_size = item.data->data.length();
             rec.stream.swap(item.data->data);
             rec.optype = item.data->req_type;
             rec.transaction_id = item.transcation_id;
             sqlite_enginer_.write(rec);
         }

    }

    void dispatch_file(const std::string& file_name)
    {

    }


    bool init_serializer(const std::string& file_name)
    {
        std::string file_path = BINARY_DIR;
        file_path += file_name;
        sqlite_enginer_.close();
        if (sqlite_enginer_.open(file_path))
        {
            bool result = sqlite_enginer_.invoke(CREATE_RECORD_TABLE);
            result = sqlite_enginer_.invoke(CREATE_RECORD_INDEX) && result;
            return result;
        }
        return false;
    }

    bool create_dir(const std::string &dir)
    {
       	boost::filesystem::path path(dir);
        if (!boost::filesystem::exists(dir))
        {
            return boost::filesystem::create_directory(dir);
        }

	    return true;
    }

private:
    csqlite  sqlite_enginer_;
    file_name_generator name_generator_;
};