#ifndef FLAGGG_NO_BOOST
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
			TCPAcceptor::TCPAcceptor(Handler::EventHandlerPtr handler, Size threadCount)
				: threadPool_(new IOFrame::NetThreadPool(threadCount))
				, acceptor_(threadPool_->getService())
				, handler_(handler)
			{ }

			bool TCPAcceptor::Bind(const char* ip, UInt16 port)
			{
				boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
				boost::system::error_code error_code;

				acceptor_.open(endpoint.protocol());
				acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				acceptor_.bind(endpoint, error_code);
				
				if (!error_code)
				{
					acceptor_.listen();
				}
				
				return !error_code;
			}

			void TCPAcceptor::Start()
			{
				threadPool_->Start();

				StartAccept();
			}

			void TCPAcceptor::Stop()
			{
				acceptor_.cancel();
				acceptor_.close();

				threadPool_->Stop();
			}

			void TCPAcceptor::WaitForStop()
			{
				threadPool_->WaitForStop();
			}

			void TCPAcceptor::StartAccept()
			{
				Channel::TCPChannelPtr channel(new Channel::TCPChannel(threadPool_->getService()));

				channel->OnRegisterd(handler_);

				acceptor_.async_accept(
					channel->getSocket(),
					boost::bind(&TCPAcceptor::HandleAccept, this, channel,
					boost::asio::placeholders::error));
			}

			void TCPAcceptor::HandleAccept(Channel::TCPChannelPtr channel, const boost::system::error_code& error_code)
			{
				if (acceptor_.is_open())
				{
					if (!error_code)
					{
						channel->OnOpend();
					}
					else
					{
						THROW_IO_ERROR(Context::TCPContext, channel, handler_, error_code);
					}

					StartAccept();
				}
			}
		}
	}
}
#endif
