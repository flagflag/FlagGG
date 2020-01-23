#include <AsyncFrame/Thread/SharedThread.h>
#include <AsyncFrame/Thread/ThreadPool.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

static UInt32 count = 0;
void ThreadFunc(Int32 index)
{
	++count;
	FLAGGG_LOG_INFO("{} {}", count, index);
}

void OneThreadTest()
{
	FlagGG::AsyncFrame::Thread::SharedThread thread;
	thread.Start();

	for (UInt32 i = 0; i < 1e5; ++i)
	{
		thread.Add(std::bind(&ThreadFunc, i));
	}

	FlagGG::Utility::SystemHelper::Sleep(10000000);
}

void MultiThreadTest()
{
	FlagGG::AsyncFrame::Thread::SharedThread thread;

	thread.Start();

	FlagGG::AsyncFrame::Thread::UniqueThreadPtr workThread[4];
	for (UInt32 i = 0; i < 4; ++i)
	{
		workThread[i] = new FlagGG::AsyncFrame::Thread::UniqueThread([&]
		{
			for (UInt32 j = 0; j < 1e5; ++j)
			{
				thread.Add(std::bind(&ThreadFunc, i * j));
			}
		});
	}

	FlagGG::Utility::SystemHelper::Sleep(10000000);
}

int main()
{
	freopen("E:\\test.txt", "w", stdout);
	OneThreadTest();
	// MultiThreadTest();

	return 0;
}

