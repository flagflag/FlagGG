#ifndef __IO_HANDLER__
#define __IO_HANDLER__

#include "IOFrame/Handler/EventHandler.h"
#include "Export.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Handler
		{
			class FlagGG_API IOHandler
			{
			public:
				virtual ~IOHandler() = default;

				virtual void OnRegisterd(Handler::EventHandlerPtr handler) = 0;

				virtual void OnOpend() = 0;

				virtual void OnClosed() = 0;
			};
		}
	}
}

#endif