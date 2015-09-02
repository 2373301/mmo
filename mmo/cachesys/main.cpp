#include "typedef.hpp"
#include "config.hpp"
#include <boost/bind.hpp>
#include "dbreader.hpp"

void guid_server(gce::stackful_actor self, std::string& service_name, config& cfg, gce::aid_t saver)
{
    try
    {
        gce::register_service(self, service_name.c_str());

        for(uint64_t i = 0;; i++)
        {   
            gce::pattern patt(gce::zero);
            gce::message msg;
            gce::aid_t sender = self.recv(msg, patt);
            if (sender == gce::aid_nil)
                continue;

//             gce::errcode_t ec;
//             self->match("timeout").timeout(gce::zero, ec).recv();
//             if(ec)
//                 continue;

            self->send(saver, "update", i, i);  
            gce::log::logger_t& log = self.get_context().get_logger();
            GCE_INFO(log) << "hello"  << i << "\n";
            
        }

        gce::deregister_service(self, service_name.c_str());
    }
    catch (std::exception& ex)
    {
        std::cerr << "guid server except: " << ex.what() << std::endl;
    }
}

void timeout_actor(gce::stackful_actor self, gce::aid_t guid_actor)
{
    try
    {
        while (true)
        {
            gce::timer_t tmr(self.get_context().get_io_service());
            gce::yield_t yield = self.get_yield();
            tmr.expires_from_now(boost::chrono::microseconds(10));
            tmr.async_wait(yield);
            self->send(guid_actor, "timeout");
        }   
    }
    catch (std::exception& ex)
    {
        std::cerr << "failed to run timeout actor: " << ex.what() << std::endl;
    }
}

void config_saver(gce::stackful_actor self, config cfg)
{
    try
    {   
        uint32_t seed = 0;
        uint64_t index = 0;
        while (true)
        {
            self->match("update").recv(seed, index);
            cfg.seed = seed;
            cfg.index = index;
            saveConfig("config.xml", cfg);
        }
    }
    catch(std::exception& ex)
    {
        std::cerr << "failed to save config: " << ex.what() << std::endl;
    }
}

int main(int argc, char * argv[])
{
    config cfg;
    if (!loadConfig("config.xml", cfg))
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
        gce::aid_t saver = spawn(base, boost::bind(&config_saver, _arg1, cfg), gce::monitored);
        dbreader db_reader(base);
        gce::aid_t guid = spawn(base, boost::bind(&guid_server, _arg1, cfg.service_name, cfg, saver), gce::monitored); 
        gce::aid_t db_reader_guid = spawn(base, boost::bind(&dbreader::run, &db_reader, _arg1, cfg), gce::monitored); 
        spawn(base, boost::bind(&timeout_actor, _arg1, guid), gce::monitored);
        base->recv(gce::exit);
    }
    catch (std::exception& ex)
    {
        std::cerr << "guid server except: " << ex.what() << std::endl;
    }



    return 0;
}

