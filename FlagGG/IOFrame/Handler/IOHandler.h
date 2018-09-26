#ifndef __IO_HANDLER__
#define __IO_HANDLER__

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Handler
		{
			class IOHandler
			{
			public:
				virtual ~IOHandler() {}

				virtual void onRegisterd() = 0;

				virtual void onOpend() = 0;

				virtual void onClosed() = 0;
			};
		}
	}
}

#endif