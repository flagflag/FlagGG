#pragma once

#include "AsyncFrame/Locker.h"
#include "AsyncFrame/ConditionVariable.h"
#include "Container/List.h"

namespace FlagGG
{
	namespace AsyncFrame
	{
		template < class ObjectType >
		class ConditionQueue
		{
		public:
			using Object = ObjectType;

			using Objects = Container::List<ObjectType>;

			void Wait()
			{
				Locker locker(mutex_);

				cnd_.Wait(&mutex_);
			}

			void Wait(uint32_t waitTime)
			{
				Locker locker(mutex_);

				cnd_.Wait(&mutex_, waitTime);
			}

			void PushBack(const Object& obj)
			{
				Locker locker(mutex_);

				queue_[queueIndex_].Insert(queue_[queueIndex_].End(), obj);

				cnd_.Trigger();
			}

			void PushFront(const Object& obj)
			{
				Locker locker(mutex_);

				queue_[queueIndex_].Insert(queue_[queueIndex_].Begin(), obj);

				cnd_.Trigger();
			}

			Objects& Swap()
			{
				Locker locker(mutex_);

				queueIndex_ ^= 1;
				queue_[queueIndex_].Clear();

				return queue_[queueIndex_ ^ 1];
			}

			uint32_t Size()
			{
				Locker locker(mutex_);

				return queue_[queueIndex_].Size();
			}

		private:
			Mutex mutex_;
			ConditionVariable cnd_;

			Objects queue_[2];
			uint32_t queueIndex_{ 0 };
		};
	}
}
