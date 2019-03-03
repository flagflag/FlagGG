#ifndef __NET_THREAD_POOL__
#define __NET_THREAD_POOL__

#include <boost/asio/io_service.hpp>

#include <atomic>

#include "AsyncFrame/Thread/UniqueThread.h"
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

			void Start() override;

			void Stop() override;

			void WaitForStop() override;

			boost::asio::io_service& getService();

		private:
			void NetThread();

			void WaitForStop(uint32_t wait_time) override { };

		private:
			size_t												threadCount_;

			boost::asio::io_service								service_;

			std::vector < AsyncFrame::Thread::UniqueThreadPtr > threadGroup_;

			std::atomic < bool >								running_;
		};

		typedef Container::SharedPtr < NetThreadPool > NetThreadPoolPtr;
	}
}

#endif