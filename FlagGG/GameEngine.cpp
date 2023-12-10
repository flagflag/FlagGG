#include "GameEngine.h"
#include "Core/EventManager.h"
#ifdef _WIN32
#include "Graphics/RenderEngine.h"
#include "Graphics/Window.h"
#endif

namespace FlagGG
{

void GameEngine::SetFrameRate(Real rate)
{
	frameRate_ = rate;
}

void GameEngine::CreateCoreObject()
{
	GetSubsystem<ResourceCache>()->AddResourceDir(GetProgramDir() + "Res");
}

void GameEngine::Start()
{
	GetSubsystem<Context>()->MarkCurrentThreadAsGameThread();

	CreateCoreObject();

#ifdef _WIN32
	WindowDevice::Initialize();
	RenderEngine::Instance().Initialize();
#endif

	isRunning_ = true;
	elapsedTime_ = 0.0f;
	timer_.Reset();
}

bool GameEngine::IsRunning()
{
	return isRunning_;
}

void GameEngine::RunFrame()
{
	UInt32 deltaTime = timer_.GetMilliSeconds(true);
	Real timeStep = (Real)deltaTime / 1000.0f;
	elapsedTime_ += timeStep;

	GetSubsystem<EventManager>()->SendEvent<Frame::BEGIN_FRAME_HANDLER>(timeStep);

	WindowDevice::Update();

	GetSubsystem<EventManager>()->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(timeStep);

	GetSubsystem<EventManager>()->SendEvent<Frame::PRERENDER_UPDATE_HANDLER>(timeStep);

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance().RenderUpdate(viewport);
	}

	WindowDevice::RenderUpdate();

	RenderEngine::Instance().GetShaderParameters().SetValue(SP_DELTA_TIME, timeStep);
	RenderEngine::Instance().GetShaderParameters().SetValue(SP_ELAPSED_TIME, elapsedTime_);

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance().Render(viewport);
	}

	WindowDevice::Render();

	GetSubsystem<EventManager>()->SendEvent<Frame::END_FRAME_HANDLER>(timeStep);

	Real sleepTime = 1000.0f / frameRate_ - timer_.GetMilliSeconds(false);
	if (sleepTime > 0.0f)
	{
		Sleep(sleepTime);
	}
}

void GameEngine::Stop()
{
	isRunning_ = false;

#ifdef _WIN32
	WindowDevice::Uninitialize();
	RenderEngine::Instance().Uninitialize();
#endif
}

}
