#ifndef __FILE_STREAM__
#define __FILE_STREAM__

#include "Export.h"
#include "Container/Str.h"
#include "IOFrame/Buffer/IOBufferAdaptor.h"

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

			class FlagGG_API FileStream : public Buffer::IOBufferAdaptor
			{
			public:
				typedef void* FilePoint;
				
				FileStream();

				~FileStream() override;

				void Open(const Container::String& filePath, FileMode fileMode = FileMode::FILE_DEFAULT);
				void Close();
				bool IsOpen() const;

				uint32_t GetIndex() const override;
				uint32_t GetSize() const override;

				void ClearIndex() override;

				void Seek(uint32_t pos) override;

				void Clear() override;

				uint32_t ReadStream(void* data, uint32_t dataSize) override;
				uint32_t WriteStream(const void* data, uint32_t dataSize) override;

				bool IsEof() override;
			
			protected:
				FilePoint file_;
			};
		}
	}
}

#endif
