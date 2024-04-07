#include "Batch2D.h"
#include "Texture.h"

namespace FlagGG
{

Batch2D::Batch2D(Texture* texture, VertexVector* vertexs) :
	Batch(DRAW_TRIANGLE, texture, vertexs, 24)
{
}

Batch2D::~Batch2D() = default;

void Batch2D::AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
	const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, unsigned color)
{
	unsigned begin = vertexs_->Size();
	vertexEnd_ = vertexs_->Size() + vertexSize_ / 4u * 3;
	vertexs_->Resize(vertexEnd_);

	float* dest = &(vertexs_->At(begin));
	dest[0] = v1.x_;
	dest[1] = v1.y_;
	dest[2] = 0.0f;
	dest[3] = uv1.x_;
	dest[4] = uv1.y_;
	((unsigned&)dest[5]) = color;

	dest[6] = v2.x_;
	dest[7] = v2.y_;
	dest[8] = 0.0f;
	dest[9] = uv2.x_;
	dest[10] = uv2.y_;
	((unsigned&)dest[11]) = color;

	dest[12] = v3.x_;
	dest[13] = v3.y_;
	dest[14] = 0.0f;
	dest[15] = uv3.x_;
	dest[16] = uv3.y_;
	((unsigned&)dest[17]) = color;
}

}
