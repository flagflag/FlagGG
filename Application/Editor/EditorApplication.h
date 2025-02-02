#pragma once

#include "CameraOperation.h"

#include <GameEngine.h>
#include <Container/Ptr.h>
#include <Scene/Node.h>
#include <Scene/Scene.h>
#include <Graphics/Window.h>

#include "Application/CoreCLRHelper/CoreCLRHelper.h"

using namespace FlagGG;

class EditorApplication : public GameEngine
{
public:
	EditorApplication();

	void Start() override;
	void Stop() override;
	void Update(float timeStep);

protected:
	void InitCSharpEnv();

	void CreateScene();

	void SetupWindow();

	void OnKeyUp(KeyState* keyState, unsigned keyCode);

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos);

	void OnTick(Real timeStep);

	void OnRender(Real timeStep);

	void WindowClose(void* window);

private:
	SharedPtr<Scene> scene_;

	SharedPtr<Node> importNode_;

	SharedPtr<CameraOperation> cameraOpt_;

	SharedPtr<Window> window_;

	DotNetLibRuntime* libRuntime_;
	DotNetFunction<void()> uiElementsInit;
	DotNetFunction<void()> uiElementsTick;
	DotNetFunction<void()> uiElementsRender;
};
