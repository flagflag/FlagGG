//linux可能会编译不过，所以把头文件引用放在这里！！！很关键！！！
#include "Container/Ptr.h"

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include "TCPChannel.h"
#include "IOFrame/Context/TCPContext.h"
#include "IOFrame/IOError.h"
#include "IOFrame/Buffer/NetBuffer.h"
#include "Log.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Channel
		{
			TCPChannel::TCPChannel(boost::asio::io_service& service)
				: service_(service)
				, socket_(service)
				, strand_(service)
				, state_(Free)
				, closed_(false)
				, shutdown_(false)
			{ }

			void TCPChannel::HandleWrite(const boost::system::error_code& error_code, size_t bytes_transferred)
			{
				//为了防止在异步发送数据的时候关闭socket出错
				if (closed_ || shutdown_)
				{
					return;
				}

				if (error_code)
				{
					THROW_IO_ERROR(Context::TCPContext, Container::SharedPtr<IOChannel>(this), handler_, error_code);
				}
			}

			bool TCPChannel::Write(Buffer::IOBufferPtr buffer)
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				//如果没连接，发送数据毫无意义
				if (state_ != Connected)
				{
					return false;
				}

				Container::SharedArrayPtr<char> data;
				uint32_t dataSize;
				buffer->ToBuffer(data, dataSize);

				if (!data || dataSize == 0)
				{
					return false;
				}

				try
				{
					socket_.async_send(
						boost::asio::buffer(data.Get(), dataSize),
						strand_.wrap(boost::bind(&TCPChannel::HandleWrite, 
						Container::SharedPtr<TCPChannel>(this),
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
				AsyncFrame::RecursiveLocker locker(mutex_);

				//do something

				return true;
			}

			void TCPChannel::HandleConnect(const boost::system::error_code& error_code)
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				//为了防止在异步连接的时候关闭socket出错
				if (closed_ || shutdown_)
				{
					return;
				}

				if (!error_code)
				{
					//state_ = Connected;

					OnOpend();

					FLAGGG_LOG_ERROR("Connect succeeded.");
				}
				else
				{
					state_ = Free;

					FLAGGG_LOG_ERROR("Connect failed.");

					THROW_IO_ERROR(Context::TCPContext, Container::SharedPtr<IOChannel>(this), handler_, error_code);
				}
			}

			bool TCPChannel::Connect(const char* ip, uint16_t port)
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				//if (state_ == Connecting || state_ == Connected || state_ == Closed || state_ == Shutdown_)
				if (state_ != Free)
				{
					return false;
				}

				try
				{
					boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);

					socket_.async_connect(
						endpoint,
						strand_.wrap(boost::bind(&TCPChannel::HandleConnect, 
						Container::SharedPtr<TCPChannel>(this),
						boost::asio::placeholders::error)));
				}
				catch (...)
				{
					return false;
				}

				state_ = Connecting;

				return true;
			}

			void TCPChannel::Close()
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				//if (state_ == Connecting || state_ == Connected)
				if (!closed_ && !shutdown_)
				{
					//socket_.cancel();
					socket_.close();

					state_ = Closed;
					closed_ = true;

					OnClosed();
				}
			}

			void TCPChannel::Shutdown()
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				//if (state_ == Connecting || state_ == Connected || state_ == Closed)
				if (!shutdown_)
				{
					socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

					state_ = Shutdown_;
					shutdown_ = true;

					OnClosed();
				}
			}

			bool TCPChannel::IsConnected()
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				return state_ == Connected;
			}

			bool TCPChannel::IsClosed()
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				return closed_ || shutdown_;
			}

			boost::asio::ip::tcp::socket& TCPChannel::getSocket()
			{
				return socket_;
			}

			void TCPChannel::HandleRead(const boost::system::error_code& error_code, size_t bytes_transferred)
			{
				if (!error_code)
				{
					Context::TCPContextPtr context(new Context::TCPContext(Container::SharedPtr<IOChannel>(this)));
					Buffer::NetBufferPtr buffer(new Buffer::NetBuffer);
					buffer->WriteStream(buffer_, bytes_transferred);

					handler_->MessageRecived(context, buffer);
				}
				else
				{
					THROW_IO_ERROR(Context::TCPContext, Container::SharedPtr<IOChannel>(this), handler_, error_code);
				}

				StartRead();
			}

			void TCPChannel::StartRead()
			{
				//如果没连接，接收不到数据的
				if (state_ != Connected)
				{
					return;
				}

				try
				{
					socket_.async_receive(
						boost::asio::buffer(buffer_, ONE_KB),
						strand_.wrap(boost::bind(&TCPChannel::HandleRead,
						Container::SharedPtr<TCPChannel>(this),
						boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
						));
				}
				catch (...)
				{
					
				}
			}

			void TCPChannel::OnRegisterd(Handler::EventHandlerPtr handler)
			{
				handler_ = handler ? handler : Handler::EventHandlerPtr(new Handler::NullEventHandler);

				Context::TCPContextPtr context(new Context::TCPContext(Container::SharedPtr<IOChannel>(this)));
				handler_->ChannelRegisterd(context);
			}

			void TCPChannel::OnOpend()
			{
				state_ = Connected;

				Context::TCPContextPtr context(new Context::TCPContext(Container::SharedPtr<IOChannel>(this)));
				handler_->ChannelOpend(context);

				StartRead();
			}

			void TCPChannel::OnClosed()
			{
				Context::TCPContextPtr context(new Context::TCPContext(Container::SharedPtr<IOChannel>(this)));
				handler_->ChannelClosed(context);
			}
		}
	}
}