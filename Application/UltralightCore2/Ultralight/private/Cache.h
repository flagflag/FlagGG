/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>

namespace ultralight {

class UExport Cache {
public:
  virtual ~Cache() { }

  virtual uint64_t bytes_allocated() = 0;
  virtual uint64_t bytes_used() = 0;

  // remove entries that haven't been used in a while and reorganize
  // cache so more entries can be added in the newly available space
  virtual void Recycle() = 0;

  // reclaim as much RAM as possible (system has signaled out-of-memory)
  virtual void PurgeMemory() = 0;
};

}  // namespace ultralight

