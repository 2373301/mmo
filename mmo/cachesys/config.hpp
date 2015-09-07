#ifndef config_h__
#define config_h__
#include "typedef.hpp"
#include <zenxml/xml.h>

struct config
{   
    uint32_t    seed;
    uint64_t    index;
    std::string addr;
    std::string context_name;
    std::string service_name;

    std::string db_name;
    std::string db_user;
    std::string db_pwd;
    std::string db_ip;
    uint32_t    db_port;
    uint32_t    db_thread_num;
};


namespace zen
{
    template <> inline
        void writeStruc(const config& value, XmlElement& output)
    {
        XmlOut out(output);
        out["seed"](value.seed);
        out["index"](value.index);
        out["addr"](value.addr);
        out["context_name"](value.context_name);
        out["service_name"](value.service_name);

        out["db_name"](value.db_name);
        out["db_user"](value.db_user);
        out["db_pwd"](value.db_pwd);
        out["db_ip"](value.db_ip);
        out["db_port"](value.db_port);
        out["db_thread_num"](value.db_thread_num);
    }
    template <> inline
        bool readStruc(const XmlElement& input, config& value)
    {
        XmlIn in(input);
        bool rv = in["seed"](value.seed);
        rv = in["index"](value.index) && rv;
        rv = in["addr"](value.addr) && rv;
        rv = in["context_name"](value.context_name)  && rv;
        rv = in["service_name"](value.service_name)  && rv;

        rv = in["db_name"](value.db_name) && rv;
        rv = in["db_user"](value.db_user) && rv;
        rv = in["db_pwd"](value.db_pwd) && rv;
        rv = in["db_ip"](value.db_ip) && rv;
        rv = in["db_port"](value.db_port) && rv;
        rv = in["db_thread_num"](value.db_thread_num) && rv;
        return rv;
    }
}

bool loadConfig(const std::string& filename, config& cfg)
{
    zen::XmlDoc doc;
    try
    {
        doc = zen::load(filename);
        zen::XmlIn in(doc);
        zen::XmlIn inConfig = in["config"]; 
        zen::readStruc(*inConfig.get(), cfg);
    }
    catch (const zen::XmlError& e) 
    { 
      e;
      return false;
    }
    
    return true;
}

bool saveConfig(const std::string& filename, config& cfg)
{
    try
    {
        zen::XmlDoc doc;
        zen::XmlOut out(doc);
        doc = zen::load(filename);
        out["config"](cfg);
        zen::save(doc, "config.xml");
    }
    catch (const zen::XmlError& e)
    {
        e;
        return false;
    }

    return true;
}


#endif // config_h__
