#ifndef __TCP_ACCEPTOR__
#define __TCP_ACCEPTOR__

#include <boost\asio\io_service.hpp>
#include <boost\asio\ip\tcp.hpp>
#include <boost\asio\error.hpp>

#include "IOFrame\Acceptor\IOAcceptor.h"
#include "IOFrame\Channel\TCPChannel.h"
#include "IOFrame\Handler\EventHandler.h"
#include "IOFrame\ThreadPool\IOThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			class TCPAcceptor : public IOAcceptor
			{
			public:
				TCPAcceptor(Handler::EventHandlerPtr handler, size_t thread_count = 1);

				~TCPAcceptor() override = default;

				bool bind(const char* ip, uint16_t port) override;

				void start() override;

				void stop() override;

				void waitForStop() override;

			protected:
				void startAccept();

				void handleAccept(Channel::TCPChannelPtr channel, const boost::system::error_code& error_code);

			private:
				//声明顺序不能变，C++初始化列表是按照声明顺序执行的！！！

				IOFrame::IOThreadPoolPtr			m_thread_pool;

				boost::asio::ip::tcp::acceptor		m_acceptor;

				Handler::EventHandlerPtr			m_handler;
			};
		}
	}
}

#endif