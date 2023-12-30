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
	// ����BeginLoadΪʲô����ɷǹ���ָ���ԭ��LoadFile����ջ�ϴ���IOBuffer������
	virtual bool BeginLoad(IOFrame::Buffer::IOBuffer* stream);

	virtual bool EndLoad();

	virtual bool BeginSave(IOFrame::Buffer::IOBuffer* stream);

	virtual bool EndSave();
};

class FlagGG_API StreamObjectXML
{
public:
	virtual ~StreamObjectXML() {}

	// ��XML����
	virtual bool LoadXML(const XMLElement& root);

	// ���浽XML��
	virtual bool SaveXML(XMLElement& root);
};

}
