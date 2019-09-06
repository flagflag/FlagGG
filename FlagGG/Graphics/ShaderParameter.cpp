#include "Graphics/ShaderParameter.h"

namespace FlagGG
{
	namespace Graphics
	{
		bool ShaderParameters::AddParametersDefine(Container::StringHash key, uint32_t typeSize)
		{
			if (descs.Contains(key))
				return false;

			if (!dataBuffer_)
				dataBuffer_ = new IOFrame::Buffer::StringBuffer();

			ShaderParameterDesc& desc = descs[key];
			desc.offset_ = dataBuffer_->GetSize();
			desc.size_ = typeSize;
			dataBuffer_->Seek(desc.offset_);
			for (uint32_t i = 0; i < desc.size_; ++i)
			{
				dataBuffer_->WriteUInt8(0);
			}

			return true;
		}

		bool ShaderParameters::SetValue(Container::StringHash key, const char* buffer, uint32_t bufferSize)
		{
			if (!dataBuffer_)
				return false;

			auto it = descs.Find(key);
			if (it == descs.End() || bufferSize != it->second_.size_)
				return false;

			dataBuffer_->Seek(it->second_.offset_);
			dataBuffer_->WriteStream(buffer, it->second_.size_);
			return true;
		}

		void ShaderParameters::WriteToBuffer(GPUBuffer* buffer)
		{
			if (!dataBuffer_)
				return;

			uint32_t dataSize = dataBuffer_->GetSize();
			if (buffer->SetSize(dataSize))
			{
				void* data = buffer->Lock(0, dataSize);
				dataBuffer_->ReadStream(data, dataSize);
				buffer->Unlock();
			}
		}
	}
}
