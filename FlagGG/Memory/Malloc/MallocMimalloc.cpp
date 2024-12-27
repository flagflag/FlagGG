#include "MallocMimalloc.h"
#include "Math/Math.h"

#include <mimalloc.h>

namespace FlagGG
{

void* MallocMimalloc::Malloc(USize size, UInt32 alignment)
{
	alignment = Max(UInt32(size) >= 16u ? 16u : 8u, alignment);
	return mi_malloc_aligned(size, alignment);
}

void* MallocMimalloc::Realloc(void* originPtr, USize newSize, UInt32 alignment)
{
	alignment = Max(UInt32(newSize) >= 16u ? 16u : 8u, alignment);
	return mi_realloc_aligned(originPtr, newSize, alignment);
}

void MallocMimalloc::Free(void* ptr)
{
	mi_free(ptr);
}

bool MallocMimalloc::GetAllocationSize(void* ptr, USize& outSize)
{
	outSize = mi_malloc_size(ptr);
	return true;
}

void MallocMimalloc::Trim(bool trimThreadCache)
{
	mi_collect(trimThreadCache);
}

}
