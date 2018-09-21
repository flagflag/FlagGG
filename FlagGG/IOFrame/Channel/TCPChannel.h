#ifndef __TCPCHANNEL__
#define __TCPCHANNEL__
 
#include <boost\asio\io_service.hpp>
#include <boost\asio\io_context_strand.hpp>
#include <boost\asio\ip\tcp.hpp>
#include <boost\asio\error.hpp>
#include <boost\enable_shared_from_this.hpp>

#include "IOFrame\Channel\IOChannel.h"
#include "AsyncFrame\Locker.hpp"

namespace FlagGG
{
	namespace IOFrame
	{
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

			class TCPChannel : public IOChannel
			{
			public:
				TCPChannel(boost::asio::io_service& service);

				virtual ~TCPChannel();

				virtual bool write(Buffer::IOBufferPtr buffer) override;

				virtual bool flush() override;

				virtual bool connect(const char* ip, uint16_t port) override;

				virtual void close() override;

				virtual void shutdown() override;

				virtual bool isConnected() override;

				virtual bool isClosed() override;

				boost::asio::ip::tcp::socket& getSocket();

			protected:
				void handleConnect(const boost::system::error_code& error_code);

				void handleWrite(const boost::system::error_code& error_code, size_t bytes_transferred);

			private:
				boost::asio::io_service&			m_service;

				boost::asio::ip::tcp::socket		m_socket;

				boost::asio::io_service::strand		m_strand;

				TCPConnectionState					m_state;

				bool								m_closed;

				bool								m_shutdown;

				std::recursive_mutex				m_mutex;
			};

			typedef std::shared_ptr < TCPChannel > TCPChannelPtr;
		}
	}
}

#endif