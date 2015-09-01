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
