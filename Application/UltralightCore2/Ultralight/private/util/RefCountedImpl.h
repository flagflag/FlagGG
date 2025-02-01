/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
// Portions of this code Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in Chromium's LICENSE file.
#pragma once
#include <Ultralight/private/util/Debug.h>
#include <mutex>
#include <atomic>

#define GENERATE_DESTRUCTOR_ERRMSG(T) "Ref-counted object '#T' was deleted without calling Release()"

namespace ultralight {

template <class T>
class RefCountedImpl {
public:
  RefCountedImpl()
    : ref_count_(1)
#ifndef NDEBUG
    , in_destructor_(false)
#endif
  {
  }

  virtual ~RefCountedImpl() {
#ifndef NDEBUG
    UL_DCHECK(in_destructor_, GENERATE_DESTRUCTOR_ERRMSG(T));
#endif
  }

  virtual void AddRef() const {
#ifndef NDEBUG
    UL_DCHECK(!in_destructor_);
#endif
    ++ref_count_;
  }

  virtual void Release() const {
#ifndef NDEBUG
    UL_DCHECK(!in_destructor_);
#endif
    if (--ref_count_ == 0) {
#ifndef NDEBUG
      in_destructor_ = true;
#endif
      delete static_cast<const T*>(this);
    }
  }

  virtual int ref_count() const {
    return ref_count_;
  }

private:
  mutable std::atomic<int> ref_count_;
#ifndef NDEBUG
  mutable std::atomic<bool> in_destructor_;
#endif
};

// A macro to disallow the copy constructor and operator= functions.
// This should be used in the private: declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

#define REF_COUNTED_IMPL(TypeName) \
  virtual void AddRef() const override { RefCountedImpl<TypeName>::AddRef(); } \
  virtual void Release() const override { RefCountedImpl<TypeName>::Release(); } \
  virtual int ref_count() const override { return RefCountedImpl<TypeName>::ref_count(); } \
  friend class RefCountedImpl<TypeName>


}  // namespace ultralight
