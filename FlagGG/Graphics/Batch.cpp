#include "Batch.h"
#include "Texture.h"

namespace FlagGG
{
	namespace Graphics
	{
		Batch::Batch(BatchType type, Texture* texture, VertexVector* vertexs, unsigned vertexSize) :
			type_(type),
			texture_(texture),
			vertexs_(vertexs),
			vertexSize_(vertexSize)
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
				vertexStart_ = vertexEnd_ = vertexs->Size();
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
			return vertexs_->Size() / vertexSize_;
		}

		Texture* Batch::GetTexture() const
		{
			return texture_;
		}

		BatchType Batch::GetType() const
		{
			return type_;
		}
	}
}
