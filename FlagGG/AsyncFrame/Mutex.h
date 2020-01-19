#pragma once

#include "Export.h"
#include "AsyncFrame/KernelObject/HandleObject.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		class FlagGG_API NullMutex final
		{
		public:
			inline void Lock() {}

			inline bool TryLock() { return false; }

			inline void UnLock() {}
		};

		class FlagGG_API Mutex : public KernelObject::HandleObject
		{
		public:
			Mutex();

			~Mutex() override;

			void Lock();

			bool TryLock();

			void UnLock();
		};
	}
}
