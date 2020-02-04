#include "Network/GameProtoDistributor.h"

GameProtoDistributor::GameProtoDistributor(Context* context) :
	context_(context)
{
	context_->RegisterEvent(EVENT_HANDLER(NetworkEvent::MESSAGE_RECIVED, GameProtoDistributor::OnMessageRecived, this));
}

#define IF_DO(MessageType, ProtoType) \
	case MessageType: \
	{ \
		ProtoType proto; \
		proto.ParseFromString(header.message_body()); \
		context_->SendEvent<GameProtoEvent::MESSAGE_RECIVED_HANDLER>(GameProtoEvent::MESSAGE_RECIVED, header.message_type(), &proto); \
	} \
	break

void GameProtoDistributor::OnMessageRecived(NetworkType type, IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer)
{
	String bufferContent;
	buffer->ToString(bufferContent);
	
	Proto::Game::MessageHeader header;
	header.ParseFromString(std::string(bufferContent.CString(), bufferContent.Length()));

	switch (header.message_type())
	{
		IF_DO(Proto::Game::MessageType_ResponseLogin, Proto::Game::ResponseLogin);
		IF_DO(Proto::Game::MessageType_AppearUnit, Proto::Game::NotifyUnitAppear);
		IF_DO(Proto::Game::MessageType_DisappearUnit, Proto::Game::NotifyUnitDisappear);
		IF_DO(Proto::Game::MessageType_SyncUnitTansform, Proto::Game::NotifySyncUnitTansform);
		IF_DO(Proto::Game::MessageType_SyncUnitStatus, Proto::Game::NotifySyncUnitStatus);
		IF_DO(Proto::Game::MessageType_SyncUnitAttribute, Proto::Game::NotifySyncUnitAttribute);
		IF_DO(Proto::Game::MessageType_SyncUnitMovement, Proto::Game::NotifySyncUnitMovement);
		IF_DO(Proto::Game::MessageType_UnitCastSpell, Proto::Game::NotifyUnitCastSpell);
		IF_DO(Proto::Game::MessageType_UnitAttachBuff, Proto::Game::NotifyUnitAttachBuff);
		IF_DO(Proto::Game::MessageType_UnitDettachBuff, Proto::Game::NotifyUnitDettachBuff);
	}
}
