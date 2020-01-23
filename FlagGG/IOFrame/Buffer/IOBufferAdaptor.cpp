#include "IOFrame/Buffer/IOBufferAdaptor.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			void IOBufferAdaptor::ReadInt8(Int8& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt8(Int8 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt8(UInt8& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt8(UInt8 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadInt16(Int16& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt16(Int16 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt16(UInt16& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt16(UInt16 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadInt32(Int32& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt32(Int32 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt32(UInt32& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt32(UInt32 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadInt64(Int64& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt64(Int64 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt64(UInt64& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt64(UInt64 value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadFloat(Real& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteFloat(Real value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ToString(Container::String& result)
			{
				UInt32 bufferSize = GetSize();
				result.Resize(bufferSize);
				ReadStream(&result[0], bufferSize);
			}

			void IOBufferAdaptor::ToBuffer(Container::SharedArrayPtr<char>& buffer, UInt32& bufferSize)
			{
				bufferSize = GetSize();
				buffer = new char[bufferSize];
				ReadStream(buffer.Get(), bufferSize);
			}
		}
	}

}
