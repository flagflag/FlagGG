#pragma once

#include "IOFrame/Buffer/IOBuffer.h"
#include "Resource/XMLElement.h"

namespace FlagGG
{

class FlagGG_API StreamObject
{
public:
	virtual ~StreamObject() {}

	bool LoadFile(const String& filepath);

	bool LoadStream(IOFrame::Buffer::IOBuffer* stream);

	bool SaveFile(const String& filepath);

	bool SaveFile(IOFrame::Buffer::IOBuffer* stream);

protected:
	// 至于BeginLoad为什么定义成非共享指针的原因：LoadFile是在栈上创建IOBuffer的子类
	virtual bool BeginLoad(IOFrame::Buffer::IOBuffer* stream);

	virtual bool EndLoad();

	virtual bool BeginSave(IOFrame::Buffer::IOBuffer* stream);

	virtual bool EndSave();
};

class FlagGG_API StreamObjectXML
{
public:
	virtual ~StreamObjectXML() {}

	// 从XML加载
	virtual bool LoadXML(const XMLElement& root);

	// 保存到XML中
	virtual bool SaveXML(XMLElement& root);
};

}
