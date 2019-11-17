#pragma once

#include "Export.h"

namespace FlagGG
{
	class FlagGG_API Launcher
	{
	public:
		void Run();

	protected:
		virtual void Start() = 0;

		virtual void Stop() = 0;

		virtual bool IsRunning() = 0;

		virtual void RunFrame() = 0;
	};
}
