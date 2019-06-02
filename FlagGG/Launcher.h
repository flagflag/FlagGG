#pragma once

namespace FlagGG
{
	class Launcher
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
