#ifndef FLAGGG_NO_BOOST
#pragma once

#include <boost/asio/io_service.hpp>

#include <atomic>

#include "AsyncFrame/Thread/UniqueThread.h"
#include "IOThreadPool.h"

namespace FlagGG { namespace IOFrame {

class NetThreadPool : public IOThreadPool
{
public:
	NetThreadPool(USize threadCount);

	~NetThreadPool() override = default;

	void Start() override;

	void Stop() override;

	void WaitForStop() override;

	boost::asio::io_service& getService();

private:
	void NetThread();

	void WaitForStop(UInt32 wait_time) override { };

private:
	USize												threadCount_;

	boost::asio::io_service								service_;

	std::vector<UniqueThreadPtr>						threadGroup_;

	std::atomic<bool>									running_;
};

typedef SharedPtr<NetThreadPool> NetThreadPoolPtr;

}}
#endif
