#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Core/BaseTypes.h"
#include "Graphics/GraphicsDef.h"
#include "Math/Rect.h"

namespace FlagGG
{

class Texture;
class ShaderParameters;

typedef Vector<Real> VertexVector;

enum BatchType
{
	DRAW_LINE = 0,
	DRAW_TRIANGLE = 1,
};

class FlagGG_API Batch : public RefCounted
{
public:
	Batch(BatchType type, VertexVector* vertexs, USize vertexSize, Texture* texture);

	virtual ~Batch();

	const char* GetVertexs() const;

	UInt32 GetVertexStart() const { return vertexStart_; }

	UInt32 GetVertexSize() const { return vertexSize_; }

	UInt32 GetVertexCount() const;

	Texture* GetTexture() const { return texture_; }

	void SetTexture(Texture* texture);

	BlendMode GetBlendMode() const { return blendMode_; }

	void SetBlendMode(BlendMode blendMode);

	const IntRect& GetScissorRect() const { return scissorRect_; }

	void SetScissorRect(const IntRect& scissorRect);

	BatchType GetType() const;

	virtual void ApplyShaderParameters(ShaderParameters* shaderParameters) {}

protected:
	VertexVector* vertexs_;
	bool owner_;

	UInt32 vertexSize_;
			
	UInt32 vertexStart_;

	UInt32 vertexEnd_;

	SharedPtr<Texture> texture_;

	BlendMode blendMode_;

	IntRect scissorRect_;

	BatchType type_;
};

}
