#pragma once

#include "Export.h"
#include "Scene/Component.h"
#include "Container/Ptr.h"
#include "Graphics/Material.h"
#include "Resource/Image.h"
#include "Math/Vector2.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{

class FlagGG_API TerrainComponent : public Component
{
	OBJECT_OVERRIDE(TerrainComponent, Component);
public:
	void SetPathSize(UInt32 pathSize);

	void SetHeightMap(Image* image);

	void SetMaterial(Material* material);

	bool IsDrawable() override;

	RenderContext* GetRenderContext() override;

protected:
	void CreateGeometry();

private:
	UInt32 pathSize_;

	IntVector2 patchesNum_;
	IntVector2 verticesNum_;
	Vector2 patchWorldSize_;
	Vector2 patchWorldOrigin_;
	Matrix3x4 indentity_;

	SharedPtr<Image> heightMap_;
	SharedPtr<Material> material_;
	SharedPtr<Geometry> geometry_;
	SharedPtr<VertexBuffer> vertexBuffer_;
	SharedPtr<IndexBuffer> indexBuffer_;

	RenderContext renderContext_;
};

}
