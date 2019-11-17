
#include <IOFrame/IOFrame.h>
#include <AsyncFrame/AsyncFrame.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

#include <iostream>

// #define USE_TCP

class ServerHandler : public FlagGG::IOFrame::Handler::EventHandler
{
public:
	~ServerHandler() override = default;

	void ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelCreated");
	}

	void ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelOpend");
	}

	void ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelClosed");
	}

	void MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer) override
	{
		FlagGG::Container::String data;
		buffer->ToString(data);

		FLAGGG_LOG_INFO("MessageRecived: {}", data.CString());
	}

	void ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override
	{
		FLAGGG_LOG_DEBUG("errorCatch {} {}", error_code.Value(), error_code.Message().CString());
	}
};

void StartServer()
{
	FLAGGG_LOG_DEBUG("ready start server.");
#ifdef USE_TCP
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr acceptor = 
		FlagGG::IOFrame::TCP::CreateAcceptor(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		1);
#else
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr acceptor =
		FlagGG::IOFrame::UDP::CreateAcceptor(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler)
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
	FlagGG::IOFrame::IOThreadPoolPtr threadPool = FlagGG::IOFrame::TCP::CreateThreadPool(1);

	threadPool->Start();

	FlagGG::IOFrame::Connector::IOConnectorPtr connector =
		FlagGG::IOFrame::TCP::CreateConnector(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		threadPool);

	connector->Connect("127.0.0.1", 5000);

	FLAGGG_LOG_DEBUG("succeed to startup client");

	FlagGG::Utility::SystemHelper::Sleep(1000);

	FlagGG::IOFrame::Buffer::IOBufferPtr buffer = FlagGG::IOFrame::TCP::CreateBuffer();
	std::string content = "test233";
	buffer->WriteStream(content.data(), content.length());
	for (int i = 0; i < 3; ++i)
	{
		bool result = connector->Write(buffer);
		FLAGGG_LOG_DEBUG("write {} result({})", i, result ? 1 : 0);
	}
#else
	FlagGG::IOFrame::IOThreadPoolPtr threadPool = FlagGG::IOFrame::UDP::CreateThreadPool();

	threadPool->Start();

	FlagGG::IOFrame::Connector::IOConnectorPtr connector =
		FlagGG::IOFrame::UDP::CreateConnector(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		threadPool);

	connector->Connect("127.0.0.1", 5000);

	FLAGGG_LOG_DEBUG("succeed to startup client");

	FlagGG::Utility::SystemHelper::Sleep(1000);

	FlagGG::IOFrame::Buffer::IOBufferPtr buffer = FlagGG::IOFrame::UDP::CreateBuffer();
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

	FlagGG::AsyncFrame::Thread::UniqueThread server_thread(StartServer);

	// �����룬��֤����������
	FlagGG::Utility::SystemHelper::Sleep(2000);
	FlagGG::AsyncFrame::Thread::UniqueThread client_thread(StartClient);

	FLAGGG_LOG_DEBUG("all running...");

	getchar();

    return 0;
}

