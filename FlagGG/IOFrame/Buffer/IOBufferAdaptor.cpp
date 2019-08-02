#include "IOFrame/Buffer/IOBufferAdaptor.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			void IOBufferAdaptor::ReadInt8(int8_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt8(int8_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt8(uint8_t& value) 
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt8(uint8_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadInt16(int16_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt16(int16_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt16(uint16_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt16(uint16_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadInt32(int32_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt32(int32_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt32(uint32_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt32(uint32_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadInt64(int64_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteInt64(int64_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadUInt64(uint64_t& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteUInt64(uint64_t value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ReadFloat(float& value)
			{
				ReadStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::WriteFloat(float value)
			{
				WriteStream(&value, sizeof(value));
			}

			void IOBufferAdaptor::ToString(Container::String& result)
			{
				uint32_t bufferSize = GetSize();
				result.Resize(bufferSize);
				ReadStream(&result[0], bufferSize);
			}

			void IOBufferAdaptor::ToBuffer(Container::SharedArrayPtr<char>& buffer, uint32_t& bufferSize)
			{
				bufferSize = GetSize();
				buffer = new char[bufferSize];
				ReadStream(buffer.Get(), bufferSize);
			}
		}
	}

}
