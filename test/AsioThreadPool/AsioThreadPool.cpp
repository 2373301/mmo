// AsioThreadPool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Windows.h>
#define NUM_THREAD 5

int g_sum = 0;
void test(int i)
{
  ++g_sum;
  printf("%d\n", GetCurrentThreadId());
}

class dbLoader
{
public:
	bool run(boost::asio::io_service::strand *t)
	{	
		connectDB();
		boost::system::error_code error;
		while(true)
		{
			t->get_io_service().run_one(error);
		}
   
		//t->get_io_service().run(error);
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
	boost::asio::io_service::work wk(io_service);
	typedef boost::asio::io_service::strand myStrand;
	std::vector<myStrand*> vec;

	for (int i = 0; i <= NUM_THREAD - 1; i++)
	{
		dbLoader* loader = new dbLoader;
		boost::asio::io_service::strand *t = new boost::asio::io_service::strand(io_service);
		threads.create_thread(boost::bind(&dbLoader::run, loader, t));
		vec.push_back(t);
	}
  
  
  for(std::vector<myStrand*>::iterator it = vec.begin(); it != vec.end(); ++it)
  {
    for (int i = 0; i != 10; i++ )
    {
      (*it)->post(boost::bind(&test, i));
    }  
  }
	threads.join_all();

	return error.value();
}



