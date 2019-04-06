#include "Batch2D.h"
#include "Texture.h"

namespace FlagGG
{
	namespace Graphics
	{
		Batch2D::Batch2D(Texture* texture, VertexVector* vertexs) :
			Batch(DRAW_TRIANGLE, texture, vertexs, 20)
		{
		}

		void Batch2D::AddTriangle(const Math::Vector2& v1, const Math::Vector2& v2, const Math::Vector2& v3,
			const Math::Vector2& uv1, const Math::Vector2& uv2, const Math::Vector2& uv3, unsigned color)
		{
			unsigned begin = vertexs_->Size();
			vertexEnd_ = vertexs_->Size() + vertexSize_ * 3;
			vertexs_->Resize(vertexEnd_);

			float* dest = reinterpret_cast<float*>(&(*vertexs_)[begin]);
			dest[0] = v1.x_;
			dest[1] = v1.y_;
			dest[2] = 0.0f;
			dest[3] = uv1.x_;
			dest[4] = uv1.y_;

			dest[5] = v2.x_;
			dest[6] = v2.y_;
			dest[7] = 0.0f;
			dest[8] = uv2.x_;
			dest[9] = uv2.y_;

			dest[10] = v3.x_;
			dest[11] = v3.y_;
			dest[12] = 0.0f;
			dest[13] = uv3.x_;
			dest[14] = uv3.y_;
		}
	}
}
