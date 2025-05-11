#include "MemoryConstants.h"
#include "Memory/Memory.h"
#include "Log.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace FlagGG
{

MemoryConstants::MemoryConstants()
	: totalPhysical_(0)
	, totalVirtual_(0)
	, pageSize_(0)
	, totalPhysicalGB_(1)
{
#if PLATFORM_WINDOWS
	// Gather platform memory constants.
	MEMORYSTATUSEX memoryStatusEx;
	Memory::Memzero(&memoryStatusEx, sizeof(memoryStatusEx));
	memoryStatusEx.dwLength = sizeof(memoryStatusEx);
	::GlobalMemoryStatusEx(&memoryStatusEx);

	SYSTEM_INFO systemInfo;
	Memory::Memzero(&systemInfo, sizeof(systemInfo));
	::GetSystemInfo(&systemInfo);

	totalPhysical_ = memoryStatusEx.ullTotalPhys;
	// On Windows, ullTotalVirtual is artificial and represent the SKU limitation (128TB on Win10Pro) instead of the commit limit of the system we're after.
	// ullTotalPageFile represents PhysicalMemory + Disk Swap Space, which is the value we care about
	totalVirtual_ = memoryStatusEx.ullTotalPageFile;
	allocationGranularity_ = systemInfo.dwAllocationGranularity;	// Use this so we get larger 64KiB pages, instead of 4KiB
	pageSize_ = systemInfo.dwPageSize;
	totalPhysicalGB_ = (UInt32)((totalPhysical_ + 1024 * 1024 * 1024 - 1) / 1024 / 1024 / 1024);
#else
	FLAGGG_LOG_INFO("MemoryConstants fetch not supported.");
#endif
}

const MemoryConstants& GetMemoryConstants()
{
	static MemoryConstants memoryConstants;
	return memoryConstants;
}

}
