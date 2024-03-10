#include "LocalFileSystem.h"
#include "FileSystem/PlatformFileInterface.h"
#include "IOFrame/Stream/FileStream.h"

namespace FlagGG
{

LocalFileSystem::LocalFileSystem()
{

}

LocalFileSystem::~LocalFileSystem()
{

}

bool LocalFileSystem::DirectoryExists(const String& directoryPath)
{
	return PlatformFileInterface::DirectoryExists(directoryPath);
}

bool LocalFileSystem::CreateDirectory(const String& directoryPath)
{
	return PlatformFileInterface::CreateDirectory(directoryPath);
}

bool LocalFileSystem::DeleteDirectory(const String& directoryPath)
{
	return PlatformFileInterface::DeleteDirectory(directoryPath);
}

bool LocalFileSystem::FileExists(const String& fileName)
{
	return PlatformFileInterface::FileExists(fileName);
}

bool LocalFileSystem::GetFileSize(const String& fileName, Int64& fileSize)
{
	fileSize = PlatformFileInterface::GetFileSize(fileName);
	return fileSize >= 0;
}

bool LocalFileSystem::GetFileAttributes(const String& fileName, FileAttributesFlags& attributes)
{
	return PlatformFileInterface::GetFileAttributes(fileName, attributes);
}

bool LocalFileSystem::SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes)
{
	return PlatformFileInterface::SetFileAttributes(fileName, attributes);
}

bool LocalFileSystem::GetFileTimeStamp(const String& fileName, UInt64& timeStamp)
{
	return PlatformFileInterface::GetFileTimeStamp(fileName, timeStamp);
}

SharedPtr<IOFrame::Buffer::IOBuffer> LocalFileSystem::OpenFile(const String& fileName, FileMode fileMode)
{
	auto fileStream = MakeShared<IOFrame::Stream::FileStream>();
	fileStream->Open(fileName, fileMode);
	return fileStream->IsOpen() ? fileStream : nullptr;
}

bool LocalFileSystem::CreateFile(const String& fileName)
{
	return PlatformFileInterface::CreateFile(fileName);
}

bool LocalFileSystem::DeleteFile(const String& fileName)
{
	return PlatformFileInterface::DeleteFile(fileName);
}

bool LocalFileSystem::MoveFile(const String& fileName, const String& newFileName)
{
	return PlatformFileInterface::MoveFile(fileName, newFileName);
}

bool LocalFileSystem::CopyFile(const String& fileName, const String& targetFileName)
{
	return PlatformFileInterface::CopyFile(fileName, targetFileName);
}

void LocalFileSystem::FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& pattern, bool recursive, FindFileModeFlags fileMode)
{
	PlatformFileInterface::FindFiles(foundFiles, directoryPath, pattern, recursive, fileMode);
}

IFileSystem* GetLocalFileSystem()
{
	static LocalFileSystem fileSystem;
	return &fileSystem;
}

}
