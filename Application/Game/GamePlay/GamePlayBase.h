#pragma once

#include <Container/Ptr.h>
#include <Lua/LuaVM.h>
#include <Scene/Scene.h>

using namespace FlagGG;

enum SyncMode
{
	SyncMode_Local = 0, // 本地同步，单机游戏
	SyncMode_State = 1, // 状态同步，联网游戏
};

class GamePlayBase : public Object
{
	OBJECT_OVERRIDE(GamePlayBase, Object);
public:
	virtual void Initialize(Scene* scene) = 0;

	virtual void Login(const LuaFunction& callback) = 0;

	virtual void StartGame() = 0;

	virtual void EndGame() = 0;

	virtual void FrameUpdate(float timeStep) = 0;

private:

};
