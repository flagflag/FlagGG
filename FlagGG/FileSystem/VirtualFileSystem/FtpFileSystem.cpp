#include "FtpFileSystem.h"

namespace FlagGG
{

FtpFileSystem::FtpFileSystem()
{
}

FtpFileSystem::~FtpFileSystem()
{

}

bool FtpFileSystem::DirectoryExists(const String& directoryPath)
{
	return false;
}

bool FtpFileSystem::CreateDirectory(const String& directoryPath)
{
	return false;
}

bool FtpFileSystem::DeleteDirectory(const String& directoryPath)
{
	return false;
}

bool FtpFileSystem::FileExists(const String& fileName)
{
	return false;
}

bool FtpFileSystem::GetFileSize(const String& fileName, Int64& fileSize)
{
	return false;
}

bool FtpFileSystem::GetFileAttributes(const String& fileName, FileAttributesFlags& attributes)
{
	return false;
}

bool FtpFileSystem::SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes)
{
	return false;
}

bool FtpFileSystem::GetFileTimeStamp(const String& fileName, UInt64& timeStamp)
{
	return false;
}

SharedPtr<IOFrame::Buffer::IOBuffer> FtpFileSystem::OpenFile(const String& fileName, FileMode fileMode)
{
	return nullptr;
}

bool FtpFileSystem::CreateFile(const String& fileName)
{
	return false;
}

bool FtpFileSystem::DeleteFile(const String& fileName)
{
	return false;
}

bool FtpFileSystem::MoveFile(const String& fileName, const String& newFileName)
{
	return false;
}

bool FtpFileSystem::CopyFile(const String& fileName, const String& targetFileName)
{
	return false;
}

void FtpFileSystem::FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& fileExtension, bool recursive, FindFileModeFlags fileMode)
{

}

}
