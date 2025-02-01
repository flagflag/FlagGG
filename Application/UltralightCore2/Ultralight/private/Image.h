/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>

namespace ultralight {

class Bitmap;

class UExport Image : public RefCounted {
 public:
  static RefPtr<Image> Create();

  virtual void SetFrame(uint32_t frame_id, uint32_t frame_count, RefPtr<Bitmap> bitmap,
                        bool is_data_complete) = 0;

  virtual uint32_t frame_count() const = 0;

  virtual bool GetFrameSize(uint32_t frame_id, uint32_t& width, uint32_t& height) = 0;

  virtual void ClearFrame(uint32_t frame_id) = 0;

 protected:
  Image() = default;
  virtual ~Image() = default;
  Image(const Image&) = delete;
  void operator=(const Image&) = delete;
};

} // namespace ultralight
