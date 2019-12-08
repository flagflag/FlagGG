#pragma once

#include "Export.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Str.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			FlagGG_API void ReadString(IOBuffer* stream, Container::String& str);

			FlagGG_API void ReadVector2(IOBuffer* stream, Math::Vector2& vec2);

			FlagGG_API void ReadVector3(IOBuffer* stream, Math::Vector3& vec3);

			FlagGG_API void ReadQuaternion(IOBuffer* stream, Math::Quaternion& quat);

			FlagGG_API void ReadMatrix3x4(IOBuffer* stream, Math::Matrix3x4& mat);

			FlagGG_API void ReadMatrix4x4(IOBuffer* stream, Math::Matrix4& mat);

			FlagGG_API void ReadBoundingBox(IOBuffer* stream, Math::BoundingBox& box);


			FlagGG_API void WriteString(IOBuffer* stream, const Container::String& str);

			FlagGG_API void WriteVector2(IOBuffer* stream, const Math::Vector2& vec2);

			FlagGG_API void WriteVector3(IOBuffer* stream, const Math::Vector3& vec3);

			FlagGG_API void WriteQuaternion(IOBuffer* stream, const Math::Quaternion& quat);

			FlagGG_API void WriteMatrix3x4(IOBuffer* stream, const Math::Matrix3x4& mat);

			FlagGG_API void WriteMatrix4x4(IOBuffer* stream, const Math::Matrix4& mat);

			FlagGG_API void WriteBoundingBox(IOBuffer* stream, const Math::BoundingBox& box);
		}
	}
}