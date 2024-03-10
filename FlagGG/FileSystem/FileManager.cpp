#include "FileManager.h"

namespace FlagGG
{

AssetFileManager::AssetFileManager()
{

}

AssetFileManager::~AssetFileManager()
{

}

SharedPtr<IOFrame::Buffer::IOBuffer> AssetFileManager::OpenFileReader(const String& fileName)
{
	for (auto& archive : fileSystemArchives_)
	{
		if (SharedPtr<IOFrame::Buffer::IOBuffer> buffer = archive->OpenFileReader(fileName))
		{
			return buffer;
		}
	}
	return nullptr;
}

SharedPtr<IOFrame::Buffer::IOBuffer> AssetFileManager::OpenFileWriter(const String& fileName)
{
	for (auto& archive : fileSystemArchives_)
	{
		if (SharedPtr<IOFrame::Buffer::IOBuffer> buffer = archive->OpenFileWriter(fileName))
		{
			return buffer;
		}
	}
	return nullptr;
}

bool AssetFileManager::DirectoryExists(const String& directoryPath)
{
	for (auto& archive : fileSystemArchives_)
	{
		if (archive->DirectoryExists(directoryPath))
			return true;
	}
	return false;
}

bool AssetFileManager::FileExists(const String& fileName)
{
	for (auto& archive : fileSystemArchives_)
	{
		if (archive->FileExists(fileName))
			return true;
	}
	return false;
}

bool AssetFileManager::GetFileTimeStamp(const String& fileName, UInt64& timeStamp)
{
	for (auto& archive : fileSystemArchives_)
	{
		if (archive->GetFileTimeStamp(fileName, timeStamp))
			return true;
	}
	return false;
}

void AssetFileManager::FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& pattern, bool recursive, FindFileModeFlags fileMode)
{
	for (auto& archive : fileSystemArchives_)
	{
		archive->FindFiles(foundFiles, directoryPath, pattern, recursive, fileMode);
	}
}

void AssetFileManager::AddArchive(FileSystemArchive* fileSystemArchive)
{
	fileSystemArchives_.Push(SharedPtr<FileSystemArchive>(fileSystemArchive));
}

void AssetFileManager::RemoveArchive(FileSystemArchive* fileSystemArchive)
{
	fileSystemArchives_.Remove(SharedPtr<FileSystemArchive>(fileSystemArchive));
}

}
