#ifndef __IO_ACCEPTOR__
#define __IO_ACCEPTOR__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"

#include <stdint.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			class FlagGG_API IOAcceptor : public Container::RefCounted
			{
			public:
				virtual ~IOAcceptor() = default;

				virtual bool Bind(const char* ip, uint16_t port) = 0;

				virtual void Start() = 0;

				virtual void Stop() = 0;

				virtual void WaitForStop() = 0;
			};

			typedef Container::SharedPtr < IOAcceptor > IOAcceptorPtr;
		}
	}
}

#endif