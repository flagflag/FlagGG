#pragma once


#include <GameEngine.h>

#include <Lua/LuaVM.h>
#ifdef _WIN32
#include <Graphics/Window.h>
#include <Graphics/Texture2D.h>
#include <Scene/Scene.h>
#include <Core/Forwarder.h>
#include <AsyncFrame/Mutex.h>

#include "CameraOperation.h"
#include "Unit/Unit.h"
#include "Unit/Terrain.h"
#include "Unit/ParticleActor.h"
#endif
#include "Network/NetworkAdaptor.h"
#include "Network//GameProtoDistributor.h"
#include "LuaInterface/LuaLog.h"
#include "LuaInterface/LuaCommon.h"
#include "LuaInterface/LuaGame.h"
#include "LuaInterface/LuaUI.h"
#include "LuaInterface/LuaNetwork.h"
#include "LuaInterface/LuaGame.h"
#include "Lobby/Lobby.h"
#include "GamePlay/GamePlayBase.h"
#include "GamePlay/Perspective.h"
#include "GameBuilder/GameBuilder.h"

using namespace FlagGG;

typedef void(*SetupFinish)(void* windowHandle);

class GameApplication : public GameEngine
{
public:
	GameApplication(LJSONValue commandParam, SetupFinish setupFinish);

	void ShowPrefab(const String& prefabPath);

	Forwarder<Mutex>& GetForwarder() { return forwarder_; }

protected:
	void Start() override;
	void Stop() override;
	void Update(float timeStep);

	void Create2DBatch();
	void CreateScene();
	void SetupWindow();
	void OpenLuaVM();
	void CreateNetwork();

#ifdef _WIN32
	void OnKeyDown(KeyState* keyState, UInt32 keyCode);
	void OnKeyUp(KeyState* keyState, UInt32 keyCode);
	void OnMouseDown(KeyState* keyState, MouseKey mouseKey);
	void OnMouseUp(KeyState* keyState, MouseKey mouseKey);
	void OnMouseMove(KeyState* keyState, const Vector2& delta);
	void WindowClose(void* window);
#endif

private:
	LJSONValue commandParam_;

	SetupFinish setupFinish_;

	GameBuilder gameBuilder_;

#ifdef _WIN32
	SharedPtr<Window> window_;
	SharedPtr<Camera> camera_;
	SharedPtr<Camera> reflectionCamera_;
	SharedPtr<Scene> scene_;

	SharedPtr<Texture2D> renderTexture_[2];
	SharedPtr<Texture2D> shadowMap_;

	SharedPtr<Texture2D> rttTexture_[2];

	SharedPtr<Unit> mainHero_;
	SharedPtr<Unit> dissolveHero_;
	SharedPtr<Terrain> terrain_;
	SharedPtr<Unit> water_;
	SharedPtr<Unit> waterDown_;
	SharedPtr<Unit> skybox_;
	SharedPtr<ParticleActor> simpleParticle_;
	SharedPtr<Node> previewPrefab_;
#endif

	SharedPtr<LuaVM> luaVM_;
	SharedPtr<LuaLog> logModule_;
	SharedPtr<LuaNetwork> networkModule_;
	SharedPtr<LuaGamePlay> gameplayModule_;

	SharedPtr<Network> tcpNetwork_;
	SharedPtr<Network> udpNetwork_;
	SharedPtr<Network> webNetwork_;
	SharedPtr<GameProtoDistributor> gameProtoDstr_;

	SharedPtr<Lobby> lobby_;
	SharedPtr<GamePlayBase> gameplay_;
	SharedPtr<Perspective> perspective_;

	Forwarder<Mutex> forwarder_;
};
