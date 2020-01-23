#include "Batch.h"
#include "Texture.h"

namespace FlagGG
{
	namespace Graphics
	{
		Batch::Batch(BatchType type, Texture* texture, VertexVector* vertexs, Size vertexSize) :
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

		const char* Batch::GetVertexs() const
		{
			return (const char*)&(vertexs_->At(0));
		}

		unsigned Batch::GetVertexSize() const
		{
			return vertexSize_;
		}

		unsigned Batch::GetVertexCount() const
		{
			return vertexs_->Size() * 4u / vertexSize_;
		}

		Texture* Batch::GetTexture() const
		{
			return texture_;
		}

		void Batch::SetTexture(Texture* texture)
		{
			texture_ = texture;
		}

		BatchType Batch::GetType() const
		{
			return type_;
		}
	}
}
