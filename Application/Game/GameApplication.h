#pragma once


#include <GameEngine.h>

#include <Lua/LuaVM.h>
#ifdef _WIN32
#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
#include <Scene/Scene.h>

#include "CameraOperation.h"
#include "Unit/Unit.h"
#include "Unit/Terrain.h"
#endif
#include "Network/NetworkAdaptor.h"
#include "LuaInterface/LuaLog.h"
#include "LuaInterface/LuaCommon.h"
#include "LuaInterface/LuaGame.h"
#include "LuaInterface/LuaUI.h"
#include "LuaInterface/LuaNetwork.h"
#include "Lobby/Lobby.h"
#include "GamePlay/GamePlayBase.h"
#include "GamePlay/Perspective.h"

using namespace FlagGG::Config;
using namespace FlagGG::Lua;
#ifdef _WIN32
using namespace FlagGG::Scene;
#endif

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
	void OpenLuaVM();
	void CreateNetwork();

#ifdef _WIN32
	void OnKeyDown(KeyState* keyState, unsigned keyCode);
	void OnKeyUp(KeyState* keyState, unsigned keyCode);
	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);
	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);
	void OnMouseMove(KeyState* keyState, const Vector2& delta);
	void WindowClose(void* window);
#endif

private:
	LJSONValue commandParam_;

#ifdef _WIN32
	SharedPtr<Window> window_;
	SharedPtr<Camera> camera_;
	SharedPtr<FlagGG::Scene::Scene> scene_;

	SharedPtr<Texture2D> renderTexture_[2];
	SharedPtr<Texture2D> shadowMap_;

	SharedPtr<Unit> mainHero_;
	SharedPtr<Unit> dissolveHero_;
	SharedPtr<Terrain> terrain_;
	SharedPtr<Unit> water_;
	SharedPtr<Unit> skybox_;
#endif

	SharedPtr<LuaVM> luaVM_;
	SharedPtr<LuaLog> logModule_;
	SharedPtr<LuaNetwork> networkModule_;

	SharedPtr<Network> tcpNetwork_;
	SharedPtr<Network> udpNetwork_;
	SharedPtr<Network> webNetwork_;

	SharedPtr<Lobby> lobby_;
	SharedPtr<GamePlayBase> gameplay_;
	SharedPtr<Perspective> perspective_;
};
