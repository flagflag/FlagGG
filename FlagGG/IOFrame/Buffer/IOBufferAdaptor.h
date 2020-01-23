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
			class FlagGG_API IOBufferAdaptor : public IOBuffer
			{
			public:
				void ReadInt8(Int8& value) override;
				void WriteInt8(Int8 value) override;

				void ReadUInt8(UInt8& value) override;
				void WriteUInt8(UInt8 value) override;

				void ReadInt16(Int16& value) override;
				void WriteInt16(Int16 value) override;

				void ReadUInt16(UInt16& value) override;
				void WriteUInt16(UInt16 value) override;

				void ReadInt32(Int32& value) override;
				void WriteInt32(Int32 value) override;

				void ReadUInt32(UInt32& value) override;
				void WriteUInt32(UInt32 value) override;

				void ReadInt64(Int64& value) override;
				void WriteInt64(Int64 value) override;

				void ReadUInt64(UInt64& value) override;
				void WriteUInt64(UInt64 value) override;

				void ReadFloat(Real& value) override;
				void WriteFloat(Real value) override;

				void ToString(Container::String& result) override;
				void ToBuffer(Container::SharedArrayPtr<char>& buffer, UInt32& bufferSize) override;
			};
		}
	}
}
