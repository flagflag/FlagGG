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
				bufferSize_(0u),
				capacity_(0u)
			{ }

			StringBuffer::StringBuffer(void* data, uint32_t length) :
				sizeFixed_(true),
				readOnly_(false),
				cBuffer_(static_cast<char*>(data)),
				ccBuffer_(static_cast<char*>(data)),
				index_(0u),
				bufferSize_(length),
				capacity_(length)
			{ }

			StringBuffer::StringBuffer(const void* data, uint32_t length) :
				sizeFixed_(true),
				readOnly_(true),
				cBuffer_(nullptr),
				ccBuffer_(static_cast<const char*>(data)),
				index_(0u),
				bufferSize_(length),
				capacity_(length)
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

			uint32_t StringBuffer::ReadStream(void* data, uint32_t dataSize)
			{
				if (index_ + dataSize < bufferSize_)
				{
					memcpy(data, ccBuffer_ + index_, dataSize);
					index_ += dataSize;
				}
				return dataSize;
			}

			uint32_t StringBuffer::WriteStream(const void* data, uint32_t dataSize)
			{
				if (readOnly_) return 0;
				if (index_ + dataSize >= capacity_)
				{
					if (sizeFixed_) return 0;
					capacity_ = (index_ + dataSize) << 1u;
					auto* newBuffer = static_cast<char*>(realloc(cBuffer_, sizeof(char)* capacity_));
					if (!newBuffer)
					{
						throw "Failed to realloc buffer!";
					}
					ccBuffer_ = cBuffer_ = newBuffer;
				}
				memcpy(cBuffer_ + index_, data, dataSize);
				index_ += dataSize;
				bufferSize_ = index_;
				return dataSize;
			}

			bool StringBuffer::IsEof()
			{
				return index_ >= bufferSize_;
			}
		}
	}
}
