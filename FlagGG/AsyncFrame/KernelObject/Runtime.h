#pragma once

#include "Export.h"

#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API Runtime
{
public:
	virtual ~Runtime() = default;

	// 主动停止
	virtual void Stop() = 0;

	// 阻塞等待停止
	virtual void WaitForStop() = 0;

	// 阻塞等待停止，最大等待时间waitTime
	virtual void WaitForStop(UInt32 waitTime) = 0;
};

}
