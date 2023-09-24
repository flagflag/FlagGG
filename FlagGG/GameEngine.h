#pragma once

#include "Core/EventDefine.h"
#include "Core/Context.h"
#include "Core/DeviceEvent.h"
#ifdef _WIN32
#include "Graphics/Viewport.h"
#include "Graphics/Batch.h"
#endif
#include "Resource/ResourceCache.h"
#include "Container/Ptr.h"
#include "Utility/SystemHelper.h"
#include "Launcher.h"

namespace FlagGG
{

class FlagGG_API GameEngine : public Launcher
{
public:
	void SetFrameRate(Real rate);

protected:
	void Start() override;

	void Stop() override;

	bool IsRunning() override;

	void RunFrame() override;

	void CreateCoreObject();

#ifdef _WIN32
	Vector<SharedPtr<Viewport>> viewports_;
#endif

	Timer timer_;
	Real elapsedTime_{ 0.0f };

	bool isRunning_{ false };

	Real frameRate_{ 99999.0f };
};

}
