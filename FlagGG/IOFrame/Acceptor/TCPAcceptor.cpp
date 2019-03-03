
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include "TCPAcceptor.h"
#include "IOFrame/IOError.h"
#include "IOFrame/Context/TCPContext.h"
#include "IOFrame/ThreadPool/NetThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			TCPAcceptor::TCPAcceptor(Handler::EventHandlerPtr handler, size_t thread_count)
				: m_thread_pool(new IOFrame::NetThreadPool(thread_count))
				, m_acceptor(std::dynamic_pointer_cast<IOFrame::NetThreadPool>(m_thread_pool)->getService())
				, m_handler(handler)
			{ }

			bool TCPAcceptor::Bind(const char* ip, uint16_t port)
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

			void TCPAcceptor::Start()
			{
				m_thread_pool->Start();

				StartAccept();
			}

			void TCPAcceptor::Stop()
			{
				m_acceptor.cancel();
				m_acceptor.close();

				m_thread_pool->Stop();
			}

			void TCPAcceptor::WaitForStop()
			{
				m_thread_pool->WaitForStop();
			}

			void TCPAcceptor::StartAccept()
			{
				Channel::TCPChannelPtr channel(new Channel::TCPChannel(std::dynamic_pointer_cast<IOFrame::NetThreadPool>(m_thread_pool)->getService()));

				channel->OnRegisterd(m_handler);

				m_acceptor.async_accept(
					channel->getSocket(),
					boost::bind(&TCPAcceptor::HandleAccept, this, channel,
					boost::asio::placeholders::error));
			}

			void TCPAcceptor::HandleAccept(Channel::TCPChannelPtr channel, const boost::system::error_code& error_code)
			{
				if (m_acceptor.is_open())
				{
					if (!error_code)
					{
						channel->OnOpend();
					}
					else
					{
						THROW_IO_ERROR(Context::TCPContext, channel, m_handler, error_code);
					}

					StartAccept();
				}
			}
		}
	}
}