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

using namespace FlagGG::Core;
using namespace FlagGG::Container;
#ifdef _WIN32
using namespace FlagGG::Graphics;
#endif
using namespace FlagGG::Resource;
using namespace FlagGG::Utility;

namespace FlagGG
{
	class FlagGG_API GameEngine : public Launcher
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

#ifdef _WIN32
		Vector<SharedPtr<Viewport>> viewports_;
#endif

		SystemHelper::Timer timer_;
		float elapsedTime_{ 0.0f };

		bool isRunning_{ false };

		float frameRate_{ 99999.0f }; // �����óɲ���֡����
	};
}
