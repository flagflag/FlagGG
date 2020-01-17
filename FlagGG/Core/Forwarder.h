#pragma once

#include <functional>

#include "Container/List.h"
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace Core
	{
		using ForwarderFuncType = std::function<void()>;

		template < class MutexType >
		class Forwarder : public RefCounted
		{
		public:
			void Forward(ForwarderFuncType&& func)
			{
				mutex_.Lock();
				queue_.Push(std::move(func));
				mutex_.UnLock();
			}

			void Forward(const ForwarderFuncType& func)
			{
				mutex_.Lock();
				queue_.Push(func);
				mutex_.UnLock();
			}

			void Execute()
			{
				bool isLock = mutex_.TryLock();

				if (isLock)
				{
					cache_ = queue_;
					queue_.Clear();

					mutex_.UnLock();

					for (auto it = cache_.Begin(); it != cache_.End(); ++it)
					{
						(*it)();
					}
				}				
			}

		private:
			MutexType mutex_;

			Container::List<ForwarderFuncType> queue_;
			Container::List<ForwarderFuncType> cache_;
		};
	}
}
