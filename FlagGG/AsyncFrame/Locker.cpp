#include "AsyncFrame/Locker.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		Locker::Locker(std::mutex& mutex) :
			mutex_(mutex)
		{
			mutex_.lock();
		}

		Locker::~Locker()
		{
			mutex_.unlock();
		}


		RecursiveLocker::RecursiveLocker(std::recursive_mutex& mutex) :
			mutex_(mutex)
		{
			mutex_.lock();
		}

		RecursiveLocker::~RecursiveLocker()
		{
			mutex_.unlock();
		}
	}
}
