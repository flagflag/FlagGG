#ifndef __BATCH2D__
#define __BATCH2D__

#include "Export.h"

#include "Math/Vector2.h"
#include "Batch.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API Batch2D : public Batch
		{
		public:
			Batch2D(Texture* texture = nullptr, VertexVector* vertexs = nullptr);

			~Batch2D() override = default;

			void AddTriangle(const Math::Vector2& v1, const Math::Vector2& v2, const Math::Vector2& v3,
				const Math::Vector2& uv1, const Math::Vector2& uv2, const Math::Vector2& uv3, unsigned color);
		};
	}
}

#endif