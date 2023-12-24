#pragma once

#include "IOFrame/Buffer/IOBuffer.h"

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

}
