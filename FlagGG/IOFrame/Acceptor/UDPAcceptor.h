#pragma once

#include "Container/List.h"
#include "IOFrame/Acceptor/IOAcceptor.h"
#include "IOFrame/Channel/UDPChannel.h"
#include "IOFrame/Handler/EventHandler.h"
#include "IOFrame/ThreadPool/UDPThreadPool.h"

namespace FlagGG { namespace IOFrame { namespace Acceptor {

class UDPAcceptor : public IOAcceptor
{
public:
	UDPAcceptor(Handler::EventHandlerPtr handler);

	bool Bind(const char* ip, UInt16 port) override;

	void Start() override;

	void Stop() override;

	void WaitForStop() override;

protected:
	void PacketReciver(SLNet::Packet* packet);

	Channel::UDPChannelPtr FindActiveChannel(SLNet::Packet* packet);

	void NewConnection(SLNet::Packet* packet);

	void DeleteConnection(SLNet::Packet* packet);

	void ReciveMessage(SLNet::Packet* packet);

private:
	UDPThreadPoolPtr							threadPool_;

	Handler::EventHandlerPtr					handler_;

	List<Channel::UDPChannelPtr>				activeConn_;
};

}}}
