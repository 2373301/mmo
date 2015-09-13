#include "typedef.hpp"
#include "config.hpp"
#include <boost/bind.hpp>
#include "dbreader.hpp"
#include "sqlite_serializer.hpp"
#include "entityhandler.hpp"

void main_actor(gce::stackful_actor self, config& cfg)
{
    try
    {
        dbreader reader(self);
        gce::aid_t db_reader_actor = spawn(self, boost::bind(&dbreader::run, &reader, _arg1, cfg), gce::monitored);

        sqlite_serializer serializer;
        gce::aid_t saver = spawn(self, boost::bind(&sqlite_serializer::run, &serializer, _arg1, "binary"), gce::monitored);

        entity_handler handler;
        gce::aid_t entity_aid = spawn(self, boost::bind(&entity_handler::run, 
            &handler, _arg1, cfg.service_name, saver, cfg.cache_size, db_reader_actor), gce::monitored);

        boost::shared_ptr<p::xs2ds_entity_req> req;
//         req->req_guid = 1;
//         self->send(db_reader_actor, XS2DS_ENTITY_REQ, req);
        gce::message msg;
        gce::aid_t sender;
        while (true)
        {
            self.recv(msg);
            msg >> req;
            msg >> sender;
            self->send(sender, DS2XS_ENTITY_ACK, req);
        }
        self->recv(gce::exit);
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
    catch (std::exception& ex)
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
        
        gce::aid_t guid = spawn(base, boost::bind(&main_actor, _arg1, cfg), gce::monitored); 
       // spawn(base, boost::bind(&timeout_actor, _arg1, guid), gce::monitored);
        
        base->recv(gce::exit);
    }
    catch (std::exception& ex)
    {
        std::cerr << "guid server except: " << ex.what() << std::endl;
    }



    return 0;
}

