#include "Launcher.h"

namespace FlagGG
{
	void Launcher::Run()
	{
		Start();

		while (IsRunning())
		{
			RunFrame();
		}

		Stop();
	}
}
