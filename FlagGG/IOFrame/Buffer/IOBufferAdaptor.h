#pragma once

#include "Export.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class IOBufferAdaptor : public IOBuffer
			{
			public:
				void ReadInt8(int8_t& value) override;
				void WriteInt8(int8_t value) override;

				void ReadUInt8(uint8_t& value) override;
				void WriteUInt8(uint8_t value) override;

				void ReadInt16(int16_t& value) override;
				void WriteInt16(int16_t value) override;

				void ReadUInt16(uint16_t& value) override;
				void WriteUInt16(uint16_t value) override;

				void ReadInt32(int32_t& value) override;
				void WriteInt32(int32_t value) override;

				void ReadUInt32(uint32_t& value) override;
				void WriteUInt32(uint32_t value) override;

				void ReadInt64(int64_t& value) override;
				void WriteInt64(int64_t value) override;

				void ReadUInt64(uint64_t& value) override;
				void WriteUInt64(uint64_t value) override;

				void ReadFloat(float& value) override;
				void WriteFloat(float value) override;

				void ToString(Container::String& result) override;
				void ToBuffer(Container::SharedArrayPtr<char>& buffer, uint32_t& bufferSize) override;
			};
		}
	}
}
