#include "DefaultFileSystemArchive.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{

DefaultFileSystemArchive::DefaultFileSystemArchive(IFileSystem* fileSystem, const String& directoryPath)
	: fileSystem_(fileSystem)
	, rootDirectory_(FormatPath(directoryPath).ToLower() + PATH_SEPARATOR)
{

}

DefaultFileSystemArchive::~DefaultFileSystemArchive()
{

}

SharedPtr<IOFrame::Buffer::IOBuffer> DefaultFileSystemArchive::OpenFileReader(const String& fileName)
{
	return fileSystem_->OpenFile(rootDirectory_ + fileName, FileMode::FILE_READ);
}

SharedPtr<IOFrame::Buffer::IOBuffer> DefaultFileSystemArchive::OpenFileWriter(const String& fileName)
{
	return fileSystem_->OpenFile(rootDirectory_ + fileName, FileMode::FILE_WRITE);
}

bool DefaultFileSystemArchive::DirectoryExists(const String& directoryPath)
{
	return fileSystem_->DirectoryExists(rootDirectory_ + directoryPath);
}

bool DefaultFileSystemArchive::FileExists(const String& fileName)
{
	return fileSystem_->FileExists(rootDirectory_ + fileName);
}

bool DefaultFileSystemArchive::GetFileTimeStamp(const String& fileName, UInt64& timeStamp)
{
	return fileSystem_->GetFileTimeStamp(rootDirectory_ + fileName, timeStamp);
}

void DefaultFileSystemArchive::FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& pattern, bool recursive, FindFileModeFlags fileMode)
{
	fileSystem_->FindFiles(foundFiles, directoryPath, pattern, recursive, fileMode);
}


}
