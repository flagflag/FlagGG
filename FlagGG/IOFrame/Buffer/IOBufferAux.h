#pragma once

#include "Export.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Str.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"
#include "Math/BoundingBox.h"

namespace FlagGG { namespace IOFrame { namespace Buffer {

FlagGG_API void ReadString(IOBuffer* stream, String& str);

FlagGG_API void ReadVector2(IOBuffer* stream, Vector2& vec2);

FlagGG_API void ReadVector3(IOBuffer* stream, Vector3& vec3);

FlagGG_API void ReadQuaternion(IOBuffer* stream, Quaternion& quat);

FlagGG_API void ReadMatrix3x4(IOBuffer* stream, Matrix3x4& mat);

FlagGG_API void ReadMatrix4x4(IOBuffer* stream, Matrix4& mat);

FlagGG_API void ReadBoundingBox(IOBuffer* stream, BoundingBox& box);


FlagGG_API void WriteString(IOBuffer* stream, const String& str);

FlagGG_API void WriteVector2(IOBuffer* stream, const Vector2& vec2);

FlagGG_API void WriteVector3(IOBuffer* stream, const Vector3& vec3);

FlagGG_API void WriteVector4(IOBuffer* stream, const Vector4& vec4);

FlagGG_API void WriteQuaternion(IOBuffer* stream, const Quaternion& quat);

FlagGG_API void WriteMatrix3x4(IOBuffer* stream, const Matrix3x4& mat);

FlagGG_API void WriteMatrix4x4(IOBuffer* stream, const Matrix4& mat);

FlagGG_API void WriteBoundingBox(IOBuffer* stream, const BoundingBox& box);
		
}}}
