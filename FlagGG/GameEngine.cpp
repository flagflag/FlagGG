#include "GameEngine.h"
#include "Core/EventManager.h"
#include "GfxDevice/GfxDevice.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Window.h"
#include "FileSystem/FileManager.h"
#include "FileSystem/FileSystemArchive/DefaultFileSystemArchive.h"
#include "UI/UISystem.h"

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

	GfxDevice::GetDevice()->BeginFrame();

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance().Render(viewport);
	}

	WindowDevice::Render();

	GfxDevice::GetDevice()->EndFrame();

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

#if PLATFORM_WINDOWS
	WindowDevice::Uninitialize();
	RenderEngine::Instance().Uninitialize();
#endif
}

}
