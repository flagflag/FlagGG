#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{

class FlagGG_API DynamicMeshComponent : public Component
{
	OBJECT_OVERRIDE(DynamicMeshComponent, Component);
public:
	DynamicMeshComponent();

	void SetSize(UInt32 width, UInt32 height);

	void SetElementSize(Real elementSize);

	void SetMaterial(Material* material);

	bool IsDrawable() override;

	RenderContext* GetRenderContext() override;

	void Update(Real timeStep) override;

	void SetViewMask(UInt32 viewMask) override;

	virtual Real GetHeight(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

	virtual Vector3 GetNormal(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

	virtual Vector2 GetUV(Int32 x, Int32 y, Int32 verticesNumX, Int32 verticesNumY);

	virtual const PODVector<VertexElement>& GetVertexElement();

protected:
	void CreateGeometry();

private:
	UInt32 width_;
	UInt32 height_;
	Real elementSize_;

	SharedPtr<Material> material_;
	SharedPtr<Geometry> geometry_;
	SharedPtr<VertexBuffer> vertexBuffer_;
	SharedPtr<IndexBuffer> indexBuffer_;

	RenderContext renderContext_;

	bool geometryDirty_;
	bool hasGeometry_;
};

}
