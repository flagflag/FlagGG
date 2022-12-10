#ifndef FLAGGG_NO_BOOST
#pragma once

#include "IOContext.h"

namespace FlagGG { namespace IOFrame { namespace Context {

class TCPContext : public IOContext
{
public:
	TCPContext(Channel::IOChannelPtr channel);

	~TCPContext() override = default;

	Channel::IOChannelPtr GetChannel() override;

private:
	Channel::IOChannelPtr channel_;
};

typedef SharedPtr<TCPContext> TCPContextPtr;
	
}}}
#endif
