#pragma once


#include <GameEngine.h>

#include <Lua/LuaVM.h>
#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
#include <Scene/Scene.h>

#include "CameraOperation.h"
#include "Unit/Unit.h"
#include "Unit/Terrain.h"
#include "Network/NetworkAdaptor.h"
#include "LuaInterface/LuaLog.h"
#include "LuaInterface/LuaCommon.h"
#include "LuaInterface/LuaGame.h"
#include "LuaInterface/LuaUI.h"
#include "LuaInterface/LuaNetwork.h"

using namespace FlagGG::Config;
using namespace FlagGG::Lua;
using namespace FlagGG::Scene;

class GameApplication : public GameEngine
{
public:
	GameApplication(LJSONValue commandParam);

protected:
	void Start() override;
	void Stop() override;
	void Update(float timeStep);

	void CreateScene();
	void SetupWindow();

	void OnKeyDown(KeyState* keyState, unsigned keyCode);
	void OnKeyUp(KeyState* keyState, unsigned keyCode);
	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);
	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);
	void OnMouseMove(KeyState* keyState, const Vector2& delta);
	void WindowClose(void* window);

private:
	LJSONValue commandParam_;

	SharedPtr<Window> window_;
	SharedPtr<CameraOperation> cameraOpt_;
	SharedPtr<FlagGG::Scene::Scene> scene_;

	SharedPtr<Texture2D> renderTexture_[2];

	SharedPtr<Unit> mainHero_;
	SharedPtr<Terrain> terrain_;

	SharedPtr<LuaVM> luaVM_;
	SharedPtr<LuaLog> logModule_;
	SharedPtr<LuaNetwork> networkModule_;

	SharedPtr<Network> network_;
};
