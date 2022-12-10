#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "Export.h"

#include "SharedThread.h"
#include "Container/Vector.h"

namespace FlagGG
{

class FlagGG_API ThreadPool : public Runtime, public RefCounted
{
public:
	ThreadPool(UInt32 threadCount);

	~ThreadPool() override = default;

	void Add(ThreadTask task_func);

	void Start();

	void Stop() override;

	void WaitForStop() override;

	void WaitForStop(UInt32 wait_time) override;

private:
	Vector<SharedThreadPtr> threads_;
};

}

#endif