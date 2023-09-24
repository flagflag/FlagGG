#pragma once

#include "Define.h"
#include "Core/Context.h"
#include "Container/Ptr.h"
#include "Resource/ResourceCache.h"
#include "Utility/SystemHelper.h"
#include "Launcher.h"

using namespace FlagGG::Core;

namespace FlagGG
{

class FlagGG_API ServerEngine : public Launcher
{
public:
	void SetFrameRate(Real rate);

protected:
	void Start() override;

	void Stop() override;

	bool IsRunning() override;

	void RunFrame() override;

	void CreateCoreObject();

	Timer timer_;
	Real elapsedTime_{ 0.0f };

	bool isRunning_{ false };

	Real frameRate_{ 99999.0f };
};

}
