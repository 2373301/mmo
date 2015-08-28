#pragma  once
#include "typedef.hpp"
#include "csqlite.hpp"
#include "file_name_generator.hpp"
#include <boost/filesystem.hpp>

class sqlite_serializer
    :boost::noncopyable
{
public:
    void run(gce::stackful_actor self)
    {

    }

    bool create_dir(const std::string &dir)
    {
       return boost::filesystem::create_directory(dir);
    }

private:
    csqlite  sqlite_enginer_;
    file_name_generator name_generator_;
};