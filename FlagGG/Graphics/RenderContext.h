#ifndef __RENDER_CONTEXT__
#define __RENDER_CONTEXT__

#include "Export.h"

#include "Batch.h"
#include "Shader.h"
#include "VertexFormat.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		struct FlagGG_API RenderContext : public Container::RefCounted
		{
			RenderContext(Batch* batch, Shader* VS, Shader* PS, VertexFormat* format);

			~RenderContext() = default;

			bool IsValid() const;

			Container::Vector<Container::SharedPtr<Batch>> batchs_;

			Container::SharedPtr<Shader> VSShader_;

			Container::SharedPtr<Shader> PSShader_;

			Container::SharedPtr<VertexFormat> format_;
		};
	}
}

#endif