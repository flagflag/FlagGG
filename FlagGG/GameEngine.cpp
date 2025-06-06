#include "GameEngine.h"
#include "Core/EventManager.h"
#include "GfxDevice/GfxDevice.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Window.h"
#include "FileSystem/FileManager.h"
#include "FileSystem/FileSystemArchive/DefaultFileSystemArchive.h"
#include "UI/UISystem.h"
#include "Core/Profiler.h"

namespace FlagGG
{

void GameEngine::SetFrameRate(Real rate)
{
	frameRate_ = rate;
}

void GameEngine::CreateCoreObject()
{
	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Res"));
}

void GameEngine::Start()
{
	GetSubsystem<Context>()->MarkCurrentThreadAsGameThread();

	CreateCoreObject();

#if PLATFORM_WINDOWS
	WindowDevice::Initialize();
	RenderEngine::Instance().Initialize();
#endif

	GetSubsystem<UISystem>()->Initialize();

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

	GetSubsystem<Profiler>()->BeginFrame();

	GetSubsystem<EventManager>()->SendEvent<Frame::BEGIN_FRAME_HANDLER>(timeStep);

	WindowDevice::Update();

	GetSubsystem<EventManager>()->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(timeStep);

	GetSubsystem<EventManager>()->SendEvent<Frame::PRERENDER_UPDATE_HANDLER>(timeStep);

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance().RenderUpdate(viewport);
	}

	WindowDevice::RenderUpdate();

	{
		RenderEngine::Instance().GetShaderParameters().SetValue<float>(SP_DELTA_TIME, timeStep);
		RenderEngine::Instance().GetShaderParameters().SetValue<float>(SP_ELAPSED_TIME, elapsedTime_);
		UInt32 frameNumber = GetSubsystem<Context>()->GetFrameNumber();
		RenderEngine::Instance().GetShaderParameters().SetValue<UInt32>(SP_FRAME_NUMBER, frameNumber);
		RenderEngine::Instance().GetShaderParameters().SetValue<UInt32>(SP_FRAME_NUMBER_MOD8, frameNumber % 8);
	}

	GfxDevice::GetDevice()->BeginFrame();

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance().Render(viewport);
	}

	GetSubsystem<UISystem>()->Render();

	GetSubsystem<EventManager>()->SendEvent<Frame::RENDER_UPDATE_HANDLER>(timeStep);

	WindowDevice::Render();

	GfxDevice::GetDevice()->EndFrame();

	GetSubsystem<EventManager>()->SendEvent<Frame::END_FRAME_HANDLER>(timeStep);

	GetSubsystem<Context>()->AddFrameNumber();

	Real sleepTime = 1000.0f / frameRate_ - timer_.GetMilliSeconds(false);
	if (sleepTime > 0.0f)
	{
		PROFILE_AUTO(FrameSleep);
		Sleep(sleepTime);
	}

	GetSubsystem<Profiler>()->BeginInterval();
	GetSubsystem<Profiler>()->EndFrame();
}

void GameEngine::Stop()
{
	isRunning_ = false;

	GetSubsystem<EventManager>()->SendEvent<Application::ENGINE_EXIT_HANDLER>();

#if PLATFORM_WINDOWS
	WindowDevice::Uninitialize();
	RenderEngine::Instance().Uninitialize();
#endif
}

}
