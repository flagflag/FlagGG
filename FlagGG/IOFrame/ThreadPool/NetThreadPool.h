#ifndef __NET_THREAD_POOL__
#define __NET_THREAD_POOL__

#include <boost\asio\io_service.hpp>

#include <atomic>

#include "AsyncFrame\KernelObject\Runtime.h"
#include "AsyncFrame\Thread\UniqueThread.h"

namespace FlagGG
{
	namespace IOFrame
	{
		class NetThreadPool : AsyncFrame::KernelObject::Runtime
		{
		public:
			NetThreadPool(size_t thread_count);

			virtual ~NetThreadPool();

			void start();

			virtual void stop() override;

			virtual void waitForStop() override;

			boost::asio::io_service& getService();

		private:
			void netThread();

			virtual void waitForStop(DWORD wait_time) override { };

		private:
			size_t												m_thread_count;

			boost::asio::io_service								m_service;

			std::vector < AsyncFrame::Thread::UniqueThreadPtr > m_thread_group;

			std::atomic < bool >								m_running;
		};

		typedef std::shared_ptr < NetThreadPool > NetThreadPoolPtr;
	}
}

#endif