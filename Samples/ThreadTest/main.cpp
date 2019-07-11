#include <AsyncFrame/Thread/SharedThread.h>
#include <AsyncFrame/Thread/ThreadPool.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

static uint32_t count = 0;
void ThreadFunc(int32_t index)
{
	++count;
	FLAGGG_LOG_INFO("%d %d", count, index);
}

void OneThreadTest()
{
	FlagGG::AsyncFrame::Thread::SharedThread thread;
	thread.Start();

	FlagGG::Utility::SystemHelper::Sleep(2000);

	for (uint32_t i = 0; i < 1e5; ++i)
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
	for (uint32_t i = 0; i < 4; ++i)
	{
		workThread[i] = new FlagGG::AsyncFrame::Thread::UniqueThread([&]
		{
			for (uint32_t j = 0; j < 1e5; ++j)
			{
				thread.Add(std::bind(&ThreadFunc, i * j));
			}
		});
	}

	FlagGG::Utility::SystemHelper::Sleep(10000000);
}

int main()
{
	// OneThreadTest();
	MultiThreadTest();

	return 0;
}

