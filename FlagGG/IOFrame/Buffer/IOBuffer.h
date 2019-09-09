#ifndef __IOBUFFER__
#define __IOBUFFER__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Container/Str.h"
#include "Container/ArrayPtr.h"

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
				virtual uint32_t GetIndex() const = 0;
				virtual uint32_t GetSize() const = 0;
				virtual void ClearIndex() = 0;
				virtual void Seek(uint32_t pos) = 0;
				virtual bool IsEof() = 0;
				virtual void Clear() = 0;

				virtual uint32_t ReadStream(void* data, uint32_t dataSize) = 0;
				virtual uint32_t WriteStream(const void* data, uint32_t dataSize) = 0;

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

				virtual void ReadFloat(float& value) = 0;
				virtual void WriteFloat(float value) = 0;

				virtual void ToString(Container::String& result) = 0;
				virtual void ToBuffer(Container::SharedArrayPtr<char>& buffer, uint32_t& bufferSize) = 0;
			};

			typedef Container::SharedPtr < IOBuffer > IOBufferPtr;
		}
	}
}

#endif