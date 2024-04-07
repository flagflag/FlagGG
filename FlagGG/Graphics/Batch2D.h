#ifndef __BATCH2D__
#define __BATCH2D__

#include "Export.h"

#include "Math/Vector2.h"
#include "Batch.h"

namespace FlagGG
{

class FlagGG_API Batch2D : public Batch
{
public:
	Batch2D(Texture* texture = nullptr, VertexVector* vertexs = nullptr);

	~Batch2D() override;

	void AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
		const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, unsigned color);
};

}

#endif