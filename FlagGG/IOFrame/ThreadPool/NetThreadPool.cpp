#include "NetThreadPool.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{
	namespace IOFrame
	{
		NetThreadPool::NetThreadPool(size_t thread_count)
			: running_(false)
			, threadCount_(thread_count)
		{ }

		void NetThreadPool::Start()
		{
			running_ = true;

			for (size_t i = 0; i < threadCount_; ++i)
			{
				threadGroup_.emplace_back(new AsyncFrame::Thread::UniqueThread(std::bind(&NetThreadPool::NetThread, this)));
			}
		}

		void NetThreadPool::Stop()
		{
			running_ = false;
		}

		void NetThreadPool::WaitForStop()
		{
			for (size_t i = 0; i < threadGroup_.size(); ++i)
			{
				threadGroup_[i]->WaitForStop();
			}
		}

		boost::asio::io_service& NetThreadPool::getService()
		{
			return service_;
		}

		void NetThreadPool::NetThread()
		{
			while (running_)
			{
				Utility::SystemHelper::Sleep(0);

				service_.run();
			}
		}
	}
}