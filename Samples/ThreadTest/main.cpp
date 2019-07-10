#include <AsyncFrame/Thread/SharedThread.h>
#include <Utility/SystemHelper.h>
#include <Log.h>

void ThreadFunc(uint32_t index)
{
	FLAGGG_LOG_INFO("%d", index);
}

int main()
{
	FlagGG::AsyncFrame::Thread::SharedThread thread;
	thread.Start();

	FlagGG::Utility::SystemHelper::Sleep(2000);

	for (uint32_t i = 0; i < 100; ++i)
	{
		thread.Add(std::bind(&ThreadFunc, i));
	}

	FlagGG::Utility::SystemHelper::Sleep(10000000);

	return 0;
}

