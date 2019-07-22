#include "IOFrame/Buffer/StringBuffer.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			StringBuffer::StringBuffer() :
				sizeFixed_(false),
				readOnly_(false),
				cBuffer_(nullptr),
				ccBuffer_(nullptr),
				index_(0u),
				bufferSize_(0u)
			{ }

			StringBuffer::StringBuffer(void* data, uint32_t length) :
				sizeFixed_(true),
				readOnly_(false),
				cBuffer_(static_cast<char*>(data)),
				ccBuffer_(static_cast<char*>(data)),
				index_(0u),
				bufferSize_(length)
			{ }

			StringBuffer::StringBuffer(const void* data, uint32_t length) :
				sizeFixed_(true),
				readOnly_(true),
				cBuffer_(nullptr),
				ccBuffer_(static_cast<const char*>(data)),
				index_(0u),
				bufferSize_(length)
			{ }

			uint32_t StringBuffer::GetIndex() const
			{
				return index_;
			}

			uint32_t StringBuffer::GetSize() const
			{
				return bufferSize_;
			}

			void StringBuffer::ClearIndex()
			{
				index_ = 0;
			}

			void StringBuffer::Seek(uint32_t pos)
			{
				index_ = pos;
			}

			void StringBuffer::ReadInt8(int8_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteInt8(int8_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadUInt8(uint8_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteUInt8(uint8_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadInt16(int16_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteInt16(int16_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadUInt16(uint16_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteUInt16(uint16_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadInt32(int32_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteInt32(int32_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadUInt32(uint32_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteUInt32(uint32_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadInt64(int64_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteInt64(int64_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ReadUInt64(uint64_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteUInt64(uint64_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			uint32_t StringBuffer::ReadStream(void* data, size_t dataSize)
			{
				if (index_ + dataSize < bufferSize_)
				{
					memcpy(data, ccBuffer_ + index_, dataSize);
					index_ += dataSize;
				}
				return dataSize;
			}

			uint32_t StringBuffer::WriteStream(const void* data, size_t dataSize)
			{
				if (readOnly_) return 0;
				if (index_ + dataSize >= bufferSize_)
				{
					if (sizeFixed_) return 0;
					bufferSize_ <<= 1u;
					auto* newBuffer = static_cast<char*>(realloc(cBuffer_, sizeof(char)* bufferSize_));
					if (!newBuffer)
					{
						throw "Failed to realloc buffer!";
					}
					ccBuffer_ = cBuffer_ = newBuffer;
				}
				memcpy(cBuffer_ + index_, data, dataSize);
				index_ += dataSize;
				return dataSize;
			}

			void StringBuffer::ReadFloat(float& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void StringBuffer::WriteFloat(float value)
			{
				WriteStream(&value, sizeof(value));
			}

			void StringBuffer::ToString(char*& data, size_t& dataSize)
			{

			}

			bool StringBuffer::IsEof()
			{
				return index_ >= bufferSize_;
			}
		}
	}
}
