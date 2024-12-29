//
// 系统内存的一些常量信息
//

#pragma once

#include "Core/BaseTypes.h"
#include "Core/BaseMacro.h"
#include "Memory/Memory.h"

namespace FlagGG
{

struct FlagGG_API MemoryConstants : public UseSystemMalloc
{
	// 物理内存大小（字节）
	UInt64 totalPhysical_;

	// 虚拟内存大小（字节）
	UInt64 totalVirtual_;

	// 物理内存页大小（字节）
	USize pageSize_;

	// 虚拟内存分配粒度（不能小于这个值）
	USize allocationGranularity_;

	// 物理内存大小（GB）
	UInt32 totalPhysicalGB_;

	MemoryConstants();
};

FlagGG_API const MemoryConstants& GetMemoryConstants();

}
