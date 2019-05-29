#include "IOBufferAux.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			void ReadString(IOBuffer* stream, Container::String& str)
			{
				uint8_t ch;
				while (!stream->IsEof())
				{
					stream->ReadUInt8(ch);

					if (!ch)
					{
						break;
					}

					str += static_cast<char>(ch);
				}
			}

			void ReadVector3(IOBuffer* stream, Math::Vector3& vec3)
			{
				stream->ReadStream(&vec3.x_, sizeof(Math::Vector3));
			}

			void ReadQuaternion(IOBuffer* stream, Math::Quaternion& quat)
			{
				stream->ReadStream(&quat.w_, sizeof(Math::Quaternion));
			}

			void ReadMatrix3x4(IOBuffer* stream, Math::Matrix3x4& mat)
			{
				stream->ReadStream(&mat.m00_, sizeof(Math::Matrix3x4));
			}
		}
	}
}