///
/// Copyright (c) 2009-2014 Nous Xiong (348944179 at qq dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/nousxiong/gce for latest version.
///

#include <gce/detail/scope.hpp>
#include "opcode.h"
#include "internal/msg_guid.adl.h"

namespace gce
{
class guidsys_ut
{
public:
  static void run()
  {
    std::cout << "service_ut begin." << std::endl;
    for (std::size_t i=0; i<test_count; ++i)
    {
      test_common();
      if (test_count > 1) std::cout << "\r" << i;
    }
    if (test_count > 1) std::cout << std::endl;
    std::cout << "service_ut end." << std::endl;
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

      svcid_t guidsys = make_svcid("guidsys", "guidsys");

      connect(base2, "guidsys", "tcp://127.0.0.1:20000");
      base2.sleep_for(millisecs(100));

      for(int i = 0; i < 1000; i++)
      {
          typed::protocol::xs2gd_guid_req req;
          req.req_no = 5000;
          base2->send(guidsys, XS2GD_GUID_REQ, req);
          typed::protocol::gd2xs_guid_ack ack;
          base2->match(GD2XS_GUID_ACK).recv(ack);
          std::cout << "received, index: " << ack.start_index << std::endl;
      }
    }
    catch (std::exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
  }

  static void echo_service(stackful_actor self)
  {
    try
    {
      register_service(self, "echo_svc");

      while (true)
      {
        message msg;
        aid_t sender = self.recv(msg);
        match_t type = msg.get_type();
        if (type == atom("echo"))
        {
          self.send(sender, msg);
        }
        else
        {
          break;
        }
      }
      deregister_service(self, "echo_svc");
    }
    catch (std::exception& ex)
    {
      std::cerr << "echo except: " << ex.what() << std::endl;
    }
  }
};
}
