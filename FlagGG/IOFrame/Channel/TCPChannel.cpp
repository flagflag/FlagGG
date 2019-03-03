#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include <memory>

#include "TCPChannel.h"
#include "IOFrame/Context/TCPContext.h"
#include "IOFrame/IOError.h"
#include "IOFrame/Buffer/Buffer.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Channel
		{
			TCPChannel::TCPChannel(boost::asio::io_service& service)
				: m_service(service)
				, m_socket(service)
				, m_strand(service)
				, m_state(Free)
				, m_closed(false)
				, m_shutdown(false)
			{ }

			void TCPChannel::HandleWrite(const boost::system::error_code& error_code, size_t bytes_transferred)
			{
				//为了防止在异步发送数据的时候关闭socket出错
				if (m_closed || m_shutdown)
				{
					return;
				}

				if (error_code)
				{
					THROW_IO_ERROR(Context::TCPContext, shared_from_this(), m_handler, error_code);
				}
			}

			bool TCPChannel::Write(Buffer::IOBufferPtr buffer)
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//如果没连接，发送数据毫无意义
				if (m_state != Connected)
				{
					return false;
				}

				char* data = nullptr;
				size_t data_size = 0;
				buffer->ToString(data, data_size);

				if (!data || data_size == 0)
				{
					return false;
				}

				try
				{
					m_socket.async_send(
						boost::asio::buffer(data, data_size),
						m_strand.wrap(boost::bind(&TCPChannel::HandleWrite, 
						std::dynamic_pointer_cast < TCPChannel >(shared_from_this()),
						boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
				}
				catch (...)
				{
					return false;
				}

				return true;
			}

			bool TCPChannel::Flush()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//do something

				return true;
			}

			void TCPChannel::HandleConnect(const boost::system::error_code& error_code)
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//为了防止在异步连接的时候关闭socket出错
				if (m_closed || m_shutdown)
				{
					return;
				}

				if (!error_code)
				{
					//m_state = Connected;

					OnOpend();

					printf("connect succeeded.\n");
				}
				else
				{
					m_state = Free;

					printf("connect failed.\n");

					THROW_IO_ERROR(Context::TCPContext, shared_from_this(), m_handler, error_code);
				}
			}

			bool TCPChannel::Connect(const char* ip, uint16_t port)
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//if (m_state == Connecting || m_state == Connected || m_state == Closed || m_state == Shutdown_)
				if (m_state != Free)
				{
					return false;
				}

				try
				{
					boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);

					m_socket.async_connect(
						endpoint,
						m_strand.wrap(boost::bind(&TCPChannel::HandleConnect, 
						std::dynamic_pointer_cast < TCPChannel >(shared_from_this()),
						boost::asio::placeholders::error)));
				}
				catch (...)
				{
					return false;
				}

				m_state = Connecting;

				return true;
			}

			void TCPChannel::Close()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//if (m_state == Connecting || m_state == Connected)
				if (!m_closed && !m_shutdown)
				{
					//m_socket.cancel();
					m_socket.close();

					m_state = Closed;
					m_closed = true;

					OnClosed();
				}
			}

			void TCPChannel::Shutdown()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//if (m_state == Connecting || m_state == Connected || m_state == Closed)
				if (!m_shutdown)
				{
					m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

					m_state = Shutdown_;
					m_shutdown = true;

					OnClosed();
				}
			}

			bool TCPChannel::IsConnected()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				return m_state == Connected;
			}

			bool TCPChannel::IsClosed()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				return m_closed || m_shutdown;
			}

			boost::asio::ip::tcp::socket& TCPChannel::getSocket()
			{
				return m_socket;
			}

			void TCPChannel::HandleRead(const boost::system::error_code& error_code, size_t bytes_transferred)
			{
				if (!error_code)
				{
					Context::TCPContextPtr context(new Context::TCPContext(shared_from_this()));
					Buffer::NetBufferPtr buffer(new Buffer::NetBuffer);
					buffer->WriteStream(m_buffer, bytes_transferred);

					m_handler->MessageRecived(context, buffer);
				}
				else
				{
					THROW_IO_ERROR(Context::TCPContext, shared_from_this(), m_handler, error_code);
				}

				StartRead();
			}

			void TCPChannel::StartRead()
			{
				//如果没连接，接收不到数据的
				if (m_state != Connected)
				{
					return;
				}

				try
				{
					m_socket.async_receive(
						boost::asio::buffer(m_buffer, ONE_KB),
						m_strand.wrap(boost::bind(&TCPChannel::HandleRead,
						std::dynamic_pointer_cast < TCPChannel >(shared_from_this()),
						boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
						));
				}
				catch (...)
				{
					
				}
			}

			void TCPChannel::OnRegisterd(Handler::EventHandlerPtr handler)
			{
				m_handler = handler ? handler : Handler::NullEventHandlerPtr(new Handler::NullEventHandler);

				Context::TCPContextPtr context(new Context::TCPContext(shared_from_this()));
				m_handler->ChannelRegisterd(context);
			}

			void TCPChannel::OnOpend()
			{
				m_state = Connected;

				Context::TCPContextPtr context(new Context::TCPContext(shared_from_this()));
				m_handler->ChannelOpend(context);

				StartRead();
			}

			void TCPChannel::OnClosed()
			{
				Context::TCPContextPtr context(new Context::TCPContext(shared_from_this()));
				m_handler->ChannelClosed(context);
			}
		}
	}
}