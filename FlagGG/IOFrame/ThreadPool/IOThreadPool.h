#ifndef __IO_THREAD_POOL__
#define __IO_THREAD_POOL__

#include "AsyncFrame/KernelObject/Runtime.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Export.h"

#include <memory>

namespace FlagGG
{
	namespace IOFrame
	{
		class FlagGG_API IOThreadPool : public AsyncFrame::KernelObject::Runtime, public Container::RefCounted
		{
		public:
			virtual void Start() = 0;
		};

		typedef Container::SharedPtr < IOThreadPool > IOThreadPoolPtr;
	}
}

#endif