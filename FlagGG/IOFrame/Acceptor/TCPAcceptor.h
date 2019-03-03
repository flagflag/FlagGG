#ifndef __TCP_ACCEPTOR__
#define __TCP_ACCEPTOR__

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/error.hpp>

#include "IOFrame/Acceptor/IOAcceptor.h"
#include "IOFrame/Channel/TCPChannel.h"
#include "IOFrame/Handler/EventHandler.h"
#include "IOFrame/ThreadPool/IOThreadPool.h"

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

				bool Bind(const char* ip, uint16_t port) override;

				void Start() override;

				void Stop() override;

				void WaitForStop() override;

			protected:
				void StartAccept();

				void HandleAccept(Channel::TCPChannelPtr channel, const boost::system::error_code& error_code);

			private:
				//声明顺序不能变，C++初始化列表是按照声明顺序执行的！！！

				IOFrame::IOThreadPoolPtr			threadPool_;

				boost::asio::ip::tcp::acceptor		acceptor_;

				Handler::EventHandlerPtr			handler_;
			};
		}
	}
}

#endif