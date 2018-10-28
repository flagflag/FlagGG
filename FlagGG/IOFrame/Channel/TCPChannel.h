#ifndef __TCPCHANNEL__
#define __TCPCHANNEL__
 
#include <boost\asio\io_service.hpp>
#include <boost\asio\io_context_strand.hpp>
#include <boost\asio\ip\tcp.hpp>
#include <boost\asio\error.hpp>
#include <boost\enable_shared_from_this.hpp>

#include "IOFrame\Channel\IOChannel.h"
#include "IOFrame\Handler\IOHandler.h"
#include "AsyncFrame\Locker.hpp"

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
				Shutdown	= 5,
			};

			class TCPChannel : public IOChannel, public Handler::IOHandler
			{
			public:
				TCPChannel(boost::asio::io_service& service);

				virtual ~TCPChannel();

				//IOChannel interface:

				virtual bool write(Buffer::IOBufferPtr buffer) override;

				virtual bool flush() override;

				virtual bool connect(const char* ip, uint16_t port) override;

				virtual void close() override;

				virtual void shutdown() override;

				virtual bool isConnected() override;

				virtual bool isClosed() override;

				boost::asio::ip::tcp::socket& getSocket();

				friend class Acceptor::TCPAcceptor;
				friend class Connector::TCPConnector;

			protected:
				void handleConnect(const boost::system::error_code& error_code);

				void handleWrite(const boost::system::error_code& error_code, size_t bytes_transferred);

				void handleRead(const boost::system::error_code& error_code, size_t bytes_transferred);

				void startRead();

				//IOHandler interface:

				virtual void onRegisterd(Handler::EventHandlerPtr handler) override;

				virtual void onOpend() override;

				virtual void onClosed() override;

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