#include "Network/LuaEventAdaptor.h"
#include "Proto/Game.pb.h"

LuaEventAdaptor::LuaEventAdaptor(NetworkSender* sender) :
	sender_(sender)
{
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

static Proto::Game::UnitMovementInfo* ToProtoMovementInfo(LuaGameEngine::Movement* movement)
{
	Proto::Game::UnitMovementInfo* protoMovement = new Proto::Game::UnitMovementInfo();
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
	return protoMovement;
}

static Proto::Game::SpellInfo* ToProtoSpellInfo(LuaGameEngine::Spell* spell)
{
	Proto::Game::SpellInfo* protoSpell = new Proto::Game::SpellInfo();
	// TO DO
	// ......
	return protoSpell;
}

static Proto::Game::BuffInfo* ToProtoBuffInfo(LuaGameEngine::Buff* buff)
{
	Proto::Game::BuffInfo* protoBuff = new Proto::Game::BuffInfo();
	// TO DO
	// ......
	return protoBuff;
}

void LuaEventAdaptor::OnAppearUnit(Int64 unitId, LuaGameEngine::Unit* unit)
{
	Proto::Game::NotifyUnitAppear notify;

	Proto::Game::Transform* trans = ToProtoTransform(unit);
	Proto::Game::UnitAttribute* attr = ToProtoAttribute(unit->GetAttribute());
	Proto::Game::UnitMovementInfo* moveInfo = ToProtoMovementInfo(unit->GetCurrentMovement());
	Proto::Game::SpellInfo* spellInfo = ToProtoSpellInfo(unit->GetCurrentSpell());
	Proto::Game::BuffInfoList* buffInfoList = new Proto::Game::BuffInfoList();
	for (UInt32 i = 0; i < unit->GetNumBuff(); ++i)
	{
		Proto::Game::BuffInfo* buffInfo = buffInfoList->add_buff_infos();
		Proto::Game::BuffInfo* temp = ToProtoBuffInfo(unit->GetBuff(i));
		(*buffInfo) = (*temp);
		delete temp;
	}

	notify.set_unit_id(unitId);
	notify.set_allocated_transform(trans);
	notify.set_status(unit->GetStatus());
	notify.set_allocated_attribute(attr);
	notify.set_allocated_movement(moveInfo);
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
