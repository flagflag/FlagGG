#pragma once

#include "IOFrame/Channel/IOChannel.h"
#include "Export.h"

namespace FlagGG { namespace IOFrame { namespace Context {

class FlagGG_API IOContext : public RefCounted
{
public:
	virtual ~IOContext() = default;

	virtual Channel::IOChannelPtr GetChannel() = 0;
};

typedef SharedPtr < IOContext > IOContextPtr;

}}}
