#include "Network/LuaEventAdaptor.h"
#include "Proto/Game.pb.h"

#include <Core/EventManager.h>
#include <Graphics/Window.h>
#include <Graphics/RenderEngine.h>
#include <Graphics/Batch2D.h>
#include <Graphics/Texture2D.h>

LuaEventAdaptor::LuaEventAdaptor(NetworkSender* sender) :
	sender_(sender)
{
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::END_FRAME, LuaEventAdaptor::EndFrameUpdate, this));
}

void LuaEventAdaptor::OnKeepAlive()
{

}

void LuaEventAdaptor::OnStartGame(const char* gameName)
{
	
}

void LuaEventAdaptor::OnStopGame()
{

}

static Proto::Game::Vector3D* ToProtoVector3(const Vector3& vec)
{
	Proto::Game::Vector3D* protoVec = new Proto::Game::Vector3D();
	protoVec->set_x(vec.x_);
	protoVec->set_y(vec.y_);
	protoVec->set_z(vec.z_);
	return protoVec;
}

static Proto::Game::Quaternion* ToProtoQuaternion(const Quaternion& qua)
{
	Proto::Game::Quaternion* protoQua = new Proto::Game::Quaternion();
	protoQua->set_x(qua.x_);
	protoQua->set_y(qua.y_);
	protoQua->set_z(qua.z_);
	protoQua->set_w(qua.w_);
	return protoQua;
}

static Proto::Game::Transform* ToProtoTransform(LuaGameEngine::Unit* unit)
{
	Proto::Game::Vector3D* vec = ToProtoVector3(unit->GetPosition());
	Proto::Game::Quaternion* qua = ToProtoQuaternion(unit->GetRotation());
	Proto::Game::Transform* trans = new Proto::Game::Transform();
	trans->set_allocated_position(vec);
	trans->set_allocated_rotation(qua);
	return trans;
}

static Proto::Game::UnitAttribute* ToProtoAttribute(const LuaGameEngine::Attribute& attr)
{
	Proto::Game::UnitAttribute* protoAttr = new Proto::Game::UnitAttribute();
	protoAttr->set_move_speed(attr.moveSpeed_);
	protoAttr->set_attack_speed(attr.attackSpeed_);
	return protoAttr;
}

static void ToProtoMovementInfo(const BaseMovement* movement, Proto::Game::UnitMovementInfo* protoMovement)
{
	Proto::Game::Vector3D* vec = ToProtoVector3(Vector3::ZERO);
	Proto::Game::Quaternion* qua = ToProtoQuaternion(Quaternion::IDENTITY);
	Proto::Game::Transform* startTrans = new Proto::Game::Transform();
	startTrans->set_allocated_position(vec);
	startTrans->set_allocated_rotation(qua);
	Proto::Game::Transform* endTrans = new Proto::Game::Transform();
	endTrans->CopyFrom(*startTrans);
	protoMovement->set_allocated_start_point(startTrans);
	protoMovement->set_allocated_end_point(endTrans);
	protoMovement->set_move_type(Proto::Game::MoveType_Line);
}

static Proto::Game::SpellInfo* ToProtoSpellInfo(LuaGameEngine::Spell* spell)
{
	Proto::Game::SpellInfo* protoSpell = new Proto::Game::SpellInfo();
	// TO DO
	// ......
	return protoSpell;
}

static void ToProtoBuffInfo(LuaGameEngine::Buff* buff, Proto::Game::BuffInfo* protoBuff)
{
	// TO DO
	// ......
}

