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

	// 设置节点属性
	void SetAttribute(const String& key, FVariant&& value);

	// 获取节点属性
	const FVariant& GetAttribute(const String& key) const;

	// 创建组件
	template < class ComponentType, class ... Args >
	ComponentType* CreateComponent(Args&& ... args)
	{
		SharedPtr<ComponentType> comp(new ComponentType(std::forward<Args>(args)...));
		AddComponent(comp);
		return comp;
	}

	// 增加组件
	void AddComponent(Component* component);

	// 移除组件
	void RemoveComponent(Component* component);

	// 获取组件
	template <class ComponentType>
	ComponentType* GetComponent()
	{
		return static_cast<ComponentType*>(GetComponent(ComponentType::GetTypeStatic()));
	}

	// 递归节点树获取组件
	template <class ComponentType>
	ComponentType* GetComponentRecursive()
	{
		return static_cast<ComponentType*>(GetComponentRecursive(ComponentType::GetTypeStatic()));
	}

	// 通过Class获取组件
	Component* GetComponent(StringHash compClass);

	// 递归节点树通过Class获取组件
	Component* GetComponentRecursive(StringHash compClass);

	// 移除所有组件
	void RemoveAllComponent();

	// 增加节点位置变化的监听（由组件调用，通知组件）
	void AddTransformListener(LinkedListNode<ITransformListener>& listenerNode);

	// 增加组件事件监听（有组件调用，通知组件）
	void AddComponentEventListener(LinkedListNode<IComponentEventListener>& listenerNode);

	// 增加孩子节点
	void AddChild(Node* node);

	// 移除孩子节点
	void RemoveChild(Node* node);

	// 从父节点移除自己
	void RemoveFromParent();

	// 移除所有孩子节点
	void RemoveAllChild();

	// 通过名字获取孩子节点
	Node* GetChild(const String& name, bool recursive = false);

	// 通过名字哈希获取孩子节点
	Node* GetChild(StringHash nameHash, bool recursive = false);

	// 获取父节点
	Node* GetParent() const;

	// 获取所有孩子节点
	Vector<SharedPtr<Node>>& GetChildren();

	// 设置名字
	void SetName(const String& name);

	// 抛弃！！！！
	void SetTranspent(bool transpent);

	// 抛弃！！！！
	bool IsTranspent() const;

	// 设置节点本地位置
	void SetPosition(const Vector3& position);

	// 获取节点本地位置
	const Vector3& GetPosition() const;

	// 设置节点本地旋转
	void SetRotation(const Quaternion& rotation);

	// 获取节点本地旋转
	const Quaternion& GetRotation() const;

	// 设置节点本地缩放
	void SetScale(const Vector3& scale);

	// 获取节点本地缩放
	const Vector3& GetScale() const;

	// 设置节点本地朝向
	void SetDirection(const Vector3& direction);

	// 获取节点本地朝向
	Vector3 GetDirection() const;

	// 设置节点本地位置、旋转、缩放
	void SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

	// 设置节点本地位置（静默修改，无任何通知）
	void SetPositionSilent(const Vector3& position);

	// 设置节点本地旋转（静默修改，无任何通知）
	void SetRotationSilent(const Quaternion& rotation);

	// 设置节点本地缩放（静默修改，无任何通知）
	void SetScaleSilent(const Vector3& scale);

	// 获取节点名字
	const String& GetName() const;

	// 获取节点名字哈希
	StringHash GetNameHash() const;

	// 获取节点本地Transform
	Matrix3x4 GetTransform() const;

	// 获取节点世界Transform
	const Matrix3x4& GetWorldTransform() const;

	// 获取节点世界位置
	Vector3 GetWorldPosition() const;

	// 设置节点世界位置
	void SetWorldPosition(const Vector3& position);

	// 获取节点世界旋转
	const Quaternion& GetWorldRotation() const;

	// 设置节点世界旋转
	void SetWorldRotation(const Quaternion& rotation);

	// 获取节点世界缩放
	Vector3 GetWorldScale() const;

	// 设置节点世界缩放
	void SetWorldScale(const Vector3& scale);

	// 获取节点世界朝向
	Vector3 GetWorldDirection() const;

	// Called by Scene
	void SetOwnerScene(Scene* scene);

	// 获取所属的场景
	Scene* GetOwnerScene() const { return ownerScene_; }

	// 更新节点属性结构dirty状态
	virtual void UpdateTreeDirty(Scene* scene);
	
	// 标脏
	void MarkDirty();

protected:
	// 标脏Transform
	void MarkTransformDirty();

	// 标脏节点树Transform
	void UpdateTreeTransformDirty();

	// 更新所有comgponent的dirty状态
	void UpdateComponentsDirty();

	// 更新世界Transform
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
	// 组件
	Vector<SharedPtr<Component>> components_;

	// 孩子节点
	Vector<SharedPtr<Node>> children_;

	// 父节点
	Node* parent_;
	// 所属场景
	Scene* ownerScene_;

	// Transform事件监听列表
	LinkedList<LinkedListNode<ITransformListener>> transformListeners_;
	// 组件事件监听列表
	LinkedList<LinkedListNode<IComponentEventListener>> componentEventListeners_;

	// 抛弃！！！！
	bool isTranspent_;

	// 节点属性
	HashMap<String, FVariant> attribute_;

	// 节点属性默认值
	static FVariant DEFAULT_VALUE;
};

}
