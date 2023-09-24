#include "ServerEngine.h"
#include "Core/EventManager.h"

namespace FlagGG
{

void ServerEngine::SetFrameRate(Real rate)
{
	frameRate_ = rate;
}

void ServerEngine::CreateCoreObject()
{
	GetSubsystem<ResourceCache>()->AddResourceDir("../../../Res");
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

	GetSubsystem<EventManager>()->SendEvent<Frame::BEGIN_FRAME_HANDLER>(timeStep);
	GetSubsystem<EventManager>()->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(timeStep);
	GetSubsystem<EventManager>()->SendEvent<Frame::END_FRAME_HANDLER>(timeStep);

	Real sleepTime = 1000.0f / frameRate_ - timer_.GetMilliSeconds(false);
	if (sleepTime > 0.0f)
	{
		Sleep(sleepTime);
	}
}

void ServerEngine::Stop()
{
	isRunning_ = false;
}

}
