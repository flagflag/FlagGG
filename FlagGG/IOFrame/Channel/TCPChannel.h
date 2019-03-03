#ifndef __TCPCHANNEL__
#define __TCPCHANNEL__
 
#include <boost/asio/io_service.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/error.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "IOFrame/Channel/IOChannel.h"
#include "IOFrame/Handler/IOHandler.h"
#include "AsyncFrame/Locker.hpp"

#include "Define.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			class TCPAcceptor;
		};

		namespace Connector
		{
			class TCPConnector;
		}

		namespace Channel
		{
			enum TCPConnectionState
			{
				Free		= 1,
				Connected	= 2,
				Connecting	= 3,
				Closed		= 4,
				Shutdown_	= 5,
			};

			class TCPChannel : public IOChannel, public Handler::IOHandler
			{
			public:
				TCPChannel(boost::asio::io_service& service);

				~TCPChannel() override = default;

				//IOChannel interface:

				bool Write(Buffer::IOBufferPtr buffer) override;

				bool Flush() override;

				bool Connect(const char* ip, uint16_t port) override;

				void Close() override;

				void Shutdown() override;

				bool IsConnected() override;

				bool IsClosed() override;

				boost::asio::ip::tcp::socket& getSocket();

				friend class Acceptor::TCPAcceptor;
				friend class Connector::TCPConnector;

			protected:
				void HandleConnect(const boost::system::error_code& error_code);

				void HandleWrite(const boost::system::error_code& error_code, size_t bytes_transferred);

				void HandleRead(const boost::system::error_code& error_code, size_t bytes_transferred);

				void StartRead();

				//IOHandler interface:

				void OnRegisterd(Handler::EventHandlerPtr handler) override;

				void OnOpend() override;

				void OnClosed() override;

			private:
				boost::asio::io_service&			m_service;

				boost::asio::ip::tcp::socket		m_socket;

				boost::asio::io_service::strand		m_strand;

				TCPConnectionState					m_state;

				bool								m_closed;

				bool								m_shutdown;

				std::recursive_mutex				m_mutex;

				char								m_buffer[ONE_KB];

				Handler::EventHandlerPtr			m_handler;
			};

			typedef std::shared_ptr < TCPChannel > TCPChannelPtr;
		}
	}
}

#endif