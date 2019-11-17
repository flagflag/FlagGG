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
			uint32_t offset_;
			uint32_t size_;
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

			bool AddParametersDefineImpl(Container::StringHash key, uint32_t typeSize);

			template < class Type >
			bool SetValue(Container::StringHash key, Type value)
			{
				return SetValueImpl(key, &value, sizeof(Type));
			}

			bool SetValueImpl(Container::StringHash key, const void* buffer, uint32_t bufferSize);

			friend class RenderEngine;

		private:
			Container::HashMap<Container::StringHash, ShaderParameterDesc> descs;

			Container::SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;
		};
	}
}
