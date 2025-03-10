#include "IOBufferAux.h"

namespace FlagGG { namespace IOFrame { namespace Buffer {

void ReadString(IOBuffer* stream, String& str)
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

void ReadVector2(IOBuffer* stream, Vector2& vec2)
{
	stream->ReadStream(&vec2.x_, sizeof(Vector2));
}

void ReadVector3(IOBuffer* stream, Vector3& vec3)
{
	stream->ReadStream(&vec3.x_, sizeof(Vector3));
}

void ReadQuaternion(IOBuffer* stream, Quaternion& quat)
{
	stream->ReadStream(&quat.w_, sizeof(Quaternion));
}

void ReadMatrix3x4(IOBuffer* stream, Matrix3x4& mat)
{
	stream->ReadStream(&mat.m00_, sizeof(Matrix3x4));
}

void ReadMatrix4x4(IOBuffer* stream, Matrix4& mat)
{
	stream->ReadStream(&mat.m00_, sizeof(Matrix4));
}

void ReadBoundingBox(IOBuffer* stream, BoundingBox& box)
{
	ReadVector3(stream, box.min_);
	ReadVector3(stream, box.max_);
}

void WriteString(IOBuffer* stream, const String& str)
{
	stream->WriteStream(str.CString(), str.Length());
	stream->WriteUInt8(0u);
}

void WriteVector2(IOBuffer* stream, const Vector2& vec2)
{
	stream->WriteStream(&vec2.x_, sizeof(Vector2));
}

void WriteVector3(IOBuffer* stream, const Vector3& vec3)
{
	stream->WriteStream(&vec3.x_, sizeof(Vector3));
}

void WriteVector4(IOBuffer* stream, const Vector4& vec4)
{
	stream->WriteStream(&vec4.x_, sizeof(Vector4));
}

void WriteQuaternion(IOBuffer* stream, const Quaternion& quat)
{
	stream->WriteStream(&quat.w_, sizeof(Quaternion));
}

void WriteMatrix3x4(IOBuffer* stream, const Matrix3x4& mat)
{
	stream->WriteStream(&mat.m00_, sizeof(Matrix3x4));
}

void WriteMatrix4x4(IOBuffer* stream, const Matrix4& mat)
{
	stream->WriteStream(&mat.m00_, sizeof(Matrix4));
}

void WriteBoundingBox(IOBuffer* stream, const BoundingBox& box)
{
	WriteVector3(stream, box.min_);
	WriteVector3(stream, box.max_);
}
		
}}}