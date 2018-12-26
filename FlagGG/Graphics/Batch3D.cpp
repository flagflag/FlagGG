#include "Batch3D.h"

namespace FlagGG
{
	namespace Graphics
	{
		Batch3D::Batch3D(BatchType type, Texture* texture, VertexVector* vertexs) :
			Batch(type, texture, vertexs, 32)
		{
		}

		void Batch3D::AddTriangle(const Math::Vector3& v1, const Math::Vector3& v2, const Math::Vector3& v3,
			const Math::Vector2& uv1, const Math::Vector2& uv2, const Math::Vector2& uv3,
			const Math::Vector3& n1, const Math::Vector3& n2, const Math::Vector3& n3,
			unsigned color)
		{
			if (type_ != DRAW_TRIANGLE) return;

			unsigned begin = vertexs_->size();
			vertexEnd_ = vertexs_->size() + vertexSize_ * 3;
			vertexs_->resize(vertexEnd_);

			float* dest = reinterpret_cast<float*>(&(*vertexs_)[begin]);
			dest[0] = v1.x_;
			dest[1] = v1.y_;
			dest[2] = v1.z_;
			dest[3] = uv1.x_;
			dest[4] = uv1.y_;
			dest[5] = n1.x_;
			dest[6] = n1.y_;
			dest[7] = n1.z_;

			dest[8] = v2.x_;
			dest[9] = v2.y_;
			dest[10] = v2.z_;
			dest[11] = uv2.x_;
			dest[12] = uv2.y_;
			dest[13] = n2.x_;
			dest[14] = n2.y_;
			dest[15] = n2.z_;

			dest[16] = v3.x_;
			dest[17] = v3.y_;
			dest[18] = v3.z_;
			dest[19] = uv3.x_;
			dest[20] = uv3.y_;
			dest[21] = n3.x_;
			dest[22] = n3.y_;
			dest[23] = n3.z_;
		}

		void Batch3D::AddLine(const Math::Vector3& v1, const Math::Vector3& v2,
			const Math::Vector2& uv1, const Math::Vector2& uv2,
			unsigned color)
		{
			if (type_ != DRAW_LINE) return;

			unsigned begin = vertexs_->size();
			vertexEnd_ = vertexs_->size() + vertexSize_ * 2;
			vertexs_->resize(vertexEnd_);

			float* dest = reinterpret_cast<float*>(&(*vertexs_)[begin]);
			dest[0] = v1.x_;
			dest[1] = v1.y_;
			dest[2] = v1.z_;
			dest[3] = uv1.x_;
			dest[4] = uv1.y_;
			dest[5] = v1.x_;
			dest[6] = v1.y_;
			dest[7] = v1.z_;

			dest[8] = v2.x_;
			dest[9] = v2.y_;
			dest[10] = v2.z_;
			dest[11] = uv2.x_;
			dest[12] = uv2.y_;
			dest[13] = v2.x_;
			dest[14] = v2.y_;
			dest[15] = v2.z_;
		}

		void Batch3D::AddBlob(const void* data, unsigned size)
		{
			unsigned begin = vertexs_->size();
			vertexEnd_ = vertexs_->size() + size;
			vertexs_->resize(vertexEnd_);

			void* dest = &(*vertexs_)[begin];
			memcpy(dest, data, size);
		}
	}
}
