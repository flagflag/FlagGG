#ifndef __LOCKER__
#define __LOCKER__

#include <mutex>

namespace FlagGG
{
	namespace AsyncFrame
	{
		class Locker
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

		class ReadLocker
		{

		};

		class WriteLocker
		{

		};

		class RecursiveLocker
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