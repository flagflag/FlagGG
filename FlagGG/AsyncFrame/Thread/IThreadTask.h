//
// 线程任务基类
//

#pragma once

namespace FlagGG
{

enum class ThreadTaskPriority
{
	Highest,
	High,
	Normal,
	Low,
	Lowest,
	Max,
};

class IThreadTask
{
public:
	virtual ~IThreadTask() {}

	virtual void Run() = 0;
};

class IAbandonableThreadTask : public IThreadTask
{
public:
	virtual void Abandon() = 0;
};

}
