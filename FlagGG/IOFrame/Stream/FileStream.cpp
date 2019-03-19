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
#define FileAPI_ReadStream(file, data, dataSize) fread(data, dataSize, 1, FileHandler(file))
#define FileAPI_WriteStream(file, data, dataSize) fwrite(data, dataSize, 1, FileHandler(file))
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
#define FileAPI_ReadStream(file, data, dataSize) fread(data, dataSize, 1, FileHandler(file))
#define FileAPI_WriteStream(file, data, dataSize) fwrite(data, dataSize, 1, FileHandler(file))
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

			void FileStream::ClearIndex()
			{
				FileAPI_SeekToBegin(file_);
			}

			void FileStream::ReadInt8(int8_t& value) 
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteInt8(int8_t value)
			{
				fwrite(&value, sizeof(value), 1, FileHandler(file_));
			}

			void FileStream::ReadUInt8(uint8_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteUInt8(uint8_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			void FileStream::ReadInt16(int16_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteInt16(int16_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			void FileStream::ReadUInt16(uint16_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteUInt16(uint16_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			void FileStream::ReadInt32(int32_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteInt32(int32_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			void FileStream::ReadUInt32(uint32_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteUInt32(uint32_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			void FileStream::ReadInt64(int64_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteInt64(int64_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			void FileStream::ReadUInt64(uint64_t& value)
			{
				FileAPI_SimpleRead(file_, value);
			}

			void FileStream::WriteUInt64(uint64_t value)
			{
				FileAPI_SimpleWrite(file_, value);
			}

			uint32_t FileStream::ReadStream(char* data, size_t dataSize)
			{
				return FileAPI_ReadStream(file_, data, dataSize);
			}

			void FileStream::WriteStream(const char* data, size_t dataSize)
			{
				FileAPI_WriteStream(file_, data, dataSize);
			}

			void FileStream::ToString(char*& data, size_t& dataSize)
			{
				FileAPI_SeekToBegin(file_);
				dataSize = FileAPI_Tell(file_);
				data = new char[dataSize + 1];
				dataSize = FileAPI_ReadStream(file_, data, dataSize);
				data[dataSize] = '\0';
			}
		}
	}
}
