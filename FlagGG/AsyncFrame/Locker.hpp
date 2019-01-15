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
				: m_mutex(mutex)
			{
				m_mutex.lock();
			}

			virtual ~Locker()
			{
				m_mutex.unlock();
			}

		private:
			std::mutex& m_mutex;
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
				: m_mutex(mutex)
			{
				m_mutex.lock();
			}

			virtual ~RecursiveLocker()
			{
				m_mutex.unlock();
			}

		private:
			std::recursive_mutex& m_mutex;
		};
	}
}

#endif