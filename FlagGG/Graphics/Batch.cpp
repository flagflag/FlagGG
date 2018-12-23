#include "Batch.h"
#include "Texture.h"

namespace FlagGG
{
	namespace Graphics
	{
		Batch::Batch(Texture* texture, VertexVector* vertexs) :
			texture_(texture),
			vertexs_(vertexs)
		{
			owner_ = vertexs_ == nullptr;

			if (vertexs_ == nullptr)
			{
				vertexs_ = new VertexVector();
				vertexStart_ = 0;
				vertexEnd_ = 0;
			}
			else
			{
				vertexStart_ = vertexEnd_ = vertexs->size();
			}
		}

		Batch::~Batch()
		{
			if (owner_ && vertexs_)
			{
				delete vertexs_;
				vertexs_ = 0;
			}
		}

		void Batch::AddTriangle(const Math::Vector2& v1, const Math::Vector2& v2, const Math::Vector2& v3,
			const Math::Vector2& uv1, const Math::Vector2& uv2, const Math::Vector2& uv3, unsigned color)
		{
			unsigned begin = vertexs_->size();
			vertexEnd_ = vertexs_->size() + vertexSize_ * 3;
			vertexs_->resize(vertexEnd_);

			float* dest = reinterpret_cast<float*>(&(*vertexs_)[begin]);
			dest[0] = v1.x_;
			dest[1] = v1.y_;
			dest[2] = 0.0f;
			((unsigned&)dest[3]) = color;
			dest[4] = uv1.x_;
			dest[5] = uv1.y_;

			dest[6] = v2.x_;
			dest[7] = v2.y_;
			dest[8] = 0.0f;
			((unsigned&)dest[9]) = color;
			dest[10] = uv2.x_;
			dest[11] = uv2.y_;

			dest[12] = v3.x_;
			dest[13] = v3.y_;
			dest[14] = 0.0f;
			((unsigned&)dest[15]) = color;
			dest[16] = uv3.x_;
			dest[17] = uv3.y_;
		}

		VertexVector* Batch::GetVertexs() const
		{
			return vertexs_;
		}

		unsigned Batch::GetVertexSize() const
		{
			return vertexSize_;
		}

		unsigned Batch::GetVertexCount() const
		{
			return vertexs_->size() / vertexSize_;
		}

		Texture* Batch::GetTexture() const
		{
			return texture_;
		}
	}
}
