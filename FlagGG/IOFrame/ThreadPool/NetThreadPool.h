#ifndef __NET_THREAD_POOL__
#define __NET_THREAD_POOL__

#include <boost\asio\io_service.hpp>

#include <atomic>

#include "AsyncFrame\Thread\UniqueThread.h"
#include "IOThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		class NetThreadPool : public IOThreadPool
		{
		public:
			NetThreadPool(size_t thread_count);

			~NetThreadPool() override = default;

			void start() override;

			void stop() override;

			void waitForStop() override;

			boost::asio::io_service& getService();

		private:
			void netThread();

			void waitForStop(uint32_t wait_time) override { };

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