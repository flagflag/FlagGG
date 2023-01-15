#include "Graphics/ShaderParameter.h"

namespace FlagGG
{

bool ShaderParameters::AddParametersDefineImpl(StringHash key, UInt32 typeSize)
{
	if (descs.Contains(key))
		return false;

	if (!dataBuffer_)
		dataBuffer_ = new IOFrame::Buffer::StringBuffer();

	ShaderParameterDesc& desc = descs[key];
	desc.offset_ = dataBuffer_->GetSize();
	desc.size_ = typeSize;
	dataBuffer_->Seek(desc.offset_);
	for (UInt32 i = 0; i < desc.size_; ++i)
	{
		dataBuffer_->WriteUInt8(0);
	}

	return true;
}

bool ShaderParameters::SetValueImpl(StringHash key, const void* buffer, UInt32 bufferSize)
{
	if (!dataBuffer_)
		return false;

	auto it = descs.Find(key);
	if (it == descs.End() || bufferSize > it->second_.size_)
		return false;

	dataBuffer_->Seek(it->second_.offset_);
	dataBuffer_->WriteStream(buffer, it->second_.size_);
	return true;
}

void ShaderParameters::WriteToBuffer(ConstantBuffer* buffer)
{
	if (!dataBuffer_)
		return;

	UInt32 dataSize = dataBuffer_->GetSize();
	buffer->SetSize(dataSize);
	void* data = buffer->Lock(0, dataSize);
	dataBuffer_->ClearIndex();
	dataBuffer_->ReadStream(data, dataSize);
	buffer->Unlock();
}

}
