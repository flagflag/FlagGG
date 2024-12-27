#include <AsyncFrame/Thread/SharedThread.h>
#include <AsyncFrame/Thread/ThreadPool.h>
#include <AsyncFrame/LockFree/ThreadDeque.h>
#include <Container/Vector.h>
#include <Utility/SystemHelper.h>
#include <Log.h>
#include <Memory/MemoryHook.h>

using namespace FlagGG;

static UInt32 count = 0;
void ThreadFunc(Int32 index)
{
	++count;
	FLAGGG_LOG_INFO("{} {}", count, index);
}

void OneThreadTest()
{
	SharedThread thread;
	thread.Start();

	for (UInt32 i = 0; i < 1e5; ++i)
	{
		thread.Add(std::bind(&ThreadFunc, i));
	}

	FlagGG::Sleep(10000000);
}

void MultiThreadTest()
{
	SharedThread thread;

	thread.Start();

	UniqueThreadPtr workThread[4];
	for (UInt32 i = 0; i < 4; ++i)
	{
		workThread[i] = new UniqueThread([&]
		{
			for (UInt32 j = 0; j < 1e5; ++j)
			{
				thread.Add(std::bind(&ThreadFunc, i * j));
			}
		});
	}

	FlagGG::Sleep(10000000);
}

static ThreadSafeDeque<int> g_queue;
Vector<int> g_ret;
void LockFreeQueueTest()
{
	UniqueThread thread([&]
	{
		while (true)
		{
			while (!g_queue.IsEmpty())
			{
				int value = 0;
				if (g_queue.TryPopFront(value))
				{
					g_ret.Push(value);
				}
			}
		}
	});

	for (UInt32 i = 0; i < 1e5; ++i)
	{
		g_queue.PushBack(i);
	}

	FlagGG::Sleep(2000);

	for (UInt32 i = 0; i < 1e5; ++i)
	{
		FLAGGG_LOG_INFO("{}", g_ret[i]);
	}
}

int main()
{
	freopen("E:\\test.txt", "w", stdout);
	// OneThreadTest();
	// MultiThreadTest();
	LockFreeQueueTest();

	return 0;
}

IMPLEMENT_MODULE_USD("ThreadTest");
