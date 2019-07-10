#include "AsyncFrame/Locker.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		Locker::Locker(Mutex& mutex) :
			mutex_(mutex)
		{
			mutex_.Lock();
		}

		Locker::~Locker()
		{
			mutex_.UnLock();
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
