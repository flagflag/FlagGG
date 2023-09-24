#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Core/Context.h"
#include "Core/Object.h"

namespace FlagGG
{

class FlagGG_API Resource : public Object
{
	OBJECT_OVERRIDE(Resource, Object);
public:
	Resource();

	~Resource() override = default;

	void SetName(const String& name);

	const String& GetName() const { return name_; }

	bool LoadFile(const String& filepath);

	bool LoadFile(IOFrame::Buffer::IOBuffer* stream);

	bool SaveFile(const String& filepath);

	bool SaveFile(IOFrame::Buffer::IOBuffer* stream);

	void SetMemoryUse(UInt32 memoryUse);

	UInt32 GetMemoryUse() const;

protected:
	// 至于BeginLoad为什么定义成非共享指针的原因：LoadFile是在栈上创建IOBuffer的子类
	virtual bool BeginLoad(IOFrame::Buffer::IOBuffer* stream);

	virtual bool EndLoad();

	virtual bool BeginSave(IOFrame::Buffer::IOBuffer* stream);

	virtual bool EndSave();

	String name_;

	UInt32 memoryUse_{ 0 };
};

}
