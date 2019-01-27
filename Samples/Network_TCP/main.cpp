
#include <IOFrame/IOFrame.h>
#include <AsyncFrame/AsyncFrame.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

#include <iostream>

class ServerHandler : public FlagGG::IOFrame::Handler::EventHandler
{
public:
	~ServerHandler() override = default;

	void channelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelCreated");
	}

	void channelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelOpend");
	}

	void channelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		FLAGGG_LOG_DEBUG("channelClosed");
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
		FLAGGG_LOG_DEBUG("errorCatch %d %s", error_code.value(), error_code.message().c_str());
	}
};

void StartServer()
{
	FLAGGG_LOG_DEBUG("ready start server.");

	FlagGG::IOFrame::Acceptor::IOAcceptorPtr acceptor = 
		FlagGG::IOFrame::TCP::createAcceptor(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		1);

	if (acceptor->bind("127.0.0.1", 5000))
	{
		acceptor->start();

		FLAGGG_LOG_DEBUG("succeed to startup server");

		acceptor->waitForStop();
	}
	else
	{
		FLAGGG_LOG_DEBUG("failed to startup server");
	}
}

void StartClient()
{
	FLAGGG_LOG_DEBUG("ready start client.");

	FlagGG::IOFrame::IOThreadPoolPtr thread_pool = FlagGG::IOFrame::TCP::createThreadPool(1);

	thread_pool->start();

	FlagGG::IOFrame::Connector::IOConnectorPtr connector =
		FlagGG::IOFrame::TCP::createConnector(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		thread_pool);

	connector->connect("127.0.0.1", 5000);

	FLAGGG_LOG_DEBUG("succeed to startup client");

	FlagGG::IOFrame::Buffer::IOBufferPtr buffer = FlagGG::IOFrame::TCP::createBuffer();
	std::string content = "test233";
	buffer->writeStream(content.data(), content.length());
	for (int i = 0; i < 3; ++i)
	{
		bool result = connector->write(buffer);
		FLAGGG_LOG_DEBUG("write %d result(%d)", i, result ? 1 : 0);
	}

	thread_pool->waitForStop();
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

