#include "ServerEngine.h"

namespace FlagGG
{

void ServerEngine::SetFrameRate(Real rate)
{
	frameRate_ = rate;
}

void ServerEngine::CreateCoreObject()
{
	context_ = new Context();
	cache_ = new ResourceCache(context_);
	cache_->AddResourceDir("../../../Res");
	context_->RegisterVariable<ResourceCache>(cache_.Get(), "ResourceCache");
}

void ServerEngine::Start()
{
	CreateCoreObject();

	isRunning_ = true;
	elapsedTime_ = 0.0f;
	timer_.Reset();
}

bool ServerEngine::IsRunning()
{
	return isRunning_;
}

void ServerEngine::RunFrame()
{
	UInt32 deltaTime = timer_.GetMilliSeconds(true);
	Real timeStep = (Real)deltaTime / 1000.0f;
	elapsedTime_ += timeStep;

	context_->SendEvent<Frame::FRAME_BEGIN_HANDLER>(Frame::FRAME_BEGIN, timeStep);
	context_->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(Frame::LOGIC_UPDATE, timeStep);
	context_->SendEvent<Frame::FRAME_END_HANDLER>(Frame::FRAME_END, timeStep);

	Real sleepTime = 1000.0f / frameRate_ - timer_.GetMilliSeconds(false);
	if (sleepTime > 0.0f)
	{
		Sleep(sleepTime);
	}

	context_->SendEvent<Frame::FRAME_END_HANDLER>(Frame::FRAME_END, timeStep);
}

void ServerEngine::Stop()
{
	isRunning_ = false;
}

}
