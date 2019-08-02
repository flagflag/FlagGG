#include "IOFrame/Stream/FileStream.h"

#include <stdio.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Stream
		{
#define FileHandler(file) static_cast<FILE*>(file)

#ifdef WIN32 || WIN64
#define FilePathString Container::WString
#define FormatPath(filePath) Container::WString(filePath.Replaced('/', '\\'))
#define FileAPI_Open(file, filePath, fileMode) (file = _wfopen(filePath.CString(), fileMode))
#define ModeText(mode) L##mode
#define FileAPI_Close(file) { fclose(FileHandler(file)); file = nullptr; }
#define FileAPI_IsOpen(file) (file && !feof(FileHandler(file_)))
#define FileAPI_SeekFromBegin(file, delta) fseek(FileHandler(file), delta, SEEK_SET)
#define FileAPI_SeekToBegin(file) fseek(FileHandler(file), 0, SEEK_SET)
#define FileAPI_SeekToEnd(file) fseek(FileHandler(file), 0, SEEK_END)
#define FileAPI_Tell(file) ftell(FileHandler(file))
#define FileAPI_SimpleRead(file, value) fread(&value, sizeof(value), 1, FileHandler(file))
#define FileAPI_SimpleWrite(file, value) fwrite(&value, sizeof(value), 1, FileHandler(file))
#define FileAPI_ReadStream(file, data, dataSize) (fread(data, dataSize, 1, FileHandler(file)) * dataSize)
#define FileAPI_WriteStream(file, data, dataSize) (fwrite(data, dataSize, 1, FileHandler(file)) * dataSize)
#define FileAPI_IsEof(file) (!!feof(FileHandler(file)))
#else
#define FilePathString Container::String
#define FormatPath(filePath) Container::String(filePath.Replaced('/', '\\'))
#define FileAPI_Open(file, filePath, fileMode) (file = fopen(filePath.CString(), fileMode))
#define ModeText(mode) mode
#define FileAPI_Close(file) { fclose(FileHandler(file)); file = nullptr; }
#define FileAPI_IsOpen(file) (file && !feof(FileHandler(file_)))
#define FileAPI_SeekFromBegin(file, delta) fseek(FileHandler(file), delta, SEEK_SET)
#define FileAPI_SeekToBegin(file) seek(FileHandler(file), 0, SEEK_SET)
#define FileAPI_SeekToEnd(file) fseek(FileHandler(file), 0, SEEK_END)
#define FileAPI_Tell(file) ftell(FileHandler(file))
#define FileAPI_SimpleRead(file, value) fread(&value, sizeof(value), 1, FileHandler(file))
#define FileAPI_SimpleWrite(file, value) fwrite(&value, sizeof(value), 1, FileHandler(file))
#define FileAPI_ReadStream(file, data, dataSize) (fread(data, dataSize, 1, FileHandler(file)) * dataSize)
#define FileAPI_WriteStream(file, data, dataSize) (fwrite(data, dataSize, 1, FileHandler(file)) * dataSize)
#define FileAPI_IsEof(file) (!!feof(FileHandler(file))))
#endif

			FileStream::FileStream() :
				file_(nullptr)
			{ }

			FileStream::~FileStream()
			{
				if (IsOpen()) Close();
			}

			void FileStream::Open(const Container::String& filePath, FileMode fileMode)
			{
				FilePathString filePathString = FormatPath(filePath);
				switch (fileMode)
				{
				case FileMode::FILE_READ:
					FileAPI_Open(file_, filePathString, ModeText("rb"));
					break;
				case FileMode::FILE_WRITE:
					FileAPI_Open(file_, filePathString, ModeText("wb"));
					break;
				case FileMode::FILE_READ_WIRTE:
					FileAPI_Open(file_, filePathString, ModeText("wb+"));
					break;
				}
			}

			void FileStream::Close()
			{
				FileAPI_Close(file_);
			}

			bool FileStream::IsOpen() const
			{
				return FileAPI_IsOpen(file_);
			}

			uint32_t FileStream::GetIndex() const
			{
				return FileAPI_Tell(file_);
			}

			uint32_t FileStream::GetSize() const
			{
				uint32_t index = FileAPI_Tell(file_);
				FileAPI_SeekToEnd(file_);
				uint32_t fileSize = FileAPI_Tell(file_);
				FileAPI_SeekFromBegin(file_, index);
				return fileSize;
			}

			void FileStream::ClearIndex()
			{
				FileAPI_SeekToBegin(file_);
			}

			void FileStream::Seek(uint32_t pos)
			{
				FileAPI_SeekFromBegin(file_, pos);
			}

			uint32_t FileStream::ReadStream(void* data, uint32_t dataSize)
			{
				return FileAPI_ReadStream(file_, data, dataSize);
			}

			uint32_t FileStream::WriteStream(const void* data, uint32_t dataSize)
			{
				return FileAPI_WriteStream(file_, data, dataSize);
			}

			bool FileStream::IsEof()
			{
				return FileAPI_IsEof(file_);
			}
		}
	}
}
