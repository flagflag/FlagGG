/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <functional>
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Bitmap.h>

namespace ultralight {

class UExport VideoFrame : public RefCounted {
 public:
  static RefPtr<VideoFrame> Create(RefPtr<Bitmap> bitmap, std::function<void()> on_destroy);

  virtual RefPtr<Bitmap> bitmap() = 0;

 protected:
  VideoFrame() = default;
  virtual ~VideoFrame() = default;
  VideoFrame(const VideoFrame&) = delete;
  void operator=(const VideoFrame&) = delete;
};

} // namespace ultralight
