#include "Ultralight/private/util/MemoryTag.h"

static const char* MEMORY_TAG_NAME[uint32_t(MemoryTag::Count)] =
{
	"App",
	"Asset",
	"Bitmap",
	"Cache",
	"Font",
	"GPU",
	"JavaScript_Bytecode",
	"JavaScript",
	"Network",
	"Renderer",
	"Resource",
	"Uncategorized",
	"WebCore",
};

UExport const char* MemoryTagToString(MemoryTag tag)
{
	return MEMORY_TAG_NAME[uint32_t(tag)];
}

namespace MemoryStats
{

UExport void UpdateAllocatedBytes(MemoryTag tag, int64_t deltaBytes)
{

}

UExport void UpdateAllocationCount(MemoryTag tag, int64_t deltaCount)
{

}

UExport int64_t GetAllocatedBytes(MemoryTag tag)
{
	return 0u;
}

UExport int64_t GetAllocationCount(MemoryTag tag)
{
	return 0u;
}

}
