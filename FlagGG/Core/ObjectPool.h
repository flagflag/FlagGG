#pragma once

#include "Container/Allocator.h"
#include "Container/HashSet.h"

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

		private:
			FlagGG::Container::Allocator<Object> allocator_;

			FlagGG::Container::HashSet<Object*> objects_;
		};
	}
}
