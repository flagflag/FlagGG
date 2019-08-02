
#include <IOFrame/IOFrame.h>
#include <AsyncFrame/AsyncFrame.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

#include <iostream>

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

		FLAGGG_LOG_INFO("MessageRecived: %s", data.CString());
	}

	void ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override
	{
		FLAGGG_LOG_DEBUG("errorCatch %d %s", error_code.Value(), error_code.Message().c_str());
	}
};

void StartServer()
{
	FLAGGG_LOG_DEBUG("ready start server.");

	FlagGG::IOFrame::Acceptor::IOAcceptorPtr acceptor = 
		FlagGG::IOFrame::TCP::CreateAcceptor(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		1);

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

	FlagGG::IOFrame::IOThreadPoolPtr thread_pool = FlagGG::IOFrame::TCP::CreateThreadPool(1);

	thread_pool->Start();

	FlagGG::IOFrame::Connector::IOConnectorPtr connector =
		FlagGG::IOFrame::TCP::CreateConnector(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		thread_pool);

	connector->Connect("127.0.0.1", 5000);

	FLAGGG_LOG_DEBUG("succeed to startup client");

	FlagGG::IOFrame::Buffer::IOBufferPtr buffer = FlagGG::IOFrame::TCP::CreateBuffer();
	std::string content = "test233";
	buffer->WriteStream(content.data(), content.length());
	for (int i = 0; i < 3; ++i)
	{
		bool result = connector->Write(buffer);
		FLAGGG_LOG_DEBUG("write %d result(%d)", i, result ? 1 : 0);
	}

	thread_pool->WaitForStop();
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

