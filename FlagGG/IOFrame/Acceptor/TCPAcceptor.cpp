#include <boost\bind.hpp>
#include <boost\asio\placeholders.hpp>

#include "TCPAcceptor.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			TCPAcceptor::TCPAcceptor(Handler::EventHandlerPtr handler, size_t thread_count)
				: m_thread_pool(new IOFrame::NetThreadPool(thread_count))
				, m_acceptor(m_thread_pool->getService())
				, m_handler(handler)
			{ }

			TCPAcceptor::~TCPAcceptor()
			{ }

			bool TCPAcceptor::bind(const char* ip, uint16_t port)
			{
				boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
				boost::system::error_code error_code;

				m_acceptor.open(endpoint.protocol());
				m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				m_acceptor.bind(endpoint, error_code);
				
				if (!error_code)
				{
					m_acceptor.listen();
				}
				
				return !error_code;
			}

			void TCPAcceptor::start()
			{
				m_thread_pool->start();

				startAccept();
			}

			void TCPAcceptor::stop()
			{
				m_acceptor.cancel();
				m_acceptor.close();

				m_thread_pool->stop();
			}

			void TCPAcceptor::waitForStop()
			{
				m_thread_pool->waitForStop();
			}

			void TCPAcceptor::startAccept()
			{
				Channel::TCPChannelPtr channel(new Channel::TCPChannel(m_thread_pool->getService()));

				m_acceptor.async_accept(
					channel->getSocket(),
					boost::bind(&TCPAcceptor::handleAccept, this, channel,
					boost::asio::placeholders::error));
			}

			void TCPAcceptor::handleAccept(Channel::TCPChannelPtr channel, const boost::system::error_code& error_code)
			{
				if (m_acceptor.is_open())
				{
					if (!error_code)
					{
						//m_handler->channelOpend(channel);
					}
					else
					{
						printf("%d %s\n", error_code.value(), error_code.message());
					}

					startAccept();
				}
			}
		}
	}
}