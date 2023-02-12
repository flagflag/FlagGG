#include "GameEngine.h"
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
	context_ = new Context();
	input_ = new Input(context_);
	cache_ = new ResourceCache(context_);
	cache_->AddResourceDir(GetProgramDir() + "Res");
	context_->RegisterVariable<Input>(input_.Get(), "input");
	context_->RegisterVariable<ResourceCache>(cache_.Get(), "ResourceCache");
}

void GameEngine::Start()
{
	CreateCoreObject();

#ifdef _WIN32
	WindowDevice::Initialize();
	RenderEngine::CreateInstance(context_)->Initialize();
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

	context_->SendEvent<Frame::BEGIN_FRAME_HANDLER>(timeStep);

	WindowDevice::Update();

	context_->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(timeStep);

	context_->SendEvent<Frame::PRERENDER_UPDATE_HANDLER>(timeStep);

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance()->RenderUpdate(viewport);
	}

	WindowDevice::RenderUpdate();

	RenderEngine::Instance()->GetShaderParameters().SetValue(SP_DELTA_TIME, timeStep);
	RenderEngine::Instance()->GetShaderParameters().SetValue(SP_ELAPSED_TIME, elapsedTime_);

	for (const auto& viewport : viewports_)
	{
		RenderEngine::Instance()->Render(viewport);
	}

	WindowDevice::Render();

	context_->SendEvent<Frame::END_FRAME_HANDLER>(timeStep);

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
	RenderEngine::Instance()->Uninitialize();
	RenderEngine::DestroyInstance();
#endif
}

}
