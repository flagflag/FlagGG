#pragma once

#include "FileSystem/FileSystem.h"

namespace FlagGG
{

class FlagGG_API PlatformFileInterface
{
public:
	// 检测目录是否存在
	static bool DirectoryExists(const String& directoryPath);

	// 创建目录
	static bool CreateDirectory(const String& directoryPath);

	// 删除目录
	static bool DeleteDirectory(const String& directoryPath);

	// 检测文件是否存在
	static bool FileExists(const String& fileName);

	// 获取文件大小
	static Int64 GetFileSize(const String& fileName);

	// 获取文件属性
	static bool GetFileAttributes(const String& fileName, FileAttributesFlags& attributes);

	// 设置文件属性
	static bool SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes);

	// 获取文件时间戳
	static bool GetFileTimeStamp(const String& fileName, UInt64& timeStamp);

	// 创建文件
	static bool CreateFile(const String& fileName);

	// 删除文件
	static bool DeleteFile(const String& fileName);

	// 移动文件
	static bool MoveFile(const String& fileName, const String& newFileName);

	// 拷贝文件
	static bool CopyFile(const String& fileName, const String& targetFileName);

	// 从目录下搜索文件
	static void FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& pattern, bool recursive, FindFileModeFlags fileMode);
};

}
