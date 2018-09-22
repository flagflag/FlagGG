//#include "AsyncFrame\Process\Process.h"
//#include "AsyncFrame\LockQueue.hpp"
//
//#include "AsyncFrame\Thread\UniqueThread.h"
//#include "AsyncFrame\Thread\SharedThread.h"
//#include "AsyncFrame\Thread\ThreadPool.h"

//void ProcessTest()
//{
//	FlagGG::AsyncFrame::Process::ParameterList param;
//	param.add(L"-test1");
//	param.add(L"-test2", 233);
//	param.add(L"-test3", 233.0);
//
//	FlagGG::AsyncFrame::Process::ProcessObjectPtr proc_obj =
//		FlagGG::AsyncFrame::Process::Builder::createProcess(L"C:\\Users\\Administrator\\Desktop\\test.txt", param);
//	if (proc_obj)
//	{
//		printf("create process success!\n");
//	}
//	else
//	{
//		printf("create process failed! error code: %d\n", GetLastError());
//	}
//
//	FlagGG::AsyncFrame::LockQueue < int > temp;
//}
//
//void UniqueThreadTest()
//{
//	FlagGG::AsyncFrame::Thread::UniqueThread thread([&]()
//	{
//		printf("2333\n");
//
//		getchar();
//	});
//
//	thread.waitForStop(2000);
//}
//
//void SharedThreadTest()
//{
//	FlagGG::AsyncFrame::Thread::SharedThread shared_thread;
//
//	shared_thread.start();
//
//	auto func = [&](int x)
//	{
//		printf("%d\n", x);
//	};
//
//	for (int i = 0; i < 1e5; ++i)
//	{
//		shared_thread.add(std::bind(func, i));
//	}
//
//	getchar();
//}
//
//void ThreadPoolTest()
//{
//	FlagGG::AsyncFrame::Thread::ThreadPool thread_pool(8);
//
//	thread_pool.start();
//
//	FILE* write_stream;
//	fopen_s(&write_stream, "C:\\MyC++Projects\\FlagGG\\test.txt", "w");
//
//	auto func = [&](int x)
//	{
//		fprintf(write_stream, "%d\n", x);
//
//		Sleep(16);
//	};
//
//	for (int i = 0; i < 36; ++i)
//	{
//		thread_pool.add(std::bind(func, i));
//	}
//
//	getchar();
//
//	fclose(write_stream);
//}

//void BufferTest()
//{
//	FlagGG::IOFrame::Buffer::NetBuffer buffer;
//
//	//buffer.writeInt32(4294967295U);
//
//	//int32_t _1;
//	//buffer.clearIndex();
//	//buffer.readInt32(_1);
//
//	//uint32_t _2;
//	//buffer.clearIndex();
//	//buffer.readUInt32(_2);
//
//	//printf("%d %u\n", _1, _2);
//
//	buffer.writeStream("23333333", 8);
//
//	char* data = nullptr;
//	size_t data_size = 0;
//	//buffer.clearIndex();
//	buffer.toString(data, data_size);
//	
//	char* temp = new char[data_size + 1];
//	memcpy(temp, data, data_size);
//	temp[data_size] = '\0';
//	printf("%s*\n", temp);
//
//	getchar();
//}

#include "Config\parse.h"

#include "IOFrame\Acceptor\TCPAcceptor.h"
#include "IOFrame\Handler\EventHandler.h"
#include "AsyncFrame\Thread\UniqueThread.h"

#include "Code\Code.h"
#include <fstream>
#include <iostream>

class ServerHandler : public FlagGG::IOFrame::Handler::EventHandler
{
public:
	virtual ~ServerHandler() {}

	virtual void channelCreated(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		printf("channelCreated\n");
	}

	virtual void channelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		printf("channelOpend\n");
	}

	virtual void channelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override
	{
		printf("channelClosed\n");
	}

	virtual void messageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer) override
	{
		char* data;
		size_t data_size = 0;
		buffer->toString(data, data_size);
		

	}

	virtual void errorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override
	{
		printf("errorCatch %d %s\n", error_code.value(), error_code.message().c_str());
	}
};

void StartServer()
{
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr acceptor(
		new FlagGG::IOFrame::Acceptor::TCPAcceptor(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler), 
		4));

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
	boost::asio::io_service io;
}

void Gao()
{
	std::string ansi_string = "我是大麻哈";
	std::wstring unicode_string = FlagGG::Code::AnsiToWide(ansi_string);
	ansi_string = FlagGG::Code::WideToUtf8(unicode_string);
	unicode_string = FlagGG::Code::Utf8ToWide(ansi_string);
	ansi_string = FlagGG::Code::WideToAnsi(unicode_string);


	printf("%d\n", unicode_string.length());
	std::cout << ansi_string << '\n';

	std::wofstream stream;
	stream.open("C:\\test\\gg.txt", std::ios::trunc | std::ios::out);
	stream << unicode_string << '\n';
	stream.close();
}

int main()
{
	//ProcessTest();

	//UniqueThreadTest();

	//SharedThreadTest();

	//ThreadPoolTest();

	//BufferTest();

	//FlagGG::AsyncFrame::Thread::UniqueThread server_thread(StartServer);

	//FlagGG::AsyncFrame::Thread::UniqueThread client_thread(StartClient);

	Gao();

	system("pause");

	return 0;
}