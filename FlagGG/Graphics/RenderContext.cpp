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
				batchs_.emplace_back(batch);
			}	
		}

		bool RenderContext::IsValid() const
		{
			return batchs_.size() > 0 && VSShader_ != nullptr && PSShader_ != nullptr;
		}
	}
}
