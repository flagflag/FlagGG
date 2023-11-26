#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Core/BaseTypes.h"
#include "Math/Color.h"
#include "Math/Rect.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

namespace FlagGG
{

FlagGG_API String ToString(const char* format, ...);

FlagGG_API bool IsInterger(const String& content);

// 基础类型：

FlagGG_API bool ToBool(const String& source);

FlagGG_API bool ToBool(const char* source);

FlagGG_API Int32 ToInt(const String& source, Int32 base = 10);

FlagGG_API Int32 ToInt(const char* source, Int32 base = 10);

FlagGG_API UInt32 ToUInt(const String& source, Int32 base = 10);

FlagGG_API UInt32 ToUInt(const char* source, Int32 base = 10);

FlagGG_API Int64 ToInt64(const String& source, Int32 base = 10);

FlagGG_API Int64 ToInt64(const char* source, Int32 base = 10);

FlagGG_API UInt64 ToUInt64(const String& source, Int32 base = 10);

FlagGG_API UInt64 ToUInt64(const char* source, Int32 base = 10);

FlagGG_API Real ToFloat(const String& source);

FlagGG_API Real ToFloat(const char* source);

FlagGG_API RealD ToDouble(const String& source);

FlagGG_API RealD ToDouble(const char* source);

// 结构体类型：

FlagGG_API Color ToColor(const String& source);

FlagGG_API Color ToColor(const char* source);

FlagGG_API IntRect ToIntRect(const String& source);

FlagGG_API IntRect ToIntRect(const char* source);

FlagGG_API IntVector2 ToIntVector2(const String& source);

FlagGG_API IntVector2 ToIntVector2(const char* source);

FlagGG_API IntVector3 ToIntVector3(const String& source);

FlagGG_API Rect ToRect(const String& source);

FlagGG_API Rect ToRect(const char* source);

FlagGG_API Quaternion ToQuaternion(const String& source);

FlagGG_API Quaternion ToQuaternion(const char* source);

FlagGG_API Vector2 ToVector2(const String& source);

FlagGG_API Vector2 ToVector2(const char* source);

FlagGG_API Vector3 ToVector3(const String& source);

FlagGG_API Vector3 ToVector3(const char* source);

FlagGG_API Vector4 ToVector4(const String& source, bool allowMissingCoords = false);

FlagGG_API Vector4 ToVector4(const char* source, bool allowMissingCoords = false);

FlagGG_API Matrix3 ToMatrix3(const String& source);

FlagGG_API Matrix3 ToMatrix3(const char* source);

FlagGG_API Matrix3x4 ToMatrix3x4(const String& source);

FlagGG_API Matrix4 ToMatrix4(const String& source);

FlagGG_API Matrix4 ToMatrix4(const char* source);

FlagGG_API void BufferToString(String& dest, const void* data, UInt32 size);

FlagGG_API UInt32 GetStringListIndex(const String& value, const String* strings, UInt32 defaultIndex, bool caseSensitive = false);
FlagGG_API UInt32 GetStringListIndex(const char* value, const String* strings, UInt32 defaultIndex, bool caseSensitive = false);
FlagGG_API UInt32 GetStringListIndex(const char* value, const char** strings, UInt32 defaultIndex, bool caseSensitive = false);

}

