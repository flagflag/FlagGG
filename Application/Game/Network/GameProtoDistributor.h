#pragma once

#include "Network/NetworkAdaptor.h"
#include "Proto/Game.pb.h"

namespace GameProtoEvent
{
	DEFINE_EVENT(MESSAGE_RECIVED, void(UInt32,::google::protobuf::Message*));
}

class GameProtoDistributor : public RefCounted
{
public:
	GameProtoDistributor(Context* context);

	void OnMessageRecived(NetworkType type, IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer);

private:
	Context* context_;
};
