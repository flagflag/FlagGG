#pragma once

namespace FlagGG
{
	namespace Utility
	{
		template < class InsType, class MutexType, class ... Args >
		class Singleton
		{
		public:
			static InsType* Instance()
			{
				return instance_;
			}

			static InsType* CreateInstance(Args ... args)
			{
				if (!initialized_)
				{
					if (!initialized_)
					{
						instanceMutex_.Lock();

						instance_ = new InsType(args ...);
						initialized_ = true;

						instanceMutex_.UnLock();
					}
				}

				return instance_;
			}

			static void DestroyInstance()
			{
				if (initialized_)
				{
					if (initialized_)
					{
						instanceMutex_.Lock();

						delete instance_;
						instance_ = nullptr;
						initialized_ = false;

						instanceMutex_.UnLock();
					}
				}
			}

			Singleton(const Singleton& ins) = delete;

			Singleton& operator=(const Singleton& ins) = delete;

		private:
			Singleton() = default;

			virtual ~Singleton() = default;

			friend InsType;

			static volatile bool initialized_;
			static InsType* instance_;
			static MutexType instanceMutex_;
		};

		template < class InsType, class MutexType, class ... Args >
		volatile bool Singleton<InsType, MutexType, Args ... >::initialized_ = false;

		template < class InsType, class MutexType, class ... Args >
		InsType* Singleton<InsType, MutexType, Args ... >::instance_ = nullptr;

		template < class InsType, class MutexType, class ... Args >
		MutexType Singleton<InsType, MutexType, Args ... >::instanceMutex_;
	}
}
