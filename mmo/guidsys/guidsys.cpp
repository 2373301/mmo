#include "typedef.hpp"
#include "config.hpp"
#include <boost/bind.hpp>
#include "guid_allocator.hpp"

void guid_server(gce::stackful_actor self, std::string& service_name, config& cfg)
{
    try
    {
        gce::register_service(self, service_name.c_str());

        guid_alloactor guid_alloc(cfg.seed, cfg.index);
        while (true)
        {
            typed::protocol::xs2gd_guid_req req;
            gce::aid_t sender  = self->match(XS2GD_GUID_REQ).recv(req);
            
            typed::protocol::gd2xs_guid_ack ack;
            guid_alloc.generate_request(req.req_no, ack.no, ack.start_index);
            ack.seed = guid_alloc.seed_;
            self->send(sender, GD2XS_GUID_ACK, ack);

            cfg.seed = guid_alloc.seed_;
            cfg.index = guid_alloc.index_;
            saveConfig("config.xml", cfg);
        }

        gce::deregister_service(self, service_name.c_str());
    }
    catch (std::exception& ex)
    {
        std::cerr << "guid server except: " << ex.what() << std::endl;
    }
}

int main(int argc, char * argv[])
{   
    config cfg;
    if(!loadConfig("config.xml", cfg))
        return -1;

    try
    {
        gce::log::asio_logger lgr;
        gce::log::logger_t lg = boost::bind(&gce::log::asio_logger::output, &lgr, _arg1, "");
        gce::attributes attr;
        attr.lg_ = lg;
        attr.id_ = gce::atom(cfg.context_name.c_str());
        gce::context ctx(attr);

        gce::threaded_actor base = gce::spawn(ctx);
        gce::bind(base, cfg.addr);
        spawn(base, boost::bind(&guid_server, _arg1, cfg.service_name, cfg), gce::monitored);
        base->recv(gce::exit);
    }
    catch (std::exception& ex)
    {
         std::cerr << "guid server except: " << ex.what() << std::endl;
    }



	return 0;
}

