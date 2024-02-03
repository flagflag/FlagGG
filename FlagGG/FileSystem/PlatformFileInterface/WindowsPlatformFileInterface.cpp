#if defined(WIN32)
#include "FileSystem/PlatformFileInterface.h"

#include <windows.h>
#include <io.h>

#include "FileSystem/PlatformFileInterface/WindowsFileInterfaceUndef.h"

namespace FlagGG
{

bool PlatformFileInterface::DirectoryExists(const String& directoryPath)
{
	const DWORD fileAttributes = ::GetFileAttributesW(WString(directoryPath).CString());
	return fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool PlatformFileInterface::CreateDirectory(const String& directoryPath)
{
	return (::CreateDirectoryW(WString(directoryPath).CString(), nullptr) == TRUE) ||
		(::GetLastError() == ERROR_ALREADY_EXISTS);
}

bool PlatformFileInterface::DeleteDirectory(const String& directoryPath)
{
	return ::RemoveDirectoryW(WString(directoryPath).CString()) == TRUE;
}

bool PlatformFileInterface::FileExists(const String& fileName)
{
	const DWORD fileAttributes = ::GetFileAttributesW(WString(fileName).CString());
	return fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

Int64 PlatformFileInterface::GetFileSize(const String& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA Info;
	if (!!::GetFileAttributesExW(WString(fileName).CString(), GetFileExInfoStandard, &Info))
	{
		if ((Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			LARGE_INTEGER li;
			li.HighPart = Info.nFileSizeHigh;
			li.LowPart = Info.nFileSizeLow;
			return li.QuadPart;
		}
	}
	return -1;
}

bool PlatformFileInterface::GetFileAttributes(const String& fileName, FileAttributesFlags& attributes)
{
	attributes = FileAttributes::NONE;
	DWORD fileAttributes = ::GetFileAttributesW(WString(fileName).CString());
	DWORD error = ::GetLastError();
	if (fileAttributes != INVALID_FILE_ATTRIBUTES)
	{
		attributes |= FileAttributes::EXISTS;
		if ((fileAttributes & FILE_ATTRIBUTE_READONLY) != 0)
		{
			attributes |= FileAttributes::READONLY;
		}
		if ((fileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0)
		{
			attributes |= FileAttributes::HIDDEN;
		}
		if ((fileAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0)
		{
			attributes |= FileAttributes::COMPRESSED;
		}
		return true;
	}
	else if (error == ERROR_PATH_NOT_FOUND || error == ERROR_FILE_NOT_FOUND)
	{
		return true;
	}
	return false;
}

bool PlatformFileInterface::SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes)
{
	DWORD fileAttributes = 0;

	if (attributes & FileAttributes::READONLY)
	{
		fileAttributes |= FILE_ATTRIBUTE_READONLY;
	}

	if (attributes & FileAttributes::HIDDEN)
	{
		fileAttributes |= FILE_ATTRIBUTE_HIDDEN;
	}

	if (attributes & FileAttributes::COMPRESSED)
	{
		fileAttributes |= FILE_ATTRIBUTE_COMPRESSED;
	}
	
	return ::SetFileAttributesW(WString(fileName).CString(), fileAttributes) == TRUE;
}

bool PlatformFileInterface::GetFileTimeStamp(const String& fileName, UInt64& timeStamp)
{
	return false;
}

bool PlatformFileInterface::CreateFile(const String& fileName)
{
	return false;
}

bool PlatformFileInterface::DeleteFile(const String& fileName)
{
	return ::DeleteFileW(WString(fileName).CString());
}

bool PlatformFileInterface::MoveFile(const String& fileName, const String& newFileName)
{
	return ::MoveFileW(WString(fileName).CString(), WString(newFileName).CString());
}

bool PlatformFileInterface::CopyFile(const String& fileName, const String& targetFileName)
{
	return ::CopyFileW(WString(fileName).CString(), WString(targetFileName).CString(), FALSE);
}

}

#include "PlatformFileInterface.inl"

#endif
