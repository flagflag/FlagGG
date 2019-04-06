#include "RenderContext.h"

namespace FlagGG
{
	namespace Graphics
	{
		RenderContext::RenderContext(Batch* batch, Shader* VS, Shader* PS, VertexFormat* format) :
			VSShader_(VS),
			PSShader_(PS),
			format_(format)
		{
			if (batch)
			{
				batchs_.Push(Container::SharedPtr<Batch>(batch));
			}	
		}

		bool RenderContext::IsValid() const
		{
			return batchs_.Size() > 0 && VSShader_ != nullptr && PSShader_ != nullptr;
		}
	}
}
