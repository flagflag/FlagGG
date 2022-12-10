#pragma once

#include "EventDefine/NetworkEvent.h"
#ifdef FLAGGG_PROTO
#include "Proto/Game.pb.h"
#endif

#include <Core/Context.h>
#include <Container/RefCounted.h>

using namespace FlagGG;

namespace GameProtoEvent
{
#ifdef FLAGGG_PROTO
	DEFINE_EVENT(MESSAGE_RECIVED, void(UInt32,::google::protobuf::Message*));
#endif
}

class GameProtoDistributor : public RefCounted
{
public:
	GameProtoDistributor(Context* context);

	void OnMessageRecived(NetworkType type, IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer);

private:
	Context* context_;
};
