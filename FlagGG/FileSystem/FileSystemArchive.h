//
// 抽象文件处理
//

#pragma once

#include "FileSystem/FileSystem.h"
#include "Container/RefCounted.h"

namespace FlagGG
{

class FlagGG_API FileSystemArchive : public RefCounted
{
public:
	virtual ~FileSystemArchive() {}

	// 返回是否只读
	virtual bool IsReadOnly() const = 0;

	// 返回是否区分路径大小写
	virtual bool IsCaseSensitive() const = 0;

	// 以只读方式打开一个文件，并且返回抽象文件流
	virtual SharedPtr<IOFrame::Buffer::IOBuffer> OpenFileReader(const String& fileName) = 0;

	// 以写方式打开一个文件，并且返回抽象文件流
	virtual SharedPtr<IOFrame::Buffer::IOBuffer> OpenFileWriter(const String& fileName) = 0;

	// 检测目录是否存在
	virtual bool DirectoryExists(const String& directoryPath) = 0;

	// 检测文件是否存在
	virtual bool FileExists(const String& fileName) = 0;

	// 获取文件时间戳
	virtual bool GetFileTimeStamp(const String& fileName, UInt64& timeStamp) = 0;

	// 从目录下搜索文件
	virtual void FindFiles(Vector<String>& foundFiles, const String& directoryPath, const String& fileExtension, bool recursive, FindFileModeFlags fileMode) = 0;
};

}
