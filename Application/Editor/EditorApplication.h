#pragma once

#include "CameraOperation.h"

#include <GameEngine.h>
#include <Container/Ptr.h>
#include <Scene/Node.h>
#include <Scene/Scene.h>
#include <Graphics/Window.h>

using namespace FlagGG;
using namespace FlagGG::Scene;
using namespace FlagGG::Container;
using namespace FlagGG::Graphics;

class EditorApplication : public GameEngine
{
public:
	void Start() override;
	void Stop() override;
	void Update(float timeStep);

protected:
	void CreateScene();

	void SetupWindow();

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	void WindowClose(void* window);

private:
	SharedPtr<FlagGG::Scene::Scene> scene_;

	SharedPtr<Node> importNode_;

	SharedPtr<CameraOperation> cameraOpt_;

	SharedPtr<Window> window_;
};
