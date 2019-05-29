#pragma once

#include "Export.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Str.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			FlagGG_API void ReadString(IOBuffer* stream, Container::String& str);

			FlagGG_API void ReadVector3(IOBuffer* stream, Math::Vector3& vec3);

			FlagGG_API void ReadQuaternion(IOBuffer* stream, Math::Quaternion& quat);

			FlagGG_API void ReadMatrix3x4(IOBuffer* stream, Math::Matrix3x4& mat);
		}
	}
}