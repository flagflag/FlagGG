#ifndef __SMART_MEMORY__
#define __SMART_MEMORY__

#include "Export.h"

#include <stdlib.h>

namespace FlagGG
{
	namespace Allocator
	{
		template < class Type = char >
		class FlagGG_API SmartMemory
		{
		public:
			SmartMemory(size_t count,
			Type* default_memory = nullptr)
				: memory_(nullptr)
			{
				if (count > 0)
				{
					if (default_memory)
					{
						memory_ = default_memory;
					}
					else
					{
						memory_ = (Type*)malloc(count * (sizeof (Type)));
					}

					default_ = !!default_memory;			
				}
			}

			virtual ~SmartMemory()
			{
				if (memory_ && !default_)
				{
					free(memory_);
				}
			}

			Type* Get()
			{
				return memory_;
			}

		private:
			Type* memory_;

			bool default_;
		};
	}
}

#endif