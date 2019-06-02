#pragma once

#include <Lua/LuaVM.h>
#include <Container/Ptr.h>

#include "GameEngine.h"
#include "Network/NetworkAdaptor.h"
#include "Unit/Unit.h"
#include "LuaInterface/LuaLog.h"
#include "LuaInterface/LuaCommon.h"
#include "LuaInterface/LuaGame.h"
#include "LuaInterface/LuaUI.h"
#include "LuaInterface/LuaNetwork.h"

using namespace FlagGG::Container;
using namespace FlagGG::Config;
using namespace FlagGG::Lua;

class GameApplication : public GameEngine
{
public:
	GameApplication(LJSONValue commandParam);

protected:
	void Start() override;
	void Stop() override;
	void Update(float timeStep);

	void OnKeyDown(KeyState* keyState, unsigned keyCode);
	void OnKeyUp(KeyState* keyState, unsigned keyCode);
	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);
	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);
	void OnMouseMove(KeyState* keyState, const Vector2& delta);

private:
	LJSONValue commandParam_;

	SharedPtr<LuaVM> luaVM_;
	SharedPtr<LuaLog> logModule_;
	SharedPtr<LuaNetwork> networkModule_;

	SharedPtr<Network> network_;
};
