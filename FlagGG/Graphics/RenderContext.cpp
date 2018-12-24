#include "RenderContext.h"

namespace FlagGG
{
	namespace Graphics
	{
		RenderContext::RenderContext(Batch* batch, Shader* VS, Shader* PS, VertexFormat* format) :
			batch_(batch),
			VSShader_(VS),
			PSShader_(PS),
			format_(format)
		{
		}

		bool RenderContext::IsValid() const
		{
			return batch_ != nullptr && VSShader_ != nullptr && PSShader_ != nullptr;
		}
	}
}
