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
#include "IOFrame\Connector\TCPConnector.h"
#include "IOFrame\Handler\EventHandler.h"
#include "AsyncFrame\Thread\UniqueThread.h"

#include "Code\Code.h"
#include <fstream>
#include <iostream>

#include "Utility\SystemHelper.h"

#include "LScript\Interpreter.h"

#include "Demo/Demo1/demo1.hpp"
#include "Demo/Demo2/demo2.hpp"

class ServerHandler : public FlagGG::IOFrame::Handler::EventHandler
{
public:
	virtual ~ServerHandler() {}

	virtual void channelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override
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
		
		std::cout << "recive: " << std::string(data, data_size) << '\n';
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
	FlagGG::IOFrame::NetThreadPool thread_pool(1);

	thread_pool.start();

	FlagGG::IOFrame::Connector::TCPConnectorPtr connector(
		new FlagGG::IOFrame::Connector::TCPConnector(
		FlagGG::IOFrame::Handler::EventHandlerPtr(new ServerHandler),
		thread_pool));

	connector->connect("127.0.0.1", 5000);

	printf("succeed to startup client\n");

	FlagGG::IOFrame::Buffer::NetBufferPtr buffer(new FlagGG::IOFrame::Buffer::NetBuffer);
	std::string content = "test233";
	buffer->writeStream(content.data(), content.length());
	for (int i = 0; i < 3; ++i)
	{
		bool result = connector->write(buffer);
		printf("write %d result(%d)\n", i, result ? 1 : 0);
	}

	thread_pool.waitForStop();
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

void SystemHelper()
{
	std::wstring temp = L"//\\2333\\/////\\23";
	std::wcout << FlagGG::Utility::SystemHelper::FormatPath(temp) << '\n';
}

void LScriptTest()
{

}

int main()
{
	//ProcessTest();

	//UniqueThreadTest();

	//SharedThreadTest();

	//ThreadPoolTest();

	//BufferTest();

	FlagGG::AsyncFrame::Thread::UniqueThread server_thread(StartServer);

	//等两秒，保证服务器开启
	Sleep(2000);
	FlagGG::AsyncFrame::Thread::UniqueThread client_thread(StartClient);

	//Gao();

	//SystemHelper();

	//LScriptTest();

	//Demo1Run();

	//Demo2Run();

	//system("pause");
	getchar();

	return 0;
}