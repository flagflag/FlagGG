#pragma once

#include "Perspective.h"

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

	// �������
	void SetCamera(Camera* camera) override;
	Camera* GetCamera() const override;

	// ������������
	void SetWindow(Window* window) override;
	Window* GetWindow() const override;

	// ���ÿ��ƶ���
	void SetNode(Node* node) override;
	Node* GetNode() const override;

	// �����ӽ�
	void Reset() override;

	// �����ӽ�
	void Lock() override;

	// �����ӽ�
	void Unlock() override;

	void OnMouseMove(KeyState* keyState, const Vector2& delta);
	void HandleUpdate(float timeStep);

private:
	WeakPtr<Camera> camera_;
	WeakPtr<Window> window_;
	WeakPtr<Node> node_;
	// ���Ʒ���������ӽǵ����
	SharedPtr<Camera> controlCamera_;
	SharedPtr<Node> lookupNode_;
	// ���������ӽǵ����
	SharedPtr<Camera> lookupCamera_;

	// ��ͷ�����
	bool isLocked_;
};
