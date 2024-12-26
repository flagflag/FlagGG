#pragma once

#include "Export.h"

#include "AsyncFrame/ConditionQueue.hpp"
#include "AsyncFrame/Thread/UniqueThread.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "Core/BaseTypes.h"

#include <functional>
#include <atomic>

namespace FlagGG
{

typedef std::function<void(void)> ThreadTask;

class FlagGG_API SharedThread : public Runtime, public RefCounted
{
public:
	SharedThread();

	~SharedThread() override;

	void Start();

	void Stop() override;

	void WaitForStop() override;

	void WaitForStop(UInt32 wait_time) override;

	void Add(ThreadTask taskFunc);

	void ForceStop();

	UInt32 WaitingTime();

protected:
	void WorkThread();

private:
	ConditionQueue<ThreadTask>	taskQueue_;

	UniqueThreadPtr				thread_;

	std::atomic<bool>			running_;
};

typedef SharedPtr<SharedThread> SharedThreadPtr;

}
