/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once

#include <Ultralight/Defines.h>

enum class UExport MemoryTag : uint8_t {
  App,
  Asset,
  Bitmap,
  Cache,
  Font,
  GPU,
  JavaScript_Bytecode,
  JavaScript,
  Network,
  Renderer,
  Resource,
  Uncategorized,
  WebCore,
  Count
};

UExport const char* MemoryTagToString(MemoryTag tag);

namespace MemoryStats {

UExport void UpdateAllocatedBytes(MemoryTag tag, int64_t deltaBytes);

UExport void UpdateAllocationCount(MemoryTag tag, int64_t deltaCount);

UExport int64_t GetAllocatedBytes(MemoryTag tag);

UExport int64_t GetAllocationCount(MemoryTag tag);

} // namespace MemoryStats
