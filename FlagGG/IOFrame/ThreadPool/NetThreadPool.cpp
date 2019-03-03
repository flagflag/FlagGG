#include "NetThreadPool.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{
	namespace IOFrame
	{
		NetThreadPool::NetThreadPool(size_t thread_count)
			: m_running(false)
			, m_thread_count(thread_count)
		{ }

		void NetThreadPool::Start()
		{
			m_running = true;

			for (size_t i = 0; i < m_thread_count; ++i)
			{
				m_thread_group.emplace_back(new AsyncFrame::Thread::UniqueThread(std::bind(&NetThreadPool::NetThread, this)));
			}
		}

		void NetThreadPool::Stop()
		{
			m_running = false;
		}

		void NetThreadPool::WaitForStop()
		{
			for (size_t i = 0; i < m_thread_group.size(); ++i)
			{
				m_thread_group[i]->WaitForStop();
			}
		}

		boost::asio::io_service& NetThreadPool::getService()
		{
			return m_service;
		}

		void NetThreadPool::NetThread()
		{
			while (m_running)
			{
				Utility::SystemHelper::Sleep(0);

				m_service.run();
			}
		}
	}
}