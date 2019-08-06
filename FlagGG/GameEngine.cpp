#include "GameEngine.h"

#include "Graphics/RenderEngine.h"
#include <Graphics/Camera.h>
#include "Graphics/Window.h"

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
		WindowDevice::Initialize();
		RenderEngine::Instance()->Initialize();

		CreateCoreObject();

		isRunning_ = true;
	}

	bool GameEngine::IsRunning()
	{
		return isRunning_;
	}

	void GameEngine::RunFrame()
	{
		uint32_t deltaTime = timer_.GetMilliSeconds(true);
		float timeStep = (float)deltaTime / 1000.0f;

		context_->SendEvent<Frame::FRAME_BEGIN_HANDLER>(Frame::FRAME_BEGIN, timeStep);

		WindowDevice::Update();

		context_->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(Frame::LOGIC_UPDATE, timeStep);

		WindowDevice::Render();

		for (const auto& viewport : viewports_)
		{
			RenderEngine::Instance()->Render(viewport);
		}

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

		WindowDevice::Uninitialize();
		RenderEngine::Instance()->Uninitialize();
	}
}
