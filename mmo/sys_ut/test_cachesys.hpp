#include <gce/detail/scope.hpp>
#include "opcode.h"
#include "internal/msg_guid.adl.h"
#include "internal/msg_cache.adl.h"
#include <iostream>
#include <gce/log/all.hpp>

namespace gce
{
class cachesys_ut
{
    static std::size_t const test_count = 1 << 20;

public:
  static void run()
  {
    std::cout << "cachesys_ut begin." << std::endl;
    test_common();
    std::cout << "cachesys_ut end." << std::endl;
  }

private:
  static void test_common()
  {
    log::asio_logger lgr;
    log::logger_t lg = boost::bind(&gce::log::asio_logger::output, &lgr, _arg1, "");
    try
    {
      std::size_t echo_num = 100;

      attributes attrs;
      attrs.lg_ = lg;
      attrs.id_ = atom("two");
      context ctx2(attrs);
      threaded_actor base2 = spawn(ctx2);

      svcid_t guidsys = make_svcid("cachesys", "cachesys");

      connect(base2, "cachesys", "tcp://127.0.0.1:20001");
      base2.sleep_for(millisecs(100));

      for(int i = 0; i < test_count; i++)
      {
          p::xs2ds_entity_req req;
          req.req_guid = 1;
          base2->send(guidsys, XS2DS_ENTITY_REQ, req);
          p::ds2xs_entity_ack ack;
          base2->match(DS2XS_ENTITY_ACK).recv(ack);
          //std::cout << "received, index: " << ack.data << std::endl;
          GCE_INFO(lg) << ack.data<< "\n";
      }
       GCE_INFO(lg) << test_count<< "\n";
    }
    catch (std::exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
  }


};

}
