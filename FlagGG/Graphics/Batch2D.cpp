#include "Batch2D.h"
#include "Texture.h"

namespace FlagGG
{

Batch2D::Batch2D(VertexVector* vertexs, Texture* texture) :
	Batch(DRAW_TRIANGLE, vertexs, 24, texture)
{
}

Batch2D::~Batch2D() = default;

void Batch2D::AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
	const Vector2& uv1, const Vector2& uv2, const Vector2& uv3,
	UInt32 color1, UInt32 color2, UInt32 color3)
{
	UInt32 begin = vertexs_->Size();
	vertexEnd_ = vertexs_->Size() + vertexSize_ / 4u * 3;
	vertexs_->Resize(vertexEnd_);

	float* dest = &(vertexs_->At(begin));
	dest[0] = v1.x_;
	dest[1] = v1.y_;
	dest[2] = 0.0f;
	dest[3] = uv1.x_;
	dest[4] = uv1.y_;
	((UInt32&)dest[5]) = color1;

	dest[6] = v2.x_;
	dest[7] = v2.y_;
	dest[8] = 0.0f;
	dest[9] = uv2.x_;
	dest[10] = uv2.y_;
	((UInt32&)dest[11]) = color2;

	dest[12] = v3.x_;
	dest[13] = v3.y_;
	dest[14] = 0.0f;
	dest[15] = uv3.x_;
	dest[16] = uv3.y_;
	((UInt32&)dest[17]) = color3;
}

}
