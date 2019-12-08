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

			void ReadVector2(IOBuffer* stream, Math::Vector2& vec2)
			{
				stream->ReadStream(&vec2.x_, sizeof(Math::Vector2));
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

			void ReadMatrix4x4(IOBuffer* stream, Math::Matrix4& mat)
			{
				stream->ReadStream(&mat.m00_, sizeof(Math::Matrix4));
			}

			void ReadBoundingBox(IOBuffer* stream, Math::BoundingBox& box)
			{
				ReadVector3(stream, box.min_);
				ReadVector3(stream, box.max_);
			}

			void WriteString(IOBuffer* stream, const Container::String& str)
			{
				stream->WriteStream(str.CString(), str.Length());
				stream->WriteUInt8(0u);
			}

			void WriteVector2(IOBuffer* stream, const Math::Vector2& vec2)
			{
				stream->WriteStream(&vec2.x_, sizeof(Math::Vector2));
			}

			void WriteVector3(IOBuffer* stream, const Math::Vector3& vec3)
			{
				stream->WriteStream(&vec3.x_, sizeof(Math::Vector3));
			}

			void WriteQuaternion(IOBuffer* stream, const Math::Quaternion& quat)
			{
				stream->WriteStream(&quat.w_, sizeof(Math::Quaternion));
			}

			void WriteMatrix3x4(IOBuffer* stream, const Math::Matrix3x4& mat)
			{
				stream->WriteStream(&mat.m00_, sizeof(Math::Matrix3x4));
			}

			void WriteMatrix4x4(IOBuffer* stream, const Math::Matrix4& mat)
			{
				stream->WriteStream(&mat.m00_, sizeof(Math::Matrix4));
			}

			void WriteBoundingBox(IOBuffer* stream, const Math::BoundingBox& box)
			{
				WriteVector3(stream, box.min_);
				WriteVector3(stream, box.max_);
			}
		}
	}
}