void LuaEventAdaptor::OnAppearUnit(Int64 unitId, LuaGameEngine::Unit* unit)
{
	Proto::Game::NotifyUnitAppear notify;

	Proto::Game::Transform* trans = ToProtoTransform(unit);
	Proto::Game::UnitAttribute* attr = ToProtoAttribute(unit->GetAttribute());
	Proto::Game::SpellInfo* spellInfo = ToProtoSpellInfo(unit->GetCurrentSpell());
	Proto::Game::BuffInfoList* buffInfoList = new Proto::Game::BuffInfoList();
	for (UInt32 i = 0; i < unit->GetNumBuff(); ++i)
	{
		Proto::Game::BuffInfo* buffInfo = buffInfoList->add_buff_infos();
		ToProtoBuffInfo(unit->GetBuff(i), buffInfo);
	}

	notify.set_unit_id(unitId);
	notify.set_allocated_transform(trans);
	notify.set_status(unit->GetStatus());
	notify.set_asset_id(unit->GetAssetId());
	notify.set_allocated_attribute(attr);
	if (auto* allMovements = unit->GetAllMovements())
	{
		for (auto& movement : *allMovements)
		{
			Proto::Game::UnitMovementInfo* moveInfo = notify.add_movements();
			ToProtoMovementInfo(movement, moveInfo);
		}
	}
	notify.set_allocated_spell(spellInfo);
	notify.set_allocated_buff(buffInfoList);

	sender_->Send(0u, Proto::Game::MessageType_AppearUnit, &notify);
}

void LuaEventAdaptor::OnDisappearUnit(Int64 unitId)
{

}

void LuaEventAdaptor::OnSyncUnitTansform(Int64 unitId, LuaGameEngine::Transform* transform)
{

}

void LuaEventAdaptor::OnSyncUnitStatus(Int64 unitId, LuaGameEngine::Status status)
{

}

void LuaEventAdaptor::OnSyncUnitAttribute(Int64 unitId, LuaGameEngine::Attribute* attribute)
{

}

void LuaEventAdaptor::OnSyncUnitMovement(Int64 unitId, LuaGameEngine::Movement* movement)
{

}

void LuaEventAdaptor::OnUnitCastSpell(Int64 unitId, LuaGameEngine::Spell* spell)
{

}

void LuaEventAdaptor::OnUnitAttachBuff(Int64 unitId, LuaGameEngine::Buff* buff)
{

}

void LuaEventAdaptor::OnUnitDettachBuff(Int64 unitId, LuaGameEngine::Buff* buff)
{

}

static SharedPtr<Window> window;
static SharedPtr<Texture2D> texture;

void LuaEventAdaptor::OnDebugUnitTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	const float pixelStep = 1.0f / 100.f;
	Vector2 pixelPos(position.x_ * pixelStep, position.y_ * pixelStep);

	SharedPtr<Batch2D> batch = MakeShared<Batch2D>();
	batch->AddTriangle(
		pixelPos + Vector2(-pixelStep, -pixelStep), pixelPos + Vector2(pixelStep, pixelStep), pixelPos + Vector2(pixelStep, -pixelStep),
		pixelPos + Vector2(0, 0), pixelPos + Vector2(pixelStep, 0), pixelPos + Vector2(pixelStep, pixelStep),
		Color::GREEN.ToUInt()
	);
	batch->AddTriangle(
		pixelPos + Vector2(-pixelStep, -pixelStep), pixelPos + Vector2(-pixelStep, pixelStep), pixelPos + Vector2(pixelStep, pixelStep),
		pixelPos + Vector2(0, 0), pixelPos + Vector2(pixelStep, pixelStep), pixelPos + Vector2(0, pixelStep),
		Color::GREEN.ToUInt()
	);

	batch->SetTexture(texture);

	batches_.Push(batch);
}

void LuaEventAdaptor::EndFrameUpdate(float timeStep)
{
	if (!window)
	{
		WindowDevice::Initialize();
		GetSubsystem<RenderEngine>()->Initialize();
		window = MakeShared<Window>(nullptr, IntRect(0, 0, 600, 600));
		window->Show();

		texture = MakeShared<Texture2D>();
		texture->SetSize(1, 1, RenderEngine::GetRGBAFormat());
		const unsigned color = Color::WHITE.ToUInt();
		texture->SetData(0, 0, 0, 1, 1, &color);
	}

	WindowDevice::Update();

	// OnDebugUnitTransform(Vector3::ZERO, Quaternion::IDENTITY, Vector3::ONE);

	GetSubsystem<RenderEngine>()->PostRenderBatch(batches_);
	batches_.Clear();

	window->Render();
}
