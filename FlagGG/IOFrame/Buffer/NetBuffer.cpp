#include "IOFrame/Buffer/NetBuffer.h"
#include "Define.h"
#include <algorithm>

namespace FlagGG { namespace IOFrame { namespace Buffer {

NetBuffer::NetBuffer()
{
	ClearIndex();
}

bool NetBuffer::CheckBuffer(int mode)
{
	if (!currentBuffer_.buffer || count_ >= (int)currentBuffer_.bufferSize)
	{
		if (index_ + 1 >= (int)buffers_.Size())
		{
			if (mode == mode_read) //读模式，不会开辟内存
			{
				return false;
			}

			currentBuffer_.buffer = new char[ONE_KB];
			currentBuffer_.bufferSize = ONE_KB;

			index_ = buffers_.Size();
			count_ = 0;

			buffers_.Push(currentBuffer_);
		}
		else
		{
			++index_;
			count_ = 0;

			currentBuffer_ = buffers_[index_];
		}
	}

	return true;
}

UInt32 NetBuffer::GetSize() const
{
	UInt32 dataSize = 0;
	for (UInt32 i = 0; i < buffers_.Size(); ++i)
	{
		dataSize += (i + 1 == buffers_.Size() ? count_ : buffers_[i].bufferSize);
	}
	return dataSize;
}

void NetBuffer::ClearIndex()
{
	index_ = -1;
	count_ = 0;
	currentBuffer_.buffer = nullptr;
	currentBuffer_.bufferSize = 0;
}

bool NetBuffer::ReadByte(UInt8& byte)
{
	if (!CheckBuffer(mode_read))
	{
		return false;
	}

	byte = (UInt8)currentBuffer_.buffer[count_];
	++count_;

	return true;
}

bool NetBuffer::WriteByte(UInt8 byte)
{
	if (!CheckBuffer(mode_write))
	{
		return false;
	}

	currentBuffer_.buffer[count_] = (char)byte;
	++count_;

	return true;
}

#define __READ__(value) \
{ \
	value = 0; \
	USize _size = sizeof(value); \
	UInt8 byte; \
	while (_size--) \
	{ \
		if (!ReadByte(byte)) \
		{ \
			break; \
		} \
		value <<= 8; \
		value |= byte; \
	} \
}

#define __WRITE__(value) \
{ \
	USize _size = sizeof(value); \
	UInt8 byte; \
	UInt8 offset = (_size - 1) * 8; \
	while (_size--) \
	{ \
		byte = (value >> offset) & 0xff; \
		offset -= 8; \
		if (!WriteByte(byte)) \
		{ \
			break; \
		} \
	} \
}

void NetBuffer::ReadInt8(Int8& value)
{
	__READ__(value);
}

void NetBuffer::WriteInt8(Int8 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadUInt8(UInt8& value)
{
	__READ__(value);
}

void NetBuffer::WriteUInt8(UInt8 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadInt16(Int16& value)
{
	__READ__(value);
}

void NetBuffer::WriteInt16(Int16 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadUInt16(UInt16& value)
{
	__READ__(value);
}

void NetBuffer::WriteUInt16(UInt16 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadInt32(Int32& value)
{
	__READ__(value);
}

void NetBuffer::WriteInt32(Int32 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadUInt32(UInt32& value)
{
	__READ__(value);
}

void NetBuffer::WriteUInt32(UInt32 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadInt64(Int64& value)
{
	__READ__(value);
}

void NetBuffer::WriteInt64(Int64 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadUInt64(UInt64& value)
{
	__READ__(value);
}

void NetBuffer::WriteUInt64(UInt64 value)
{
	__WRITE__(value);
}

void NetBuffer::ReadFloat(float& value)
{
	UInt8 byte[4];
	ReadByte(byte[0]);
	ReadByte(byte[1]);
	ReadByte(byte[2]);
	ReadByte(byte[3]);
	memcpy(&value, byte, 4);
}

void NetBuffer::WriteFloat(float value)
{
	UInt8 byte[4];
	memcpy(byte, &value, 4);
	WriteByte(byte[0]);
	WriteByte(byte[1]);
	WriteByte(byte[2]);
	WriteByte(byte[3]);
}

UInt32 NetBuffer::ReadStream(void* data, UInt32 dataSize)
{
	if (buffers_.Size() <= 0u)
		return 0u;

	auto* index = static_cast<char*>(data);
	UInt32 readSize = 0u;
	for (UInt32 i = 0; i < buffers_.Size() && readSize < dataSize; ++i)
	{
		UInt32 realSize = (i + 1 == buffers_.Size() ? count_ : buffers_[i].bufferSize);
		if (readSize + realSize > dataSize)
			realSize = dataSize - readSize;
		memcpy(index, buffers_[i].buffer, realSize);
		index += realSize;
		readSize += realSize;
	}
	return readSize;
}

UInt32 NetBuffer::WriteStream(const void* data, UInt32 dataSize)
{
	if (!data || dataSize == 0) return 0;

	const char* index = (const char*)data;
	USize leftSize = dataSize;
	while (leftSize)
	{
		CheckBuffer(mode_write);

		USize writeSize = std::min<USize>(leftSize,
			currentBuffer_.bufferSize - count_);

		memcpy(currentBuffer_.buffer, index, writeSize);
		index += writeSize;
		count_ += writeSize;

		leftSize -= writeSize;
	}
	return dataSize;
}

void NetBuffer::ToString(String& result)
{
	if (buffers_.Size() <= 0)
	{
		result.Reserve(0);
		return;
	}

	UInt32 dataSize = GetSize();
	result.Resize(dataSize);

	char* index = &result[0];
	for (UInt32 i = 0; i < buffers_.Size(); ++i)
	{
		UInt32 realSize = (i + 1 == buffers_.Size() ? count_ : buffers_[i].bufferSize);
		memcpy(index, buffers_[i].buffer, realSize);
		index += realSize;
	}
}

void NetBuffer::ToBuffer(SharedArrayPtr<char>& buffer, UInt32& bufferSize)
{
	if (buffers_.Size() <= 0)
	{
		buffer = nullptr;
		bufferSize = 0u;
		return;
	}

	bufferSize = GetSize();
	buffer = new char[bufferSize];

	char* index = buffer.Get();
	for (UInt32 i = 0; i < buffers_.Size(); ++i)
	{
		UInt32 realSize = (i + 1 == buffers_.Size() ? count_ : buffers_[i].bufferSize);
		memcpy(index, buffers_[i].buffer, realSize);
		index += realSize;
	}
}

}}}
