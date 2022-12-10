#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "IOFrame/Buffer/IOBufferAdaptor.h"

namespace FlagGG { namespace IOFrame { namespace Stream {

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

	void Open(const String& filePath, FileMode fileMode = FileMode::FILE_DEFAULT);
	void Close();
	bool IsOpen() const;

	UInt32 GetIndex() const override;
	UInt32 GetSize() const override;

	void ClearIndex() override;

	void Seek(UInt32 pos) override;

	void Clear() override;

	UInt32 ReadStream(void* data, UInt32 dataSize) override;
	UInt32 WriteStream(const void* data, UInt32 dataSize) override;

	bool IsEof() override;
			
protected:
	FilePoint file_;
};

}}}
