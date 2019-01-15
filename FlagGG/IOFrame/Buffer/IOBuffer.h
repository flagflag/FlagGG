#ifndef __IOBUFFER__
#define __IOBUFFER__

#include "Export.h"

#include <memory>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class FlagGG_API IOBuffer
			{
			public:
				virtual ~IOBuffer() = default;

				virtual void clearIndex() = 0;

				virtual void readInt8(int8_t& value) = 0;
				virtual void writeInt8(int8_t value) = 0;

				virtual void readUInt8(uint8_t& value) = 0;
				virtual void writeUInt8(uint8_t value) = 0;

				virtual void readInt16(int16_t& value) = 0;
				virtual void writeInt16(int16_t value) = 0;

				virtual void readUInt16(uint16_t& value) = 0;
				virtual void writeUInt16(uint16_t value) = 0;

				virtual void readInt32(int32_t& value) = 0;
				virtual void writeInt32(int32_t value) = 0;

				virtual void readUInt32(uint32_t& value) = 0;
				virtual void writeUInt32(uint32_t value) = 0;

				virtual void readInt64(int64_t& value) = 0;
				virtual void writeInt64(int64_t value) = 0;

				virtual void readUInt64(uint64_t& value) = 0;
				virtual void writeUInt64(uint64_t value) = 0;

				virtual void writeStream(const char* data, size_t data_size) = 0;

				virtual void toString(char*& data, size_t& data_size) = 0;
			};

			typedef std::shared_ptr < IOBuffer > IOBufferPtr;
		}
	}
}

#endif