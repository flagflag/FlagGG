#ifndef __IOBUFFER__
#define __IOBUFFER__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"

#include <stdint.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class FlagGG_API IOBuffer : public Container::RefCounted
			{
			public:
				virtual ~IOBuffer() = default;

				virtual void ClearIndex() = 0;

				virtual void ReadInt8(int8_t& value) = 0;
				virtual void WriteInt8(int8_t value) = 0;

				virtual void ReadUInt8(uint8_t& value) = 0;
				virtual void WriteUInt8(uint8_t value) = 0;

				virtual void ReadInt16(int16_t& value) = 0;
				virtual void WriteInt16(int16_t value) = 0;

				virtual void ReadUInt16(uint16_t& value) = 0;
				virtual void WriteUInt16(uint16_t value) = 0;

				virtual void ReadInt32(int32_t& value) = 0;
				virtual void WriteInt32(int32_t value) = 0;

				virtual void ReadUInt32(uint32_t& value) = 0;
				virtual void WriteUInt32(uint32_t value) = 0;

				virtual void ReadInt64(int64_t& value) = 0;
				virtual void WriteInt64(int64_t value) = 0;

				virtual void ReadUInt64(uint64_t& value) = 0;
				virtual void WriteUInt64(uint64_t value) = 0;

				virtual void WriteStream(const char* data, size_t data_size) = 0;

				virtual void ToString(char*& data, size_t& data_size) = 0;
			};

			typedef Container::SharedPtr < IOBuffer > IOBufferPtr;
		}
	}
}

#endif