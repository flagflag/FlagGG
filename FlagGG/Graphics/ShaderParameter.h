#pragma once

#include "Container/HashMap.h"
#include "Container/Vector.h"
#include "Graphics/GPUBuffer.h"
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
	void WriteToBuffer(GPUBuffer* buffer);

	template < class Type >
	bool AddParametersDefine(StringHash key)
	{
		return AddParametersDefineImpl(key, sizeof(Type));
	}

	bool AddParametersDefineImpl(StringHash key, UInt32 typeSize);

	template < class Type >
	bool SetValue(StringHash key, Type value)
	{
		return SetValueImpl(key, &value, sizeof(Type));
	}

	bool SetValueImpl(StringHash key, const void* buffer, UInt32 bufferSize);

	friend class GfxDeviceD3D11;

private:
	HashMap<StringHash, ShaderParameterDesc> descs;

	SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;
};

}
