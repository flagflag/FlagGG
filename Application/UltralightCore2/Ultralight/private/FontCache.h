/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/private/Cache.h>
#include <Ultralight/private/Font.h>
#include <map>

namespace ultralight {

class UExport FontCache : public Cache {
public:
  ~FontCache();

  static FontCache* instance();

  virtual RefPtr<Font> GetFont(void* platform_data, uint64_t font_file_hash, float font_size);

  // inherited from Cache

  virtual uint64_t bytes_allocated() override;

  virtual uint64_t bytes_used() override;

  // remove entries that haven't been used in a while and reorganize
  // cache so more entries can be added in the newly available space
  virtual void Recycle() override;

  // reclaim as much RAM as possible (system has signaled out-of-memory)
  virtual void PurgeMemory() override;

protected:
  FontCache();

  typedef std::map<uint32_t, RefPtr<Font>> FontMap;
  FontMap font_map_;

  friend class CacheManager;
};

}  // namespace ultralight
