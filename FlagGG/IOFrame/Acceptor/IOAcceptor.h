#ifndef __IO_ACCEPTOR__
#define __IO_ACCEPTOR__

#include <memory>
#include <stdint.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			class IOAcceptor
			{
			public:
				virtual ~IOAcceptor() {};

				virtual bool bind(const char* ip, uint16_t port) = 0;

				virtual void start() = 0;

				virtual void stop() = 0;

				virtual void waitForStop() = 0;
			};

			typedef std::shared_ptr < IOAcceptor > IOAcceptorPtr;
		}
	}
}

#endif