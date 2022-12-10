#pragma once

#include "GPUBuffer.h"
#include "Container/Vector.h"

namespace FlagGG
{

class FlagGG_API VertexBuffer : public GPUBuffer
{
public:
	UInt32 GetBindFlags() override;

	bool SetSize(UInt32 vertexCount, const PODVector<VertexElement>& vertexElements);

	UInt32 GetVertexSize() const;

	UInt32 GetVertexCount() const;

	const PODVector<VertexElement>& GetElements() const;

	static PODVector<VertexElement> GetElements(UInt32 elementMask);

	static UInt32 GetVertexSize(const PODVector<VertexElement>& elements);

protected:
	void UpdateOffset();

private:
	PODVector<VertexElement> vertexElements_;

	UInt32 vertexSize_{ 0 };
	UInt32 vertexCount_{ 0 };
};

}
