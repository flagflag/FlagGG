//
// 资产文件管理
//

#pragma once

#include "Core/Subsystem.h"
#include "FileSystem/FileSystemArchive.h"

namespace FlagGG
{

class FlagGG_API AssetFileManager : public Subsystem<AssetFileManager>, public FileSystemArchive
{
public:
	AssetFileManager();

	~AssetFileManager() override;

// Override FileSystem Archive
	// 以只读方式打开一个文件，并且返回抽象文件流
	SharedPtr<IOFrame::Buffer::IOBuffer> OpenFileReader(const String& fileName) override;

	// 以写方式打开一个文件，并且返回抽象文件流
	SharedPtr<IOFrame::Buffer::IOBuffer> OpenFileWriter(const String& fileName) override;

	// 检测目录是否存在
	bool DirectoryExists(const String& directoryPath) override;

	// 检测文件是否存在
	bool FileExists(const String& fileName) override;

	// 获取文件时间戳
	bool GetFileTimeStamp(const String& fileName, UInt64& timeStamp) override;

	// 从目录下搜索文件
	void FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& fileExtension, bool recursive, FindFileModeFlags fileMode) override;


	// 增加子文件系统
	void AddArchive(FileSystemArchive* fileSystemArchive);

	// 删除子文件系统
	void RemoveArchive(FileSystemArchive* fileSystemArchive);

protected:
	// 返回是否只读
	bool IsReadOnly() const override { return true; }

	// 返回是否区分路径大小写
	bool IsCaseSensitive() const override { return false; }

private:
	Vector<SharedPtr<FileSystemArchive>> fileSystemArchives_;
};

}
