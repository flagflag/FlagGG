#include "Format.h"

#include <stdarg.h>

namespace FlagGG
{

String ToString(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	USize len = vsnprintf(nullptr, 0, format, va);

	String buffer;
	buffer.Resize(len);

	vsnprintf(&buffer[0], len + 1, format, va);
	va_end(va);

	return buffer;
}

bool IsInterger(const String& content)
{
	for (UInt32 i = 0; i < content.Length(); ++i)
	{
		if (content[i] < '0' || content[i] > '9')
		{
			return false;
		}
	}
	return true;
}

bool ToBool(const String& source)
{
	return ToBool(source.CString());
}

bool ToBool(const char* source)
{
	unsigned length = String::CStringLength(source);

	for (unsigned i = 0; i < length; ++i)
	{
		auto c = (char)tolower(source[i]);
		if (c == 't' || c == 'y' || c == '1')
			return true;
		else if (c != ' ' && c != '\t')
			break;
	}

	return false;
}

Int32 ToInt(const String& source, Int32 base)
{
	return ToInt(source.CString(), base);
}

Int32 ToInt(const char* source, Int32 base)
{
	if (!source)
		return 0;

	// Shield against runtime library assert by converting illegal base values to 0 (autodetect)
	if (base < 2 || base > 36)
		base = 0;

	return (Int32)strtol(source, nullptr, base);
}

UInt32 ToUInt(const String& source, Int32 base)
{
	return ToUInt(source.CString(), base);
}

UInt32 ToUInt(const char* source, Int32 base)
{
	if (!source)
		return 0;

	// Shield against runtime library assert by converting illegal base values to 0 (autodetect)
	if (base < 2 || base > 36)
		base = 0;

	return (UInt32)strtoull(source, nullptr, base);
}

Int64 ToInt64(const String& source, Int32 base)
{
	return ToInt64(source.CString(), base);
}

Int64 ToInt64(const char* source, Int32 base)
{
	if (!source)
		return 0;

	// Shield against runtime library assert by converting illegal base values to 0 (autodetect)
	if (base < 2 || base > 36)
		base = 0;

	return (Int64)strtoll(source, nullptr, base);
}

UInt64 ToUInt64(const String& source, Int32 base)
{
	return ToUInt64(source.CString(), base);
}

UInt64 ToUInt64(const char* source, Int32 base)
{
	if (!source)
		return 0;

	// Shield against runtime library assert by converting illegal base values to 0 (autodetect)
	if (base < 2 || base > 36)
		base = 0;

	return (UInt64)strtoull(source, nullptr, base);
}

Real ToFloat(const String& source)
{
	return ToFloat(source.CString());
}

Real ToFloat(const char* source)
{
	if (!source)
		return 0;

	return (Real)strtod(source, nullptr);
}

RealD ToDouble(const String& source)
{
	return ToDouble(source.CString());
}

RealD ToDouble(const char* source)
{
	if (!source)
		return 0;

	return strtod(source, nullptr);
}

static UInt32 CountElements(const char* buffer, char separator)
{
	if (!buffer)
		return 0;

	const char* endPos = buffer + String::CStringLength(buffer);
	const char* pos = buffer;
	UInt32 ret = 0;

	while (pos < endPos)
	{
		if (*pos != separator)
			break;
		++pos;
	}

	while (pos < endPos)
	{
		const char* start = pos;

		while (start < endPos)
		{
			if (*start == separator)
				break;

			++start;
		}

		if (start == endPos)
		{
			++ret;
			break;
		}

		const char* end = start;

		while (end < endPos)
		{
			if (*end != separator)
				break;

			++end;
		}

		++ret;
		pos = end;
	}

	return ret;
}

Color ToColor(const String& source)
{
	return ToColor(source.CString());
}

Color ToColor(const char* source)
{
	Color ret;

	UInt32 elements = CountElements(source, ' ');
	if (elements < 3)
		return ret;

	auto* ptr = (char*)source;
	ret.r_ = (float)strtod(ptr, &ptr);
	ret.g_ = (float)strtod(ptr, &ptr);
	ret.b_ = (float)strtod(ptr, &ptr);
	if (elements > 3)
		ret.a_ = (float)strtod(ptr, &ptr);

	return ret;
}

IntRect ToIntRect(const String& source)
{
	return ToIntRect(source.CString());
}

IntRect ToIntRect(const char* source)
{
	IntRect ret(IntRect::ZERO);

	UInt32 elements = CountElements(source, ' ');
	if (elements < 4)
		return ret;

	auto* ptr = (char*)source;
	ret.left_ = (int)strtol(ptr, &ptr, 10);
	ret.top_ = (int)strtol(ptr, &ptr, 10);
	ret.right_ = (int)strtol(ptr, &ptr, 10);
	ret.bottom_ = (int)strtol(ptr, &ptr, 10);

	return ret;
}

IntVector2 ToIntVector2(const String& source)
{
	return ToIntVector2(source.CString());
}

IntVector2 ToIntVector2(const char* source)
{
	IntVector2 ret(IntVector2::ZERO);

	UInt32 elements = CountElements(source, ' ');
	if (elements < 2)
		return ret;

	auto* ptr = (char*)source;
	ret.x_ = (int)strtol(ptr, &ptr, 10);
	ret.y_ = (int)strtol(ptr, &ptr, 10);

	return ret;
}

IntVector3 ToIntVector3(const String& source)
{
	return ToIntVector3(source.CString());
}

IntVector3 ToIntVector3(const char* source)
{
	IntVector3 ret(IntVector3::ZERO);

	UInt32 elements = CountElements(source, ' ');
	if (elements < 3)
		return ret;

	auto* ptr = (char*)source;
	ret.x_ = (int)strtol(ptr, &ptr, 10);
	ret.y_ = (int)strtol(ptr, &ptr, 10);
	ret.z_ = (int)strtol(ptr, &ptr, 10);

	return ret;
}

Rect ToRect(const String& source)
{
	return ToRect(source.CString());
}

Rect ToRect(const char* source)
{
	Rect ret(Rect::ZERO);

	UInt32 elements = CountElements(source, ' ');
	if (elements < 4)
		return ret;

	auto* ptr = (char*)source;
	ret.min_.x_ = (float)strtod(ptr, &ptr);
	ret.min_.y_ = (float)strtod(ptr, &ptr);
	ret.max_.x_ = (float)strtod(ptr, &ptr);
	ret.max_.y_ = (float)strtod(ptr, &ptr);

	return ret;
}

Quaternion ToQuaternion(const String& source)
{
	return ToQuaternion(source.CString());
}

Quaternion ToQuaternion(const char* source)
{
	UInt32 elements = CountElements(source, ' ');
	auto* ptr = (char*)source;

	if (elements < 3)
		return Quaternion::IDENTITY;
	else if (elements < 4)
	{
		// 3 coords specified: conversion from Euler angles
		float x, y, z;
		x = (float)strtod(ptr, &ptr);
		y = (float)strtod(ptr, &ptr);
		z = (float)strtod(ptr, &ptr);

		return Quaternion(x, y, z);
	}
	else
	{
		// 4 coords specified: full quaternion
		Quaternion ret;
		ret.w_ = (float)strtod(ptr, &ptr);
		ret.x_ = (float)strtod(ptr, &ptr);
		ret.y_ = (float)strtod(ptr, &ptr);
		ret.z_ = (float)strtod(ptr, &ptr);

		return ret;
	}
}

Vector2 ToVector2(const String& source)
{
	return ToVector2(source.CString());
}

Vector2 ToVector2(const char* source)
{
	Vector2 ret(Vector2::ZERO);

	UInt32 elements = CountElements(source, ' ');
	if (elements < 2)
		return ret;

	auto* ptr = (char*)source;
	ret.x_ = (float)strtod(ptr, &ptr);
	ret.y_ = (float)strtod(ptr, &ptr);

	return ret;
}

Vector3 ToVector3(const String& source)
{
	return ToVector3(source.CString());
}

Vector3 ToVector3(const char* source)
{
	Vector3 ret(Vector3::ZERO);

	UInt32 elements = CountElements(source, ' ');
	if (elements < 3)
		return ret;

	auto* ptr = (char*)source;
	ret.x_ = (float)strtod(ptr, &ptr);
	ret.y_ = (float)strtod(ptr, &ptr);
	ret.z_ = (float)strtod(ptr, &ptr);

	return ret;
}

Vector4 ToVector4(const String& source, bool allowMissingCoords)
{
	return ToVector4(source.CString(), allowMissingCoords);
}

Vector4 ToVector4(const char* source, bool allowMissingCoords)
{
	Vector4 ret(Vector4::ZERO);

	unsigned elements = CountElements(source, ' ');
	auto* ptr = (char*)source;

	if (!allowMissingCoords)
	{
		if (elements < 4)
			return ret;

		ret.x_ = (float)strtod(ptr, &ptr);
		ret.y_ = (float)strtod(ptr, &ptr);
		ret.z_ = (float)strtod(ptr, &ptr);
		ret.w_ = (float)strtod(ptr, &ptr);

		return ret;
	}
	else
	{
		if (elements > 0)
			ret.x_ = (float)strtod(ptr, &ptr);
		if (elements > 1)
			ret.y_ = (float)strtod(ptr, &ptr);
		if (elements > 2)
			ret.z_ = (float)strtod(ptr, &ptr);
		if (elements > 3)
			ret.w_ = (float)strtod(ptr, &ptr);

		return ret;
	}
}

Matrix3 ToMatrix3(const String& source)
{
	return ToMatrix3(source.CString());
}

Matrix3 ToMatrix3(const char* source)
{
	Matrix3 ret(Matrix3::ZERO);

	unsigned elements = CountElements(source, ' ');
	if (elements < 9)
		return ret;

	auto* ptr = (char*)source;
	ret.m00_ = (float)strtod(ptr, &ptr);
	ret.m01_ = (float)strtod(ptr, &ptr);
	ret.m02_ = (float)strtod(ptr, &ptr);
	ret.m10_ = (float)strtod(ptr, &ptr);
	ret.m11_ = (float)strtod(ptr, &ptr);
	ret.m12_ = (float)strtod(ptr, &ptr);
	ret.m20_ = (float)strtod(ptr, &ptr);
	ret.m21_ = (float)strtod(ptr, &ptr);
	ret.m22_ = (float)strtod(ptr, &ptr);

	return ret;
}

Matrix3x4 ToMatrix3x4(const String& source)
{
	return ToMatrix3x4(source.CString());
}

Matrix3x4 ToMatrix3x4(const char* source)
{
	Matrix3x4 ret(Matrix3x4::ZERO);

	unsigned elements = CountElements(source, ' ');
	if (elements < 12)
		return ret;

	auto* ptr = (char*)source;
	ret.m00_ = (float)strtod(ptr, &ptr);
	ret.m01_ = (float)strtod(ptr, &ptr);
	ret.m02_ = (float)strtod(ptr, &ptr);
	ret.m03_ = (float)strtod(ptr, &ptr);
	ret.m10_ = (float)strtod(ptr, &ptr);
	ret.m11_ = (float)strtod(ptr, &ptr);
	ret.m12_ = (float)strtod(ptr, &ptr);
	ret.m13_ = (float)strtod(ptr, &ptr);
	ret.m20_ = (float)strtod(ptr, &ptr);
	ret.m21_ = (float)strtod(ptr, &ptr);
	ret.m22_ = (float)strtod(ptr, &ptr);
	ret.m23_ = (float)strtod(ptr, &ptr);

	return ret;
}

Matrix4 ToMatrix4(const String& source)
{
	return ToMatrix4(source.CString());
}

Matrix4 ToMatrix4(const char* source)
{
	Matrix4 ret(Matrix4::ZERO);

	unsigned elements = CountElements(source, ' ');
	if (elements < 16)
		return ret;

	auto* ptr = (char*)source;
	ret.m00_ = (float)strtod(ptr, &ptr);
	ret.m01_ = (float)strtod(ptr, &ptr);
	ret.m02_ = (float)strtod(ptr, &ptr);
	ret.m03_ = (float)strtod(ptr, &ptr);
	ret.m10_ = (float)strtod(ptr, &ptr);
	ret.m11_ = (float)strtod(ptr, &ptr);
	ret.m12_ = (float)strtod(ptr, &ptr);
	ret.m13_ = (float)strtod(ptr, &ptr);
	ret.m20_ = (float)strtod(ptr, &ptr);
	ret.m21_ = (float)strtod(ptr, &ptr);
	ret.m22_ = (float)strtod(ptr, &ptr);
	ret.m23_ = (float)strtod(ptr, &ptr);
	ret.m30_ = (float)strtod(ptr, &ptr);
	ret.m31_ = (float)strtod(ptr, &ptr);
	ret.m32_ = (float)strtod(ptr, &ptr);
	ret.m33_ = (float)strtod(ptr, &ptr);

	return ret;
}

void BufferToString(String& dest, const void* data, UInt32 size)
{
	// Precalculate needed string size
	const auto* bytes = (const unsigned char*)data;
	UInt32 length = 0;
	for (UInt32 i = 0; i < size; ++i)
	{
		// Room for separator
		if (i)
			++length;

		// Room for the value
		if (bytes[i] < 10)
			++length;
		else if (bytes[i] < 100)
			length += 2;
		else
			length += 3;
	}

	dest.Resize(length);
	UInt32 index = 0;

	// Convert values
	for (UInt32 i = 0; i < size; ++i)
	{
		if (i)
			dest[index++] = ' ';

		if (bytes[i] < 10)
		{
			dest[index++] = '0' + bytes[i];
		}
		else if (bytes[i] < 100)
		{
			dest[index++] = (char)('0' + bytes[i] / 10);
			dest[index++] = (char)('0' + bytes[i] % 10);
		}
		else
		{
			dest[index++] = (char)('0' + bytes[i] / 100);
			dest[index++] = (char)('0' + bytes[i] % 100 / 10);
			dest[index++] = (char)('0' + bytes[i] % 10);
		}
	}
}

UInt32 GetStringListIndex(const String& value, const String* strings, UInt32 defaultIndex, bool caseSensitive)
{
	return GetStringListIndex(value.CString(), strings, defaultIndex, caseSensitive);
}

UInt32 GetStringListIndex(const char* value, const String* strings, UInt32 defaultIndex, bool caseSensitive)
{
	UInt32 i = 0;

	while (!strings[i].Empty())
	{
		if (!strings[i].Compare(value, caseSensitive))
			return i;
		++i;
	}

	return defaultIndex;
}

UInt32 GetStringListIndex(const char* value, const char** strings, UInt32 defaultIndex, bool caseSensitive)
{
	UInt32 i = 0;

	while (strings[i])
	{
		if (!String::Compare(value, strings[i], caseSensitive))
			return i;
		++i;
	}

	return defaultIndex;
}

}