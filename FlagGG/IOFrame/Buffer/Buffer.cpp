#include "Buffer.h"
#include "Define.h"
#include <algorithm>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
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

			void NetBuffer::ClearIndex()
			{
				index_ = -1;
				count_ = 0;
				currentBuffer_.buffer = nullptr;
				currentBuffer_.bufferSize = 0;
			}

			bool NetBuffer::ReadByte(uint8_t& byte)
			{
				if (!CheckBuffer(mode_read))
				{
					return false;
				}

				byte = (uint8_t)currentBuffer_.buffer[count_];
				++count_;

				return true;
			}

			bool NetBuffer::WriteByte(uint8_t byte)
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
				size_t _size = sizeof(value); \
				uint8_t byte; \
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
				size_t _size = sizeof(value); \
				uint8_t byte; \
				uint8_t offset = (_size - 1) * 8; \
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

			void NetBuffer::ReadInt8(int8_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt8(int8_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt8(uint8_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt8(uint8_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadInt16(int16_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt16(int16_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt16(uint16_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt16(uint16_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadInt32(int32_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt32(int32_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt32(uint32_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt32(uint32_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadInt64(int64_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt64(int64_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt64(uint64_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt64(uint64_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadFloat(float& value)
			{
				uint8_t byte[4];
				ReadByte(byte[0]);
				ReadByte(byte[1]);
				ReadByte(byte[2]);
				ReadByte(byte[3]);
				memcpy(&value, byte, 4);
			}

			void NetBuffer::WriteFloat(float value)
			{
				uint8_t byte[4];
				memcpy(byte, &value, 4);
				WriteByte(byte[0]);
				WriteByte(byte[1]);
				WriteByte(byte[2]);
				WriteByte(byte[3]);
			}

			uint32_t NetBuffer::WriteStream(const void* data, size_t data_size)
			{
				if (!data || data_size == 0) return 0;

				const char* index = (const char*)data;
				size_t left_size = data_size;
				while (left_size)
				{
					CheckBuffer(mode_write);

					size_t write_size = std::min < size_t >(left_size, 
						currentBuffer_.bufferSize - count_);

					memcpy(currentBuffer_.buffer, index, write_size);
					index += write_size;
					count_ += write_size;

					left_size -= write_size;
				}
				return data_size;
			}

			void NetBuffer::ToString(char*& data, size_t& data_size)
			{
				if (buffers_.Size() <= 0)
				{
					data = nullptr;
					data_size = 0;
					return;
				}

				data_size = 0;
				for (size_t i = 0; i < buffers_.Size(); ++i)
				{
					data_size += (i + 1 == buffers_.Size() ? count_ : buffers_[i].bufferSize);
				}

				data = new char[data_size];
				char* index = data;
				for (size_t i = 0; i < buffers_.Size(); ++i)
				{
					size_t real_size = (i + 1 == buffers_.Size() ? count_ : buffers_[i].bufferSize);
					memcpy(index, buffers_[i].buffer, real_size);
					index += real_size;
				}
			}
		}
	}
}