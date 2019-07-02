#ifndef __LOCKER__
#define __LOCKER__

#include "Export.h"

#include <mutex>

namespace FlagGG
{
	namespace AsyncFrame
	{
		class FlagGG_API Locker
		{
		public:
			Locker(std::mutex& mutex);

			virtual ~Locker();

		private:
			std::mutex& mutex_;
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