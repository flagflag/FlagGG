#pragma once

#include "Core/BaseTypes.h"
#include "Container/Str.h"
#include "Container/Vector.h"
#include "Container/FlagSet.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "FileSystem/FileHandle.h"
#if defined(WIN32)
#include "FileSystem/PlatformFileInterface/WindowsFileInterfaceUndef.h"
#endif

namespace FlagGG
{

enum class FileAttributes : UInt32
{
	NONE		= 0,
	EXISTS		= 1 << 0,
	READONLY	= 1 << 2,
	HIDDEN      = 1 << 3,
	COMPRESSED  = 1 << 4,
	EXECUTABLE  = 1 << 5,
};
FLAGGG_FLAGSET(FileAttributes, FileAttributesFlags);

enum class FindFileMode
{
	NONE       = 0,
	DIRECTORY  = 1 << 0,
	FILE       = 1 << 2,
};
FLAGGG_FLAGSET(FindFileMode, FindFileModeFlags);

class FlagGG_API IFileSystem
{
public:
	virtual ~IFileSystem() {}

	// 检测目录是否存在
	virtual bool DirectoryExists(const String& directoryPath) = 0;

	// 创建目录
	virtual bool CreateDirectory(const String& directoryPath) = 0;

	// 删除目录
	virtual bool DeleteDirectory(const String& directoryPath) = 0;

	// 检测文件是否存在
	virtual bool FileExists(const String& fileName) = 0;

	// 获取文件大小
	virtual bool GetFileSize(const String& fileName, Int64& fileSize) = 0;

	// 获取文件属性
	virtual bool GetFileAttributes(const String& fileName, FileAttributesFlags& attributes) = 0;

	// 设置文件属性
	virtual bool SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes) = 0;

	// 获取文件时间戳
	virtual bool GetFileTimeStamp(const String& fileName, UInt64& timeStamp) = 0;

	// 打开文件，并且返回抽象文件流
	virtual SharedPtr<IOFrame::Buffer::IOBuffer> OpenFile(const String& fileName, FileMode fileMode) = 0;

	// 创建文件
	virtual bool CreateFile(const String& fileName) = 0;

	// 删除文件
	virtual bool DeleteFile(const String& fileName) = 0;

	// 移动文件
	virtual bool MoveFile(const String& fileName, const String& newFileName) = 0;

	// 拷贝文件
	virtual bool CopyFile(const String& fileName, const String& targetFileName) = 0;

	// 从目录下搜索文件
	virtual void FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& fileExtension, bool recursive, FindFileModeFlags fileMode) = 0;
};

FlagGG_API IFileSystem* GetLocalFileSystem();
FlagGG_API IFileSystem* GetPackageFileSystem();
FlagGG_API IFileSystem* GetFtpFileSystem();

}
