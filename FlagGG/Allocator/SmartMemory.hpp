#ifndef __SMART_MEMORY__
#define __SMART_MEMORY__

#include <stdlib.h>

namespace FlagGG
{
	namespace Allocator
	{
		template < class Type = char >
		class SmartMemory
		{
		public:
			SmartMemory(size_t count)
				: m_memory(nullptr)
			{
				if (count > 0)
				{
					m_memory = (Type*)malloc(count * (sizeof Type));
				}
			}

			virtual ~SmartMemory()
			{
				if (m_memory)
				{
					free(m_memory);
				}
			}

			Type* get()
			{
				return m_memory;
			}

		private:
			Type* m_memory;
		};
	}
}

#endif