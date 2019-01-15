#ifndef __IO_HANDLER__
#define __IO_HANDLER__

#include "IOFrame\Handler\EventHandler.h"
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

				virtual void onRegisterd(Handler::EventHandlerPtr handler) = 0;

				virtual void onOpend() = 0;

				virtual void onClosed() = 0;
			};
		}
	}
}

#endif