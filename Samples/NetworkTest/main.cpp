
#include <IOFrame/IOFrame.h>
#include <AsyncFrame/AsyncFrame.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

#include <iostream>

// #define USE_TCP

using namespace FlagGG;

class ServerHandler : public IOFrame::Handler::EventHandler
{
public:
	~ServerHandler() override = default;

	void ChannelRegisterd(IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelCreated");
	}

	void ChannelOpend(IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelOpend");
	}

	void ChannelClosed(IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelClosed");
	}

	void MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer) override
	{
		String data;
		buffer->ToString(data);

		FLAGGG_LOG_INFO("MessageRecived: {}", data.CString());
	}

	void ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& error_code) override
	{
		FLAGGG_LOG_DEBUG("errorCatch {} {}", error_code.Value(), error_code.Message().CString());
	}
};

void StartServer()
{
	FLAGGG_LOG_DEBUG("ready start server.");
#ifdef USE_TCP
	IOFrame::Acceptor::IOAcceptorPtr acceptor = 
		IOFrame::TCP::CreateAcceptor(
		IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		1);
#else
	IOFrame::Acceptor::IOAcceptorPtr acceptor =
		IOFrame::UDP::CreateAcceptor(
		IOFrame::Handler::EventHandlerPtr(new ServerHandler)
		);
#endif

	if (acceptor->Bind("127.0.0.1", 5000))
	{
		acceptor->Start();

		FLAGGG_LOG_DEBUG("succeed to startup server");

		acceptor->WaitForStop();
	}
	else
	{
		FLAGGG_LOG_DEBUG("failed to startup server");
	}
}

void StartClient()
{
	FLAGGG_LOG_DEBUG("ready start client.");
#ifdef USE_TCP
	IOFrame::IOThreadPoolPtr threadPool = IOFrame::TCP::CreateThreadPool(1);

	threadPool->Start();

	IOFrame::Connector::IOConnectorPtr connector =
		IOFrame::TCP::CreateConnector(
		IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		threadPool);

	connector->Connect("127.0.0.1", 5000);

	FLAGGG_LOG_DEBUG("succeed to startup client");

	FlagGG::Sleep(1000);

	IOFrame::Buffer::IOBufferPtr buffer = IOFrame::TCP::CreateBuffer();
	std::string content = "test233";
	buffer->WriteStream(content.data(), content.length());
	for (int i = 0; i < 3; ++i)
	{
		bool result = connector->Write(buffer);
		FLAGGG_LOG_DEBUG("write {} result({})", i, result ? 1 : 0);
	}
#else
	IOFrame::IOThreadPoolPtr threadPool = IOFrame::UDP::CreateThreadPool();

	threadPool->Start();

	IOFrame::Connector::IOConnectorPtr connector =
		IOFrame::UDP::CreateConnector(
		IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		threadPool);

	connector->Connect("127.0.0.1", 5000);

	FLAGGG_LOG_DEBUG("succeed to startup client");

	FlagGG::Sleep(1000);

	IOFrame::Buffer::IOBufferPtr buffer = IOFrame::UDP::CreateBuffer();
	std::string content = "test2333";
	buffer->WriteStream(content.data(), content.length());
	for (int i = 0; i < 10; ++i)
	{
		bool result = connector->Write(buffer);
		FLAGGG_LOG_DEBUG("write {} result({})", i, result ? 1 : 0);
	}
#endif
	threadPool->WaitForStop();
}

int main()
{
	FLAGGG_LOG_DEBUG("ready run all.");

	UniqueThread server_thread(StartServer);

	FlagGG::Sleep(2000);
	UniqueThread client_thread(StartClient);

	FLAGGG_LOG_DEBUG("all running...");

	getchar();

    return 0;
}

