// AsioThreadPool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#define NUM_THREAD 6

void test()
{
	printf("hello, world.\n");
	//boost::this_thread::sleep(boost::posix_time::millisec(5000));
}

class dbLoader
{
public:
	bool run(boost::asio::io_service* io_srv)
	{	
		connectDB();
		boost::system::error_code error;
		io_srv->run_one(error);
		io_srv->poll_one(error);
		return true;
	}

	bool connectDB()
	{
		return true;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{	
	boost::asio::io_service io_service;
	boost::system::error_code error;
	boost::thread_group threads;
	for (int i = 0; i < NUM_THREAD - 1; i++)
	{
		dbLoader* loader = new dbLoader;
		threads.create_thread(boost::bind(&dbLoader::run, loader, &io_service));
	}

	threads.join_all();

	return error.value();
}

