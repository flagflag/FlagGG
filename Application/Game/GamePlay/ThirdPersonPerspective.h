#pragma once

#include "Perspective.h"
#include "Network/NetworkAdaptor.h"

#include <Core/Context.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

using namespace FlagGG::Core;
using namespace FlagGG::Math;

class ThirdPersonPerspective : public Perspective
{
public:
	explicit ThirdPersonPerspective(Context* context);

	// 设置相机
	void SetCamera(Camera* camera) override;
	Camera* GetCamera() const override;

	// 设置所属窗口
	void SetWindow(Window* window) override;
	Window* GetWindow() const override;

	// 设置控制对象
	void SetNode(Node* node) override;
	Node* GetNode() const override;

	// 设置同步模式
	void SetSyncMode(SyncMode syncMode) override;
	SyncMode GetSyncMode() const override;

	// 重置视角
	void Reset() override;

	// 锁定视角
	void Lock() override;

	// 解锁视角
	void Unlock() override;

protected:
	void OnMouseMove(KeyState* keyState, const Vector2& delta);
	void HandleUpdate(float timeStep);

private:
	WeakPtr<Camera> camera_;
	WeakPtr<Window> window_;
	WeakPtr<Node> node_;
	// 控制方向和左右视角的相机
	SharedPtr<Camera> controlCamera_;
	SharedPtr<Node> lookupNode_;
	// 控制上下视角的相机
	SharedPtr<Camera> lookupCamera_;

	// 镜头锁标记
	bool isLocked_;

	WeakPtr<Network> network_;

	// 同步模式
	SyncMode syncMode_;
	int dir_[4];
	Quaternion rotation_[2][2][2][2];
	Quaternion stop_;
	Quaternion currentRot_;
};
