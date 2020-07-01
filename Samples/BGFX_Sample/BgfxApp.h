#pragma once

#include <GameEngine.h>
#include <Config/LJSONValue.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Window.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>

using namespace FlagGG::Container;
using namespace FlagGG::Scene;
using namespace FlagGG::Core;
using namespace FlagGG::Math;
using namespace FlagGG::Graphics;

class BgfxApp : public FlagGG::GameEngine
{
public:
	BgfxApp(const FlagGG::Config::LJSONValue& command);

	void Start() override;
	void Stop() override;
	void Update(float timeStep);
	void OnMouseMove(KeyState* keyState, const Vector2& delta);

private:
	SharedPtr<Window> window_;
	SharedPtr<Camera> camera_;
	SharedPtr<FlagGG::Scene::Scene> scene_;
};
