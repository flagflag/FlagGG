#pragma once

#include "FileSystem/FileSystem.h"

namespace FlagGG
{

class LocalFileSystem : public IFileSystem
{
public:
	LocalFileSystem();

	~LocalFileSystem() override;

	// 检测目录是否存在
	bool DirectoryExists(const String& directoryPath) override;

	// 创建目录
	bool CreateDirectory(const String& directoryPath) override;

	// 删除目录
	bool DeleteDirectory(const String& directoryPath) override;

	// 检测文件是否存在
	bool FileExists(const String& fileName) override;

	// 获取文件大小
	bool GetFileSize(const String& fileName, Int64& fileSize) override;

	// 获取文件属性
	bool GetFileAttributes(const String& fileName, FileAttributesFlags& attributes) override;

	// 设置文件属性
	bool SetFileAttributes(const String& fileName, const FileAttributesFlags& attributes) override;

	// 获取文件时间戳
	bool GetFileTimeStamp(const String& fileName, UInt64& timeStamp) override;

	// 打开文件
	SharedPtr<IOFrame::Buffer::IOBuffer> OpenFile(const String& fileName, FileMode fileMode) override;

	// 创建文件
	bool CreateFile(const String& fileName) override;

	// 删除文件
	bool DeleteFile(const String& fileName) override;

	// 移动文件
	bool MoveFile(const String& fileName, const String& newFileName) override;

	// 拷贝文件
	bool CopyFile(const String& fileName, const String& targetFileName) override;

	// 从目录下搜索文件
	void FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& fileExtension, bool recursive, FindFileModeFlags fileMode) override;

private:

};

}
