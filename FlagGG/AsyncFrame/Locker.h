#ifndef __LOCKER__
#define __LOCKER__

#include "Export.h"
#include "AsyncFrame/Mutex.h"

#include <mutex>

namespace FlagGG
{
	namespace AsyncFrame
	{
		class FlagGG_API Locker
		{
		public:
			Locker(Mutex& mutex);

			virtual ~Locker();

		private:
			Mutex& mutex_;
		};

		class FlagGG_API RecursiveLocker
		{
		public:
			RecursiveLocker(std::recursive_mutex& mutex);

			virtual ~RecursiveLocker();

		private:
			std::recursive_mutex& mutex_;
		};
	}
}

#endif