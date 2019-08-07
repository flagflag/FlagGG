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

		class ShaderParameters
		{
		public:
			void WriteToBuffer(GPUBuffer* buffer);

			template < class Type >
			bool AddParametersDefine(Container::StringHash key)
			{
				if (descs.Contains(key))
					return false;

				if (!dataBuffer_)
					dataBuffer_ = new IOFrame::Buffer::StringBuffer();

				ShaderParameterDesc& desc = descs[key];
				desc.offset_ = dataBuffer_->GetSize();
				desc.size_ = sizeof(Type);
				dataBuffer_->Seek(desc.offset_);
				for (uint32_t i = 0; i < desc.size_; ++i)
				{
					dataBuffer_->WriteUInt8(0);
				}
			}

			template < class Type >
			bool SetValue(Container::StringHash key, Type value)
			{
				if (!dataBuffer_)
					return false;

				auto it = descs.Find(key);
				if (it == descs.End() || sizeof(Type) != it->second_.size_)
					return false;

				dataBuffer_->Seek(it->second_.offset_);
				dataBuffer_->WriteStream(&value, it->second_.size_);
				return true;
			}

		private:
			Container::HashMap<Container::StringHash, ShaderParameterDesc> descs;

			Container::SharedPtr<IOFrame::Buffer::StringBuffer> dataBuffer_;
		};
	}
}
