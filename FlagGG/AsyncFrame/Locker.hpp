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
			Locker(std::mutex& mutex)
				: mutex_(mutex)
			{
				mutex_.lock();
			}

			virtual ~Locker()
			{
				mutex_.unlock();
			}

		private:
			std::mutex& mutex_;
		};

		class FlagGG_API ReadLocker
		{

		};

		class FlagGG_API WriteLocker
		{

		};

		class FlagGG_API RecursiveLocker
		{
		public:
			RecursiveLocker(std::recursive_mutex& mutex)
				: mutex_(mutex)
			{
				mutex_.lock();
			}

			virtual ~RecursiveLocker()
			{
				mutex_.unlock();
			}

		private:
			std::recursive_mutex& mutex_;
		};
	}
}

#endif