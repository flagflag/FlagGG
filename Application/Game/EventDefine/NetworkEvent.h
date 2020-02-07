#pragma once

#include <Core/EventDefine.h>
#include <IOFrame/IOFrame.h>
#include <Container/Str.h>

enum NetworkType
{
	NETWORK_TYPE_TCP = 0,
	NETWORK_TYPE_UDP,
	NETWORK_TYPE_WEB,
	NETWORK_TYPE_MAX,
};

static const char* NETWORK_TYPE_NAME[] = {
	"TCPNetwork",
	"UDPNetwork",
	"WebNetwork",
};


using namespace FlagGG;
using namespace FlagGG::Container;

namespace NetworkEvent
{
	DEFINE_EVENT(OPEND, void(NetworkType, IOFrame::Context::IOContextPtr));
	DEFINE_EVENT(CLOSED, void(NetworkType, IOFrame::Context::IOContextPtr));
	DEFINE_EVENT(CATCH_ERROR, void(NetworkType, IOFrame::Context::IOContextPtr, int, String));
	DEFINE_EVENT(MESSAGE_RECIVED, void(NetworkType, IOFrame::Context::IOContextPtr, IOFrame::Buffer::IOBufferPtr));
}
