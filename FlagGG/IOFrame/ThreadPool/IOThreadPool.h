#ifndef __IO_THREAD_POOL__
#define __IO_THREAD_POOL__

#include "AsyncFrame/KernelObject/Runtime.h"
#include "Export.h"

#include <memory>

namespace FlagGG
{
	namespace IOFrame
	{
		class FlagGG_API IOThreadPool : public AsyncFrame::KernelObject::Runtime
		{
		public:
			virtual void start() = 0;
		};

		typedef std::shared_ptr < IOThreadPool > IOThreadPoolPtr;
	}
}

#endif