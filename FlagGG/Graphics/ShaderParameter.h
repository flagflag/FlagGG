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

		class ShaderParameters : public Container::RefCounted
		{
		public:
			void WriteToBuffer(GPUBuffer* buffer);

			template < class Type >
			bool AddParametersDefine(Container::StringHash key)
			{
				return AddParametersDefine(key, sizeof(Type));
			}

			bool AddParametersDefine(Container::StringHash key, uint32_t typeSize);

			template < class Type >
			bool SetValue(Container::StringHash key, Type value)
			{
				return SetValue(&value, sizeof(Type));
			}

			bool SetValue(Container::StringHash key, const char* buffer, uint32_t bufferSize);

		private:
			Container::HashMap<Container::StringHash, ShaderParameterDesc> descs;

			Container::SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;
		};
	}
}
