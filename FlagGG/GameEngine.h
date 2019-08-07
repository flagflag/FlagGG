#pragma once

#include "Core/EventDefine.h"
#include "Core/Context.h"
#include "Core/DeviceEvent.h"
#include "Graphics/Viewport.h"
#include "Resource/ResourceCache.h"
#include "Container/Ptr.h"
#include "Utility/SystemHelper.h"
#include "Launcher.h"

using namespace FlagGG::Core;
using namespace FlagGG::Container;
using namespace FlagGG::Graphics;
using namespace FlagGG::Resource;
using namespace FlagGG::Utility;

namespace FlagGG
{
	class GameEngine : public Launcher
	{
	public:
		void SetFrameRate(float rate);

	protected:
		void Start() override;

		void Stop() override;

		bool IsRunning() override;

		void RunFrame() override;

		void CreateCoreObject();

		SharedPtr<Context> context_;

		SharedPtr<Input> input_;

		SharedPtr<ResourceCache> cache_;

		Vector<SharedPtr<Viewport>> viewports_;

		SystemHelper::Timer timer_;
		float elapsedTime_{ 0.0f };

		bool isRunning_{ false };

		float frameRate_{ 99999.0f }; // 先设置成不限帧数把
	};
}
