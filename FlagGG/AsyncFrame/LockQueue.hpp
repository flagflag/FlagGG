#ifndef __LOCK_QUEUE__
#define __LOCK_QUEUE__

#include "Export.h"

#include "Locker.hpp"

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
				RecursiveLocker locker(m_mutex);

				m_queue.insert(m_queue.end(), obj);
			}

			void Pop(Object& obj)
			{
				RecursiveLocker locker(m_mutex);

				typename Objects::iterator it = m_queue.begin();
				obj = (*it);
				m_queue.erase(it);
			}

			void Slipce(Objects& objs)
			{
				RecursiveLocker locker(m_mutex);

				objs.splice(objs.begin(), m_queue, m_queue.begin(), m_queue.end());
			}

			size_t Size()
			{
				RecursiveLocker locker(m_mutex);

				size_t _size = m_queue.size();

				return _size;
			}

		private:
			std::recursive_mutex m_mutex;

			Objects m_queue;
		};
	}
}

#endif