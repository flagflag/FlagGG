#ifndef __RENDER_CONTEXT__
#define __RENDER_CONTEXT__

#include "Batch.h"
#include "Shader.h"
#include "VertexFormat.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		struct RenderContext
		{
			RenderContext(Batch* batch, Shader* VS, Shader* PS, VertexFormat* format);

			~RenderContext() = default;

			bool IsValid() const;

			std::vector<Batch*> batchs_;

			Shader* VSShader_;

			Shader* PSShader_;

			VertexFormat* format_;
		};
	}
}

#endif