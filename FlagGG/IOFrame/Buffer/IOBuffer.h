#ifndef __IOBUFFER__
#define __IOBUFFER__

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Container/Str.h"
#include "Container/ArrayPtr.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class FlagGG_API IOBuffer : public Container::RefCounted
			{
			public:
				virtual UInt32 GetIndex() const = 0;
				virtual UInt32 GetSize() const = 0;
				virtual void ClearIndex() = 0;
				virtual void Seek(UInt32 pos) = 0;
				virtual bool IsEof() = 0;
				virtual void Clear() = 0;

				virtual UInt32 ReadStream(void* data, UInt32 dataSize) = 0;
				virtual UInt32 WriteStream(const void* data, UInt32 dataSize) = 0;

				virtual void ReadInt8(Int8& value) = 0;
				virtual void WriteInt8(Int8 value) = 0;

				virtual void ReadUInt8(UInt8& value) = 0;
				virtual void WriteUInt8(UInt8 value) = 0;

				virtual void ReadInt16(Int16& value) = 0;
				virtual void WriteInt16(Int16 value) = 0;

				virtual void ReadUInt16(UInt16& value) = 0;
				virtual void WriteUInt16(UInt16 value) = 0;

				virtual void ReadInt32(Int32& value) = 0;
				virtual void WriteInt32(Int32 value) = 0;

				virtual void ReadUInt32(UInt32& value) = 0;
				virtual void WriteUInt32(UInt32 value) = 0;

				virtual void ReadInt64(Int64& value) = 0;
				virtual void WriteInt64(Int64 value) = 0;

				virtual void ReadUInt64(UInt64& value) = 0;
				virtual void WriteUInt64(UInt64 value) = 0;

				virtual void ReadFloat(Real& value) = 0;
				virtual void WriteFloat(Real value) = 0;

				virtual void WriteString(const Container::String& value) = 0;

				virtual void ToString(Container::String& result) = 0;
				virtual void ToBuffer(Container::SharedArrayPtr<char>& buffer, UInt32& bufferSize) = 0;
			};

			typedef Container::SharedPtr < IOBuffer > IOBufferPtr;
		}
	}
}

#endif