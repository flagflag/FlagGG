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
	Batch2D(VertexVector* vertexs = nullptr, Texture* texture = nullptr);

	~Batch2D() override;

	void AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
		const Vector2& uv1, const Vector2& uv2, const Vector2& uv3,
		UInt32 color1, UInt32 color2, UInt32 color3);
};

}

#endif