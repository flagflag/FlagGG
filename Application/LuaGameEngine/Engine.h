#pragma once

#include "LGEExport.h"
#include "LuaEvent.h"
#include "EngineObject.h"
#include "Controller.h"

namespace LuaGameEngine
{

struct LuaUserInfo
{
	Int64 userId_;
	String userName_;
};

class LuaGameEngine_API Engine
{
public:
	virtual ~Engine() = default;

	template < class ClassName >
	ClassName* CreateObject()
	{
		return static_cast<ClassName*>(CreateObjectImpl(ClassName::GetTypeNameStatic().CString()));
	}

	virtual void DestroyObject(EngineObject* object) = 0;

	virtual void RegisterEventHandler(LuaEventHandler* handler) = 0;

	virtual void UnregisterEventHandler(LuaEventHandler* handler) = 0;

	virtual void AddUser(const LuaUserInfo& info) = 0;

	virtual void RemoveUser(Int64 userId) = 0;

	virtual EngineObject* GetPlayer(Int64 userId) = 0;

	virtual void OnStart() = 0;

	virtual void OnStop() = 0;

	virtual void OnFrameUpdate(float timeStep) = 0;

	virtual Controller* GetControler(Int64 userId) = 0;

protected:
	virtual EngineObject* CreateObjectImpl(const char* className) = 0;
};

}
