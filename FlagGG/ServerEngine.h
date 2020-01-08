#pragma once

#include "Define.h"
#include "Core/Context.h"
#include "Container/Ptr.h"
#include "Resource/ResourceCache.h"
#include "Utility/SystemHelper.h"
#include "Launcher.h"

using namespace FlagGG::Core;
using namespace FlagGG::Container;
using namespace FlagGG::Resource;
using namespace FlagGG::Utility;

namespace FlagGG
{
	class FlagGG_API ServerEngine : public Launcher
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

		SharedPtr<ResourceCache> cache_;

		SystemHelper::Timer timer_;
		float elapsedTime_{ 0.0f };

		bool isRunning_{ false };

		float frameRate_{ 99999.0f };
	};
}
