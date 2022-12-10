#pragma once

#include "AsyncFrame/Thread/UniqueThread.h"
#include "IOFrame/ThreadPool/IOThreadPool.h"

#include <SLikeNet/RakPeerInterface.h>
#include <atomic>
#include <functional>

namespace FlagGG { namespace IOFrame {

class UDPThreadPool : public IOThreadPool
{
public:
	UDPThreadPool();

	~UDPThreadPool() override;

	void Start() override;

	void Stop() override;

	void WaitForStop() override;

	SLNet::RakPeerInterface* getService();

	void SetPacketReciver(std::function<void(SLNet::Packet* packet)> func);

private:
	void NetThread();

	void WaitForStop(UInt32 wait_time) override { };

private:
	SLNet::RakPeerInterface*					rakPeer_;

	UniqueThreadPtr								thread_;

	std::atomic < bool >						running_;

	std::function<void(SLNet::Packet* packet)> reciver_;
};

typedef SharedPtr<UDPThreadPool> UDPThreadPoolPtr;
	
}}
