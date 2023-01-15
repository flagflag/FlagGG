//
// Shader参数集
//

#pragma once

#include "Container/HashMap.h"
#include "Container/Vector.h"
#include "Graphics/ConstantBuffer.h"
#include "IOFrame/Buffer/StringBuffer.h"

#include <stdint.h>

namespace FlagGG
{

struct ShaderParameterDesc
{
	UInt32 offset_;
	UInt32 size_;
};

class FlagGG_API ShaderParameters : public RefCounted
{
public:
	// 将数据写入ConstantBuffer
	void WriteToBuffer(ConstantBuffer* buffer);

	// 增加参数定义
	template < class Type >
	bool AddParametersDefine(StringHash key)
	{
		return AddParametersDefineImpl(key, sizeof(Type));
	}

	// 增加参数定义（具体实现）
	bool AddParametersDefineImpl(StringHash key, UInt32 typeSize);

	// 设置参数的值
	template < class Type >
	bool SetValue(StringHash key, Type value)
	{
		return SetValueImpl(key, &value, sizeof(Type));
	}

	// 设置参数的值（具体实现）
	bool SetValueImpl(StringHash key, const void* buffer, UInt32 bufferSize);

	friend class GfxDeviceD3D11;

private:
	HashMap<StringHash, ShaderParameterDesc> descs;

	SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;
};

}
