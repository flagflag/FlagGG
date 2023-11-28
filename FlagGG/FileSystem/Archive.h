//
// 抽象文件处理
//

#pragma once

#include "Container/RefCounted.h"
#include "Container/Str.h"
#include "Container/Vector.h"
#include "FileSystem/FileHandler.h"

namespace FlagGG
{

class FlagGG_API Archive : public RefCounted
{
public:
	// 返回是否只读
	virtual bool IsReadOnly() const = 0;

	// 返回是否区分路径大小写
	virtual bool IsCaseSensitive() const = 0;

	// 打开一个文件，并且返回抽象文件句柄
	virtual IFileHandler* Open(const String& fileName) = 0;

	// 创建一个文件，并且返回抽象文件句柄
	virtual IFileHandler* Create(const String& fileName) = 0;

	// 删除一个文件，返回是否成功
	virtual bool Remove(const String& fileName) = 0;

	// 判断文件是否存在
	virtual bool Exists(const String& fileName) = 0;

	// 返回文件修改时间
	virtual Int64 GetModifiedTime(const String& fileName) = 0;

	// 遍历当前Archive，返回所有满足条件的文件路径
	virtual void List(Vector<String>& results, const String& pattern, bool recursive, Int32 fileMode) = 0;
};

}
