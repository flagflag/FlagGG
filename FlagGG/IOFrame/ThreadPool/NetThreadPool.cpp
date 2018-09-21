#include "NetThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		NetThreadPool::NetThreadPool(size_t thread_count)
			: m_running(false)
			, m_thread_count(thread_count)
		{ }

		NetThreadPool::~NetThreadPool()
		{ }

		void NetThreadPool::start()
		{
			m_running = true;

			for (size_t i = 0; i < m_thread_count; ++i)
			{
				m_thread_group.emplace_back(new AsyncFrame::Thread::UniqueThread(std::bind(&NetThreadPool::netThread, this)));
			}
		}

		void NetThreadPool::stop()
		{
			m_running = false;
		}

		void NetThreadPool::waitForStop()
		{
			for (size_t i = 0; i < m_thread_group.size(); ++i)
			{
				m_thread_group[i]->waitForStop();
			}
		}

		boost::asio::io_service& NetThreadPool::getService()
		{
			return m_service;
		}

		void NetThreadPool::netThread()
		{
			while (m_running)
			{
				Sleep(0);

				m_service.run();
			}
		}
	}
}