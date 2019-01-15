
#include <IOFrame/IOFrame.h>
#include <AsyncFrame/AsyncFrame.h>

#include <windows.h>

#include <iostream>

class ServerHandler : public FlagGG::IOFrame::Handler::EventHandler
{
public:
	~ServerHandler() override = default;

	void channelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		printf("channelCreated\n");
	}

	void channelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		printf("channelOpend\n");
	}

	void channelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		printf("channelClosed\n");
	}

	void messageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer) override
	{
		char* data;
		size_t data_size = 0;
		buffer->toString(data, data_size);

		std::cout << "recive: " << std::string(data, data_size) << '\n';
	}

	void errorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override
	{
		printf("errorCatch %d %s\n", error_code.value(), error_code.message().c_str());
	}
};

void StartServer()
{
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr acceptor = 
		FlagGG::IOFrame::TCP::createAcceptor(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		4);

	if (acceptor->bind("127.0.0.1", 5000))
	{
		acceptor->start();

		printf("succeed to startup server\n");

		acceptor->waitForStop();
	}
	else
	{
		printf("failed to startup server\n");
	}
}

void StartClient()
{
	FlagGG::IOFrame::IOThreadPoolPtr thread_pool = FlagGG::IOFrame::TCP::createThreadPool(1);

	thread_pool->start();

	FlagGG::IOFrame::Connector::IOConnectorPtr connector =
		FlagGG::IOFrame::TCP::createConnector(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		thread_pool);

	connector->connect("127.0.0.1", 5000);

	printf("succeed to startup client\n");

	FlagGG::IOFrame::Buffer::IOBufferPtr buffer = FlagGG::IOFrame::TCP::createBuffer();
	std::string content = "test233";
	buffer->writeStream(content.data(), content.length());
	for (int i = 0; i < 3; ++i)
	{
		bool result = connector->write(buffer);
		printf("write %d result(%d)\n", i, result ? 1 : 0);
	}

	thread_pool->waitForStop();
}

int main()
{
	FlagGG::AsyncFrame::Thread::UniqueThread server_thread(StartServer);

	// 等两秒，保证服务器开启
	Sleep(2000);
	FlagGG::AsyncFrame::Thread::UniqueThread client_thread(StartClient);

	getchar();

    return 0;
}

