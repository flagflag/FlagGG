#ifndef __FILE_STREAM__
#define __FILE_STREAM__

#include "Export.h"
#include "Container/Str.h"
#include "IOFrame/Buffer/IOBuffer.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Stream
		{
			enum class FileMode
			{
				FILE_READ		= 1 << 0,
				FILE_WRITE		= 1 << 1,
				FILE_READ_WIRTE = FILE_READ | FILE_WRITE,
				FILE_DEFAULT	= FILE_READ,
			};

			class FlagGG_API FileStream : public Buffer::IOBuffer
			{
			public:
				typedef void* FilePoint;
				
				FileStream();

				~FileStream() override;

				void Open(const Container::String& filePath, FileMode fileMode = FileMode::FILE_DEFAULT);
				void Close();
				bool IsOpen() const;

				void ClearIndex() override;

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

				uint32_t ReadStream(char* data, size_t dataSize) override;
				void WriteStream(const char* data, size_t dataSize) override;

				void ToString(char*& data, size_t& dataSize) override;
			
			protected:
				FilePoint file_;
			};
		}
	}
}

#endif
