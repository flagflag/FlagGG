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
				: m_memory(nullptr)
			{
				if (count > 0)
				{
					if (default_memory)
					{
						m_memory = default_memory;
					}
					else
					{
						m_memory = (Type*)malloc(count * (sizeof (Type)));
					}

					m_default = !!default_memory;			
				}
			}

			virtual ~SmartMemory()
			{
				if (m_memory && !m_default)
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

			bool m_default;
		};
	}
}

#endif