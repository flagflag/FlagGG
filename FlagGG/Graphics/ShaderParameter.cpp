#include "Graphics/ShaderParameter.h"

namespace FlagGG
{
	namespace Graphics
	{
		void ShaderParameters::WriteToBuffer(GPUBuffer* buffer)
		{
			if (!dataBuffer_)
				return;

			//uint32_t dataSize = dataBuffer_->GetSize();
			//buffer->SetSize(dataSize);
			//void* data = buffer->Lock(0, dataSize);
			//dataBuffer_->ReadStream(data, dataSize);
			//buffer->Unlock();
		}
	}
}
