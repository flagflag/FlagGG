#pragma once

#include "Export.h"

#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include "Batch.h"
#include "Graphics/GraphicsDef.h"
#include "Container/Vector.h"

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

	static const PODVector<VertexElement>& GetVertexElements();

	static UInt32 GetVertexElementsSize();
};

class FlagGG_API BatchWebKit : public Batch
{
public:
	BatchWebKit(VertexVector* vertexs = nullptr, Texture* texture = nullptr);

	~BatchWebKit() override;

	void AddTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3,
		const Vector2& uv1, const Vector2& uv2, const Vector2& uv3,
		UInt32 color1, UInt32 color2, UInt32 color3,
		const Vector4& data0 = Vector4::ZERO,
		const Vector4& data1 = Vector4::ZERO,
		const Vector4& data2 = Vector4::ZERO,
		const Vector4& data3 = Vector4::ZERO,
		const Vector4& data4 = Vector4::ZERO,
		const Vector4& data5 = Vector4::ZERO,
		const Vector4& data6 = Vector4::ZERO,
		const Vector2& objectCoord1 = Vector2::ZERO, const Vector2& objectCoord2 = Vector2::ZERO, const Vector2& objectCoord3 = Vector2::ZERO);

	void ApplyShaderParameters(ShaderParameters* shaderParameters) override;

	static const PODVector<VertexElement>& GetVertexElements();

	static UInt32 GetVertexElementsSize();

// shader parameters
	Vector4 vector_[8];

	PODVector<Matrix4> clipArray_;
};

}
