/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#ifdef __cplusplus
#define YG_EXTERN_C_BEGIN extern "C" {
#define YG_EXTERN_C_END }
#else
#define YG_EXTERN_C_BEGIN
#define YG_EXTERN_C_END
#endif

// BEGIN_UNITY @jonathanma DisableAssertRelease
#if UNITY_RELEASE
#define YGAssert(a, b)
#define YGAssertWithNode(a, b, c)
#define YGAssertWithConfig(a, b, c)
#endif
// END_UNITY

// BEGIN_UNITY @joce 11-01-2016 DontExportSymbols
//#ifdef _WINDLL
#if defined(_WINDLL) && !defined(UNITY)
// END_UNITY
#define WIN_EXPORT __declspec(dllexport)
#else
#define WIN_EXPORT
#endif

#ifdef WINARMDLL
#define WIN_STRUCT(type) type *
#define WIN_STRUCT_REF(value) &value
#else
#define WIN_STRUCT(type) type
#define WIN_STRUCT_REF(value) value
#endif

#ifndef FB_ASSERTIONS_ENABLED
#define FB_ASSERTIONS_ENABLED 1
#endif

#ifdef NS_ENUM
// Cannot use NSInteger as NSInteger has a different size than int (which is the default type of a
// enum).
// Therefor when linking the Yoga C library into obj-c the header is a missmatch for the Yoga ABI.
#define YG_ENUM_BEGIN(name) NS_ENUM(int, name)
#define YG_ENUM_END(name)
#else
#define YG_ENUM_BEGIN(name) enum name
#define YG_ENUM_END(name) name
#endif
