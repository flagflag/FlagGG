#pragma once

#include "AsyncFrame/Locker.h"
#include "AsyncFrame/ConditionVariable.h"
#include "Container/List.h"

namespace FlagGG
{

template < class ObjectType >
class ConditionQueue
{
public:
	using Object = ObjectType;

	using Objects = List<ObjectType>;

	void PushBack(const Object& obj)
	{
		Locker locker(mutex_);

		queueImpl_.Push(obj);

		cnd_.Trigger();
	}

	void PushFront(const Object& obj)
	{
		Locker locker(mutex_);

		queueImpl_.PushFront(obj);

		cnd_.Trigger();
	}

	bool Swap(Objects& objects)
	{
		Locker locker(mutex_);
		if (queueImpl_.Empty())
			cnd_.Wait(&mutex_);
		// 这里再加一个if判断是因为：如果多个线程同时调用Swap，条件变量释放以后第一个进入下面语句的线程会导致队列数据被取出，轮到其他线程时队列里面的数据可能已经没了
		if (queueImpl_.Empty())
			return false;
		queueImpl_.Swap(objects);
		queueImpl_.Clear();
		return true;
	}

	bool PopBack(Object& object)
	{
		Locker locker(mutex_);
		if (queueImpl_.Empty())
			cnd_.Wait(&mutex_);
		// 这里再加一个if判断是因为：如果多个线程同时调用Swap，条件变量释放以后第一个进入下面语句的线程会导致队列数据被取出，轮到其他线程时队列里面的数据可能已经没了
		if (queueImpl_.Empty())
			return false;
		object = queueImpl_.Back();
		queueImpl_.Pop();
		return true;
	}

	bool PopFront(Object& object)
	{
		Locker locker(mutex_);
		if (queueImpl_.Empty())
			cnd_.Wait(&mutex_);
		// 这里再加一个if判断是因为：如果多个线程同时调用Swap，条件变量释放以后第一个进入下面语句的线程会导致队列数据被取出，轮到其他线程时队列里面的数据可能已经没了
		if (queueImpl_.Empty())
			return false;
		object = queueImpl_.Front();
		queueImpl_.PopFront();
		return true;
	}

	UInt32 Size()
	{
		Locker locker(mutex_);

		return queueImpl_.Size();
	}

	bool IsEmpty()
	{
		return Size() == 0u;
	}

	void Release()
	{
		Locker locker(mutex_);
		cnd_.Trigger();
	}

private:
	Mutex mutex_;
	ConditionVariable cnd_;

	Objects queueImpl_;
	bool forceRelease_{ false };
};

}
