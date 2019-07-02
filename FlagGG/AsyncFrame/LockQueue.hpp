#ifndef __LOCK_QUEUE__
#define __LOCK_QUEUE__

#include "Export.h"

#include "AsyncFrame/Locker.h"

#include <list>

namespace FlagGG
{
	namespace AsyncFrame
	{
		template < class ObjectType >
		class FlagGG_API LockQueue
		{
		public:
			typedef ObjectType Object;

			typedef std::list < Object > Objects;

			void Push(const Object& obj)
			{
				RecursiveLocker locker(mutex_);

				queue_.insert(queue_.end(), obj);
			}

			void Pop(Object& obj)
			{
				RecursiveLocker locker(mutex_);

				typename Objects::iterator it = queue_.begin();
				obj = (*it);
				queue_.erase(it);
			}

			void Slipce(Objects& objs)
			{
				RecursiveLocker locker(mutex_);

				objs.splice(objs.begin(), queue_, queue_.begin(), queue_.end());
			}

			size_t Size()
			{
				RecursiveLocker locker(mutex_);

				size_t _size = queue_.size();

				return _size;
			}

		private:
			std::recursive_mutex mutex_;

			Objects queue_;
		};
	}
}

#endif