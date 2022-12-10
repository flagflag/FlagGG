#pragma once

#include "Container/Allocator.h"
#include "Container/HashSet.h"
#include "Container/Vector.h"

namespace FlagGG
{

template < class Object >
class ObjectPool
{
public:
	Object* CreateObject()
	{
		Object* object = allocator_.Reserve();
		objects_.Insert(object);
		return object;
	}

	void DestroyObject(Object* object)
	{
		objects_.Erase(object);
		allocator_.Free(object);
	}

	void DelayDestroyObject(Object* object)
	{
		objects_.Erase(object);
		delayDestroyObjects_.Push(object);
	}

	void Recycling(std::function<bool(Object*)>&& condition)
	{
		for (auto it = delayDestroyObjects_.Begin(); it != delayDestroyObjects_.End(); ++it)
		{
			if (condition(*it))
				it = delayDestroyObjects_.Erase(it);
			else
				++it;
		}
	}

	const FlagGG::HashSet<Object*>& GetObjects()
	{
		return objects_;
	}

private:
	Allocator<Object> allocator_;

	HashSet<Object*> objects_;

	PODVector<Object*> delayDestroyObjects_;
};

}
