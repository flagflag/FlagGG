//
// 渲染对象控件
//

#pragma once

#include "Scene/Component.h"
#include "Graphics/RenderContext.h"
#include "Container/Vector.h"

namespace FlagGG
{

struct OctreeNode;

struct HiZVisibilityTestInfo : public RefCounted
{
	// 帧号
	UInt32 frameNumber_;
	// 裁剪测试后的可见性
	bool visible_;
};

class FlagGG_API DrawableComponent : public Component
{
	OBJECT_OVERRIDE(DrawableComponent, Component);
public:
	~DrawableComponent() override;

	// 控件树发生dirty
	void UpdateTreeDirty() override;

	// 插入场景时调用
	void OnAddToScene(Scene* scene) override;

	// 从场景删除是调用
	void OnRemoveFromScene(Scene* scene) override;

	// 是否可渲染
	virtual bool IsRenderable() { return false; }

	// 获取DrawableFlags
	virtual UInt32 GetDrawableFlags() const { return DRAWABLE_UNDEFINED; }

	// 更新包围盒
	virtual void OnUpdateWorldBoundingBox() {}

	// 设置查询
	virtual void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);

	// 设置octree node
	void SetOcNode(OctreeNode* ocnode);

	// 获取octree node
	OctreeNode* GetOcNode() { return ocnode_; }

	// 获取所属的场景
	Scene* GetOwnerScene() const { return ownerScene_; }

	// 获取世界空间下的包围盒
	const BoundingBox& GetWorldBoundingBox();

	// 设置阴影投射
	void SetCastShadow(bool castShadow);

	// 获取阴影投射
	bool GetCastShadows() const { return castShadow_; }

	void SetHasLitPass(bool hasLitPass);

	bool GetHasLitPass() const { return hasLitPass_; }

	// 设置遮挡裁剪
	void SetOcclusionCulling(bool occlusionCulling);

	// 获取遮挡裁剪
	bool GetOcclusionCulling() const { return occlusionCulling_; }

	// 获取渲染上下文
	const Vector<RenderContext>& GetRenderContext() const { return renderContexts_; }

protected:
	// 设置Hi-Z可见性测试信息
	void SetHiZVisibilityTestInfo(HiZVisibilityTestInfo* HiZTestInfo);

	// 获取Hi-Z可见性测试信息
	HiZVisibilityTestInfo* GetHiZVisibilityTestInfo() const { return HiZTestInfo_; }

	friend class HiZCulling;

protected:
	BoundingBox worldBoundingBox_;

	bool worldBoundingBoxDirty_{ true };

	OctreeNode* ocnode_{};

	Scene* ownerScene_{};

	bool castShadow_{};

	Vector<RenderContext> renderContexts_;

	bool hasLitPass_{};

	// 是否需要遮挡裁剪
	bool occlusionCulling_{ true };

	// Hi-Z可见性测试信息
	SharedPtr<HiZVisibilityTestInfo> HiZTestInfo_;
};

}
