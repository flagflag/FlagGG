#pragma once

#include "AsyncFrame/Thread/UniqueThread.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class PerThreadFromThreadPool;
class PerThreadRunnable;

class IThreadPoolRunnable
{
public:
	virtual ~IThreadPoolRunnable() {}
	
	// 初始化
	virtual void Init(PerThreadFromThreadPool* perThread) = 0;

	// 运行
	virtual void Run(PerThreadFromThreadPool* perThread) = 0;

	// 退出
	virtual void Exit(PerThreadFromThreadPool* perThread) = 0;
};

class FlagGG_API PerThreadFromThreadPool : public UniqueThread
{
public:
	explicit PerThreadFromThreadPool(UInt32 index, IThreadPoolRunnable* runnable);

	~PerThreadFromThreadPool() override;

	// 获取在线程池里的下标
	UInt32 GetIndexInThreadPool() const { return index_; }

private:
	UInt32 index_;

	UniquePtr<PerThreadRunnable> runnable_;
};

class FlagGG_API ThreadPool : public Runtime, public RefCounted
{
public:
	ThreadPool(UInt32 numThreads, ThreadPriority threadPriority, IThreadPoolRunnable* runnable);

	~ThreadPool() override;

	// 主动停止
	void Stop() override;

	// 阻塞等待停止
	void WaitForStop() override;

	// 阻塞等待停止，最大等待时间waitTime
	void WaitForStop(UInt32 waitTime) override;

private:
	Vector<SharedPtr<PerThreadFromThreadPool>> threads_;
};

}
