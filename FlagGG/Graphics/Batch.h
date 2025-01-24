#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class Texture;

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

	BatchType GetType() const;

protected:
	VertexVector* vertexs_;
	bool owner_;

	UInt32 vertexSize_;
			
	UInt32 vertexStart_;

	UInt32 vertexEnd_;

	SharedPtr<Texture> texture_;

	BatchType type_;
};

}
