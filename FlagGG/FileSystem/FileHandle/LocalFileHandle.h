//
// 本地文件句柄，内部实现为C标准库
//

#pragma once

#include "FileSystem/FileHandle.h"

namespace FlagGG
{

/// 本地文件句柄
/// Using C standard IO functions.
class FlagGG_API LocalFileHandle : public IFileHandle
{
public:
	LocalFileHandle();

	~LocalFileHandle() override;

	/// 打开文件，成功返回true
	bool Open(const String& fileName, FileMode mode) override;

	/// 返回文件是否被打开
	bool IsOpen() const override;

	/// 从文件读取字节流，返回实际读取的字节大小
	bool Read(void* buffer, USize size) override;

	/// 写字节流到文件，返回实际写的字节大小
	bool Write(const void* buffer, USize size) override;

	/// 设置文件指针位置
	bool Seek(SSize offset, SeekFrom origin) override;

	/// 关闭文件
	void Close() override;

	/// 刷新文件缓冲区
	void Flush() override;

	/// 返回文件指针位置
	USize Position() const override;

	/// 返回文件大小
	USize Size() const override;

	/// 返回文件的原生句柄
	void* NativePtr() const override { return handle_; }

private:
	void* handle_;

	mutable USize size_;

	mutable bool sizeDirty_;
};

}
