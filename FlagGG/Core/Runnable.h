//
// Runnable interface
//

#pragma once

namespace FlagGG
{

class IRunnable
{
public:
	virtual ~IRunnable() {}

	// 初始化
	virtual void Init() = 0;

	// 运行
	virtual void Run() = 0;

	// 停止
	virtual void Stop() = 0;

	// 退出
	virtual void Exit() = 0;
};

}
