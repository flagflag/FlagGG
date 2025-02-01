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

class BitmapImageHandler;

// Any object that can be rendered to a Canvas should inherit this.
// Painter will periodically paint and update .
class UExport Paintable {
public:
  virtual ~Paintable() {}

  // Called during Paint(). Paintables should paint to their top-level canvas here.
  //
  // @param  frame_id  The ID of the frame being rendered.
  //
  // @param  frame_time_ms  The simulation time for the frame being rendered.
  //                        This may be several milliseconds into the future.
  //
  virtual void Paint(uint64_t frame_id, double frame_time_ms) = 0;

  virtual bool needs_paint() const = 0;
};

UExport void InitPainter();
UExport void DestroyPainter();

class UExport Painter {
 public:
   // This will implicitly call InitPainter() if the instance doesn't exist.
   static Painter& instance();

   virtual ~Painter() {}

   // Attach Paintable, Paint() will call Paintable::Paint so that
   // top-level canvases can perform all painting in lock-step with the timer.
   virtual void AttachPaintable(Paintable* paintable) = 0;

   // Detach Paintable
   virtual void DetachPaintable(Paintable* paintable) = 0;

   virtual void Paint() = 0;

   virtual void PaintOnly(Paintable** paintable_array, size_t len) = 0;

   virtual void Recycle() = 0;

   // Purge memory from cache and pools (set critical to true to release all).
   virtual void PurgeMemory(bool critical) = 0;

   virtual void LogStatistics() = 0;

   virtual BitmapImageHandler* bitmap_image_handler() = 0;
};

}  // namespace ultralight
