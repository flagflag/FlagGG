//
// UI系统
//

#pragma once

#include "Core/Subsystem.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"
#include "Graphics/Batch.h"
#include "Math/Rect.h"

namespace FlagGG
{

class UIElement;
class Texture;
class Texture2D;
class Batch;
class VertexBuffer;
class ShaderCode;
class Shader;
class GfxRenderSurface;
struct RenderUITree;
struct UIBatchRenderData;
class ShaderParameters;

class FlagGG_API UISystem : public Subsystem<UISystem>
{
public:
	UISystem();

	~UISystem() override;

	void Initialize();

	void Render(UIElement* uiElement);

	void RenderWebKit(GfxRenderSurface* renderSurface, const Rect& viewport, const Vector<SharedPtr<Batch>>& uiBatches);

	void HandleRenderUpdate(Real timeStep);

protected:
	void UpdateBatches(UIElement* element, VertexVector* vertexVector, Vector<SharedPtr<Batch>>& batches);

private:
	// 默认贴图
	SharedPtr<Texture2D> defaultTexture_;

	// 渲染ui树
	Vector<RenderUITree> renderUITreesCache_;

	// 渲染数据
	HashMap<SharedPtr<UIElement>, UIBatchRenderData> batchRenderDataMap_;

	// 材质参数
	SharedPtr<ShaderParameters> materialParameters_;
};

}
