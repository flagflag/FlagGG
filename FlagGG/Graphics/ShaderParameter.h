#pragma once

#include "Container/HashMap.h"
#include "Container/Vector.h"
#include "Graphics/GPUBuffer.h"
#include "IOFrame/Buffer/StringBuffer.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Graphics
	{
		struct ShaderParameterDesc
		{
			UInt32 offset_;
			UInt32 size_;
		};

		class FlagGG_API ShaderParameters : public Container::RefCounted
		{
		public:
			void WriteToBuffer(GPUBuffer* buffer);

			template < class Type >
			bool AddParametersDefine(Container::StringHash key)
			{
				return AddParametersDefineImpl(key, sizeof(Type));
			}

			bool AddParametersDefineImpl(Container::StringHash key, UInt32 typeSize);

			template < class Type >
			bool SetValue(Container::StringHash key, Type value)
			{
				return SetValueImpl(key, &value, sizeof(Type));
			}

			bool SetValueImpl(Container::StringHash key, const void* buffer, UInt32 bufferSize);

			friend class RenderEngine;

		private:
			Container::HashMap<Container::StringHash, ShaderParameterDesc> descs;

			Container::SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;
		};
	}
}
