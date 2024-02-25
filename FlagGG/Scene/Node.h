#pragma once

#include "Export.h"
#include "Core/Object.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/StringHash.h"
#include "Container/Variant.h"
#include "Container/HashMap.h"
#include "Container/LinkedList.h"
#include "Graphics/RenderContext.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Scene/Component.h"

namespace FlagGG
{

// class Component;
class ITransformListener;
class IComponentEventListener;
class Scene;
class Octree;

struct NodeUpdateContext
{
	Real timeStep_;
	Scene* scene_;
	Octree* octree_;
};

class FlagGG_API Node : public Object
{
	OBJECT_OVERRIDE(Node, Object);
public:
	Node();

	~Node() override;

	virtual void Update(const NodeUpdateContext& updateContext);

	virtual void Render(PODVector<const RenderContext*>& renderContexts);

	void SetAttribute(const String& key, FVariant&& value);

	const FVariant& GetAttribute(const String& key) const;

	template < class ComponentType, class ... Args >
	ComponentType* CreateComponent(Args&& ... args)
	{
		SharedPtr<ComponentType> comp(new ComponentType(std::forward<Args>(args)...));
		AddComponent(comp);
		return comp;
	}

	void AddComponent(Component* component);

	void RemoveComponent(Component* component);

	template <class ComponentType>
	ComponentType* GetComponent()
	{
		return static_cast<ComponentType*>(GetComponent(ComponentType::GetTypeStatic()));
	}

	template <class ComponentType>
	ComponentType* GetComponentRecursive()
	{
		return static_cast<ComponentType*>(GetComponentRecursive(ComponentType::GetTypeStatic()));
	}

	Component* GetComponent(StringHash compClass);

	Component* GetComponentRecursive(StringHash compClass);

	void RemoveAllComponent();

	void AddTransformListener(LinkedListNode<ITransformListener>& listenerNode);

	void AddComponentEventListener(LinkedListNode<IComponentEventListener>& listenerNode);

	void AddChild(Node* node);

	void RemoveChild(Node* node);

	void RemoveFromParent();

	void RemoveAllChild();

	Node* GetChild(const String& name, bool recursive = false);

	Node* GetChild(StringHash nameHash, bool recursive = false);

	Node* GetParent() const;

	Vector<SharedPtr<Node>>& GetChildren();

	void SetName(const String& name);

	void SetTranspent(bool transpent);

	bool IsTranspent() const;

	void SetPosition(const Vector3& position);

	const Vector3& GetPosition() const;

	void SetRotation(const Quaternion& rotation);

	const Quaternion& GetRotation() const;

	void SetScale(const Vector3& scale);

	const Vector3& GetScale() const;

	void SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

	const String& GetName() const;

	StringHash GetNameHash() const;

	Matrix3x4 GetTransform() const;

	const Matrix3x4& GetWorldTransform() const;

	Vector3 GetWorldPosition() const;

	void SetWorldPosition(const Vector3& position);

	Quaternion GetWorldRotation() const;

	void SetWorldRotation(const Quaternion& rotation);

	Vector3 GetWorldScale() const;

	void SetWorldScale(const Vector3& scale);

	Vector3 GetWorldDirection() const;

	// Called by Scene
	void SetOwnerScene(Scene* scene);

	// 获取所属的场景
	Scene* GetOwnerScene() const { return ownerScene_; }

	// 更新节点属性结构dirty状态
	virtual void UpdateTreeDirty(Scene* scene);

	// 更新所有comgponent的dirty状态
	void UpdateComponentsDirty();

protected:
	void MarkTransformDirty();

	void UpdateTreeTransformDirty();

	void UpdateWorldTransform() const;

	// node名
	String name_;
	StringHash nameHash_;

	// 相对父节点的位置、旋转、缩放
	Vector3		position_;
	Quaternion	rotation_;
	Vector3		scale_;

	// 相对父节点的坐标变换
	mutable Matrix3x4		worldTransform_;
	mutable Quaternion      worldRotation_;
	mutable bool			dirty_;

private:
	Vector<SharedPtr<Component>> components_;

	Vector<SharedPtr<Node>> children_;

	Node* parent_;
	Scene* ownerScene_;

	LinkedList<LinkedListNode<ITransformListener>> transformListeners_;
	LinkedList<LinkedListNode<IComponentEventListener>> componentEventListeners_;

	bool isTranspent_;

	HashMap<String, FVariant> attribute_;
	static FVariant DEFAULT_VALUE;
};

}
