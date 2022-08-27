#pragma once

#include "Container/HashMap.h"
#include "Container/Vector.h"
#include "Graphics/GPUBuffer.h"
#include "IOFrame/Buffer/StringBuffer.h"
#include "bgfx/bgfx.h"

namespace FlagGG
{
	namespace Graphics
	{
		class ShaderProgram;

		struct ShaderParameterDesc
		{
			UInt32 offset_;
			UInt32 size_;

			Container::String name_;
			bgfx::UniformHandle handle_;
			bgfx::UniformType::Enum type_;
			UInt32 num_;
		};

		class FlagGG_API ShaderParameters : public Container::RefCounted
		{
		public:
			void WriteToBuffer(GPUBuffer* buffer);

			template < class Type >
			bool AddParametersDefine(Container::String key, bgfx::UniformType::Enum type = bgfx::UniformType::Count, UInt32 num = 0u)
			{
				return AddParametersDefineImpl(key, sizeof(Type), type, num);
			}

			bool AddParametersDefineImpl(Container::String key, UInt32 typeSize, bgfx::UniformType::Enum type, UInt32 num);

			template < class Type >
			bool SetValue(Container::String key, Type value)
			{
				return SetValueImpl(key, &value, sizeof(Type));
			}

			bool SetValueImpl(Container::String key, const void* buffer, UInt32 bufferSize);

			void SubmitUniforms(ShaderProgram* program);

			friend class RenderEngine;

		private:
			Container::HashMap<Container::String, ShaderParameterDesc> descs_;

			Container::SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;

			Container::PODVector<char> tempBuffer_;
		};
	}
}
