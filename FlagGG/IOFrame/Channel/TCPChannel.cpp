#include <boost\bind.hpp>
#include <boost\asio\placeholders.hpp>

#include <memory>

#include "TCPChannel.h"

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

			TCPChannel::~TCPChannel()
			{ }

			void TCPChannel::handleWrite(const boost::system::error_code& error_code, size_t bytes_transferred)
			{
				//为了防止在异步发送数据的时候关闭socket出错
				if (m_closed || m_shutdown)
				{
					return;
				}

				if (error_code)
				{
					printf("%d %s.\n", error_code.value(), error_code.message());
				}
			}

			bool TCPChannel::write(Buffer::IOBufferPtr buffer)
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//如果没连接，发送数据毫无意义
				if (m_state != Connected)
				{
					return false;
				}

				char* data = nullptr;
				size_t data_size = 0;
				buffer->toString(data, data_size);

				if (!data || data_size == 0)
				{
					return false;
				}

				try
				{
					m_socket.async_send(
						boost::asio::buffer(data, data_size),
						m_strand.wrap(boost::bind(&TCPChannel::handleWrite, 
						std::dynamic_pointer_cast < TCPChannel >(shared_from_this()),
						boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
				}
				catch (...)
				{
					return false;
				}

				return true;
			}

			bool TCPChannel::flush()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//do something

				return true;
			}

			void TCPChannel::handleConnect(const boost::system::error_code& error_code)
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//为了防止在异步连接的时候关闭socket出错
				if (m_closed || m_shutdown)
				{
					return;
				}

				if (!error_code)
				{
					m_state = Connected;

					printf("connect succeeded.\n");
				}
				else
				{
					m_state = Free;

					printf("connect failed.\n");
				}
			}

			bool TCPChannel::connect(const char* ip, uint16_t port)
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//if (m_state == Connecting || m_state == Connected || m_state == Closed || m_state == Shutdown)
				if (m_state != Free)
				{
					return false;
				}

				try
				{
					boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);

					m_socket.async_connect(
						endpoint,
						m_strand.wrap(boost::bind(&TCPChannel::handleConnect, 
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

			void TCPChannel::close()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//if (m_state == Connecting || m_state == Connected)
				if (!m_closed && !m_shutdown)
				{
					//m_socket.cancel();
					m_socket.close();

					m_state = Closed;
					m_closed = true;
				}
			}

			void TCPChannel::shutdown()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				//if (m_state == Connecting || m_state == Connected || m_state == Closed)
				if (!m_shutdown)
				{
					m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

					m_state = Shutdown;
					m_shutdown = true;
				}
			}

			bool TCPChannel::isConnected()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				return m_state == Connected;
			}

			bool TCPChannel::isClosed()
			{
				AsyncFrame::RecursiveLocker locker(m_mutex);

				return m_closed || m_shutdown;
			}

			boost::asio::ip::tcp::socket& TCPChannel::getSocket()
			{
				return m_socket;
			}
		}
	}
}