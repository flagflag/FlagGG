#ifndef __BATCH3D__
#define __BATCH3D__

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Batch.h"

namespace FlagGG
{
	namespace Graphics
	{
		class Batch3D : public Batch
		{
		public:
			Batch3D(Texture* texture, VertexVector* vertexs = nullptr);

			~Batch3D() override = default;

			void AddTriangle(const Math::Vector3& v1, const Math::Vector3& v2, const Math::Vector3& v3,
				const Math::Vector2& uv1, const Math::Vector2& uv2, const Math::Vector2& uv3, 
				const Math::Vector3& n1, const Math::Vector3& n2, const Math::Vector3& n3,
				unsigned color);
		};
	}
}

#endif