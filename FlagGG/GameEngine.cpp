#include "GameEngine.h"
#ifdef _WIN32
#include "Graphics/RenderEngine.h"
#include "Graphics/Window.h"
#endif

using namespace FlagGG::Math;

namespace FlagGG
{
	void GameEngine::SetFrameRate(float rate)
	{
		frameRate_ = rate;
	}

	void GameEngine::CreateCoreObject()
	{
		context_ = new Context();
		input_ = new Input(context_);
		cache_ = new ResourceCache(context_);
		cache_->AddResourceDir("../../../Res");
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
		uint32_t deltaTime = timer_.GetMilliSeconds(true);
		float timeStep = (float)deltaTime / 1000.0f;
		elapsedTime_ += timeStep;

		context_->SendEvent<Frame::FRAME_BEGIN_HANDLER>(Frame::FRAME_BEGIN, timeStep);

#ifdef _WIN32
		WindowDevice::Update();
#endif

		context_->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(Frame::LOGIC_UPDATE, timeStep);

#ifdef _WIN32
		RenderEngine::Instance()->GetShaderParameters().SetValue(SP_DELTA_TIME, timeStep);
		RenderEngine::Instance()->GetShaderParameters().SetValue(SP_ELAPSED_TIME, elapsedTime_);

		WindowDevice::Render();

		for (const auto& viewport : viewports_)
		{
			RenderEngine::Instance()->Render(viewport);
		}
#endif

		context_->SendEvent<Frame::FRAME_END_HANDLER>(Frame::FRAME_END, timeStep);

		float sleepTime = 1000.0f / frameRate_ - timer_.GetMilliSeconds(false);
		if (sleepTime > 0.0f)
		{
			SystemHelper::Sleep(sleepTime);
		}

		context_->SendEvent<Frame::FRAME_END_HANDLER>(Frame::FRAME_END, timeStep);
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
