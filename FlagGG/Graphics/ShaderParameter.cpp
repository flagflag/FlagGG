#include "Graphics/ShaderParameter.h"

namespace FlagGG
{
	namespace Graphics
	{
		bool ShaderParameters::AddParametersDefineImpl(Container::String key, UInt32 typeSize, bgfx::UniformType::Enum type, UInt32 num)
		{
			if (descs.Contains(key))
				return false;

			if (!dataBuffer_)
				dataBuffer_ = new IOFrame::Buffer::StringBuffer();

			ShaderParameterDesc& desc = descs[key];
			desc.offset_ = dataBuffer_->GetSize();
			desc.size_ = typeSize;
			desc.type_ = type;
			desc.num_ = num;
			dataBuffer_->Seek(desc.offset_);
			for (UInt32 i = 0; i < desc.size_; ++i)
			{
				dataBuffer_->WriteUInt8(0);
			}

			return true;
		}

		bool ShaderParameters::SetValueImpl(Container::String key, const void* buffer, UInt32 bufferSize)
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

			UInt32 dataSize = dataBuffer_->GetSize();
			if (buffer->SetSize(dataSize))
			{
				void* data = buffer->Lock(0, dataSize);
				dataBuffer_->ClearIndex();
				dataBuffer_->ReadStream(data, dataSize);
				buffer->Unlock();
			}
		}

		void ShaderParameters::SubmitUniforms()
		{
			if (!dataBuffer_)
				return;

			dataBuffer_->ClearIndex();
			for (auto it : descs)
			{
				bgfx::UniformHandle handle = bgfx::createUniform(it.first_.CString(), it.second_.type_, it.second_.num_);
				dataBuffer_->Seek(it.second_.offset_);
				char* data = new char[it.second_.size_];
				dataBuffer_->ReadStream(data, it.second_.size_);
				bgfx::setUniform(handle, data, it.second_.num_);
				delete[] data;
			}
		}
	}
}
