#ifndef __BATCH3D__
#define __BATCH3D__

#include "Export.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Batch.h"

namespace FlagGG
{

class FlagGG_API Batch3D : public Batch
{
public:
	Batch3D(BatchType type, Texture* texture, VertexVector* vertexs = nullptr);

	~Batch3D() override = default;

	void AddTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3,
		const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, 
		const Vector3& n1, const Vector3& n2, const Vector3& n3,
		UInt32 color);

	void AddLine(const Vector3& v1, const Vector3& v2,
		const Vector2& uv1, const Vector2& uv2,
		UInt32 color);

	void AddBlob(const void* data, UInt32 size);
};

}

#endif