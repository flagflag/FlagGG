#ifndef FLAGGG_NO_BOOST
#include "NetThreadPool.h"
#include "Utility/SystemHelper.h"

namespace FlagGG { namespace IOFrame {

NetThreadPool::NetThreadPool(USize threadCount)
	: running_(false)
	, threadCount_(threadCount)
{ }

void NetThreadPool::Start()
{
	running_ = true;

	for (USize i = 0; i < threadCount_; ++i)
	{
		threadGroup_.emplace_back(new UniqueThread(std::bind(&NetThreadPool::NetThread, this)));
	}
}

void NetThreadPool::Stop()
{
	running_ = false;
}

void NetThreadPool::WaitForStop()
{
	for (USize i = 0; i < threadGroup_.size(); ++i)
	{
		threadGroup_[i]->WaitForStop();
	}
}

boost::asio::io_service& NetThreadPool::getService()
{
	return service_;
}

void NetThreadPool::NetThread()
{
	while (running_)
	{
		Sleep(0);

		service_.run();
	}
}

}}
#endif
