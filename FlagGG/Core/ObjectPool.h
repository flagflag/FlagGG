#pragma once

#include "Container/Allocator.h"
#include "Container/HashSet.h"
#include "Container/Vector.h"

namespace FlagGG
{
	namespace Core
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

			const FlagGG::Container::HashSet<Object*>& GetObjects()
			{
				return objects_;
			}

		private:
			FlagGG::Container::Allocator<Object> allocator_;

			FlagGG::Container::HashSet<Object*> objects_;

			FlagGG::Container::PODVector<Object*> delayDestroyObjects_;
		};
	}
}
