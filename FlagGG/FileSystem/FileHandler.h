//
// 文件底层抽象接口
//

#pragma once

#include "Core/BaseTypes.h"
#include "Container/Str.h"

namespace FlagGG
{

/// 文件打开模式.
enum class FileMode
{
	FILE_READ = 0,
	FILE_WRITE,
	FILE_ADD,
	FILE_READWRITE,
	FILE_DEFAULT = FILE_READ,
};

/// 文件指针移动方式.
enum class SeekFrom
{
	SEEK_FROM_BEGIN = 0,
	SEEK_FROM_CUR,
	SEEK_FROM_END,
};

/// 抽线文件句柄基类
class FlagGG_API IFileHandler
{
public:
	virtual ~IFileHandler() {}

	/// 打开文件，成功返回true
	virtual bool Open(const String& fileName, FileMode mode) = 0;

	/// 返回文件是否被打开
	virtual bool IsOpen() const = 0;

	/// 从文件读取字节流，返回实际读取的字节大小
	virtual bool Read(void* buffer, USize size) = 0;

	/// 写字节流到文件，返回实际写的字节大小
	virtual bool Write(const void* buffer, USize size) = 0;

	/// 设置文件指针位置
	virtual bool Seek(SSize offset, SeekFrom origin) = 0;

	/// 关闭文件
	virtual void Close() = 0;

	/// 刷新文件缓冲区
	virtual void Flush() = 0;

	/// 返回文件指针位置
	virtual USize Position() const = 0;

	/// 返回文件大小
	virtual USize Size() const = 0;

	/// 返回文件的原生句柄
	virtual void* NativePtr() const = 0;
};

}
