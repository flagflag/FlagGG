#include "Scene/Node.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"
#include "Scene/Octree.h"
#include "Scene/DrawableComponent.h"
#include "Scene/TransformComponent.h"
#include "Scene/ComponentEventListener.h"

namespace FlagGG
{

Node::Node() :
	name_(""),
	nameHash_(0u),
	isTranspent_(false),
	parent_(nullptr),
	ownerScene_(nullptr),
	dirty_(true),
	position_(Vector3::ZERO),
	rotation_(Quaternion::IDENTITY),
	scale_(Vector3::ONE),
	worldTransform_(Matrix3x4::IDENTITY)
{
}

Node::~Node()
{
	RemoveAllComponent();
	RemoveAllChild();
	RemoveFromParent();
}

void Node::Update(const NodeUpdateContext& updateContext)
{
	if (dirty_)
		UpdateWorldTransform();

	for (const auto& compoment : components_)
	{
		compoment->Update(updateContext.timeStep_);
	}
}

void Node::Render(PODVector<const RenderContext*>& renderContexts)
{
	for (const auto& compoment : components_)
	{
		if (auto* drawableComponent = compoment->Cast<DrawableComponent>())
		{
			if (drawableComponent->IsRenderable())
			{
				const Vector<RenderContext>& _renderContexts = drawableComponent->GetRenderContext();
				for (auto& renderContext : _renderContexts)
				{
					renderContexts.Push(&renderContext);
				}
			}
		}
	}
}

void Node::SetAttribute(const String& key, FVariant&& value)
{
	attribute_[key] = std::forward<FVariant>(value);
}

FVariant Node::DEFAULT_VALUE;

const FVariant& Node::GetAttribute(const String& key) const
{
	auto it = attribute_.Find(key);
	if (it == attribute_.End())
		return DEFAULT_VALUE;
	return it->second_;
}

void Node::AddComponent(Component* component)
{
	SharedPtr<Component> sharedComponent(component);
	for (const auto& comp : components_)
	{
		if (comp == sharedComponent)
		{
			return;
		}
	}
	components_.Push(sharedComponent);
	sharedComponent->SetNode(this);

	sharedComponent->OnAddToNode(this);
	for (auto& it : componentEventListeners_)
	{
		it->OnGlobalAddToNode(sharedComponent);
	}

	if (ownerScene_)
	{
		ownerScene_->OnAddToScene(this, component);
		for (auto& it : componentEventListeners_)
		{
			it->OnGlobalAddToScene(ownerScene_, component);
		}
	}

	sharedComponent->UpdateTreeDirty();
}

Component* Node::GetComponent(StringHash compClass)
{
	for (const auto& comp : components_)
	{
		if (comp->IsInstanceOf(compClass))
		{
			return comp;
		}
	}
	return nullptr;
}

Component* Node::GetComponentRecursive(StringHash compClass)
{
	auto* comp = GetComponent(compClass);
	if (comp)
		return comp;

	for (auto& child : children_)
	{
		auto* comp = child->GetComponentRecursive(compClass);
		if (comp)
			return comp;
	}

	return nullptr;
}

void Node::RemoveComponent(Component* component)
{
	SharedPtr<Component> sharedComponent(component);
	components_.Remove(sharedComponent);

	sharedComponent->OnRemoveFromNode(this);
	for (auto& it : componentEventListeners_)
	{
		it->OnGlobalRemoveFromNode(sharedComponent);
	}

	if (ownerScene_)
	{
		ownerScene_->OnRemoveFromScene(this, sharedComponent);
		for (auto& it : componentEventListeners_)
		{
			it->OnGlobalRemoveFromScene(ownerScene_, sharedComponent);
		}
	}
}

void Node::RemoveAllComponent()
{
	if (ownerScene_)
	{
		while (components_.Size())
		{
			SharedPtr<Component> sharedComponent(components_.Back());
			components_.Pop();

			sharedComponent->OnRemoveFromNode(this);
			for (auto& it : componentEventListeners_)
			{
				it->OnGlobalRemoveFromNode(sharedComponent);
			}

			ownerScene_->OnRemoveFromScene(this, sharedComponent);
			for (auto& it : componentEventListeners_)
			{
				it->OnGlobalRemoveFromScene(ownerScene_, sharedComponent);
			}
		}
	}
	else
	{
		components_.Clear();
	}
}

void Node::AddTransformListener(LinkedListNode<ITransformListener>& listenerNode)
{
	transformListeners_.Push(listenerNode);
}

void Node::AddComponentEventListener(LinkedListNode<IComponentEventListener>& listenerNode)
{
	componentEventListeners_.Push(listenerNode);
}

void Node::AddChild(Node* node)
{
	SharedPtr<Node> sharedNode(node);
	for (const auto& child : children_)
	{
		if (child == node)
		{
			return;
		}
	}
	if (node->parent_)
	{
		node->RemoveFromParent();
	}
	children_.Push(sharedNode);
	node->parent_ = this;
	node->UpdateTreeDirty(ownerScene_);
}

void Node::RemoveChild(Node* node)
{
	SharedPtr<Node> sharedNode(node);
	children_.Remove(sharedNode); 
	node->parent_ = nullptr;
	node->UpdateTreeDirty(nullptr);
}

void Node::RemoveFromParent()
{
	if (parent_)
	{
		parent_->RemoveChild(this);
		parent_ = nullptr;
	}
}

void Node::RemoveAllChild()
{
	for (const auto& child : children_)
	{
		child->parent_ = nullptr;
		child->UpdateTreeDirty(nullptr);
	}
	children_.Clear();
}

Node* Node::GetChild(const String& name, bool recursive)
{
	return GetChild(StringHash(name), recursive);
}

Node* Node::GetChild(StringHash nameHash, bool recursive)
{
	for (const auto& child : children_)
	{
		if (child->GetNameHash() == nameHash)
		{
			return child;
		}
	}

	if (recursive)
	{
		for (const auto& child : children_)
		{
			Node* node = child->GetChild(nameHash, true);
			if (node)
			{
				return node;
			}
		}
	}

	return nullptr;
}

Node* Node::GetParent() const
{
	return parent_;
}

Vector<SharedPtr<Node>>& Node::GetChildren()
{
	return children_;
}

void Node::SetName(const String& name)
{
	name_ = name;
	nameHash_ = name_;
}

const String& Node::GetName() const
{
	return name_;
}

void Node::SetTranspent(bool transpent)
{
	isTranspent_ = transpent;
}

bool Node::IsTranspent() const
{
	return isTranspent_;
}

StringHash Node::GetNameHash() const
{
	return nameHash_;
}

void Node::SetPosition(const Vector3& position)
{
	position_ = position;

	UpdateTreeTransformDirty();

	for (auto& it : transformListeners_)
	{
		it->OnPositionChange();
	}
}

const Vector3& Node::GetPosition() const
{
	return position_;
}

void Node::SetRotation(const Quaternion& rotation)
{
	rotation_ = rotation;

	UpdateTreeTransformDirty();

	for (auto& it : transformListeners_)
	{
		it->OnRotationChange();
	}
}

const Quaternion& Node::GetRotation() const
{
	return rotation_;
}

void Node::SetScale(const Vector3& scale)
{
	scale_ = scale;

	UpdateTreeTransformDirty();

	for (auto& it : transformListeners_)
	{
		it->OnScaleChange();
	}
}

const Vector3& Node::GetScale() const
{
	return scale_;
}

void Node::SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	position_ = position;
	rotation_ = rotation;
	scale_ = scale;

	UpdateTreeTransformDirty();

	for (auto& it : transformListeners_)
	{
		it->OnTransformChange();
	}
}

Matrix3x4 Node::GetTransform() const
{
	return Matrix3x4(position_, rotation_, scale_);
}

const Matrix3x4& Node::GetWorldTransform() const
{
	if (dirty_)
		UpdateWorldTransform();
	return worldTransform_;
}

Vector3 Node::GetWorldPosition() const
{
	if (dirty_)
		UpdateWorldTransform();
	return worldTransform_.Translation();
}

void Node::SetWorldPosition(const Vector3& position)
{
	SetPosition(!parent_ ? position : parent_->GetWorldTransform().Inverse() * position);
}

Quaternion Node::GetWorldRotation() const
{
	if (dirty_)
		UpdateWorldTransform();
	return worldRotation_;
}

void Node::SetWorldRotation(const Quaternion& rotation)
{
	SetRotation(!parent_ ? rotation : parent_->GetWorldRotation().Inverse() * rotation);
}

Vector3 Node::GetWorldScale() const
{
	if (dirty_)
		UpdateWorldTransform();
	return worldTransform_.Scale();
}

void Node::SetWorldScale(const Vector3& scale)
{
	SetScale(!parent_ ? scale : scale / parent_->GetWorldScale());
}

Vector3 Node::GetWorldDirection() const
{
	if (dirty_)
		UpdateWorldTransform();
	return worldRotation_ * Vector3::FORWARD;
}

void Node::UpdateWorldTransform() const
{
	Matrix3x4 transform = GetTransform();
			
	if (!parent_)
	{
		worldTransform_ = transform;
		worldRotation_ = rotation_;
	}
	else
	{
		worldTransform_ = parent_->GetWorldTransform() * transform;
		worldRotation_ = parent_->GetWorldRotation() * rotation_;
	}

	dirty_ = false;
}

void Node::SetOwnerScene(Scene* scene)
{
	ownerScene_ = scene;
}

void Node::MarkTransformDirty()
{
	dirty_ = true;
}

void Node::UpdateTreeTransformDirty()
{
	MarkTransformDirty();

	for (auto& child : children_)
	{
		for (auto& it : child->transformListeners_)
		{
			it->OnTransformChange();
		}

		child->UpdateTreeTransformDirty();
	}
}

void Node::UpdateComponentsDirty()
{
	for (auto& component : components_)
	{
		component->UpdateTreeDirty();
	}
}

void Node::UpdateTreeDirty(Scene* scene)
{
	MarkTransformDirty();

	for (auto& it : transformListeners_)
	{
		it->OnTransformChange();
	}

	for (auto& component : components_)
	{
		component->UpdateTreeDirty();
		if (scene)
		{
			scene->OnAddToScene(this, component);
			for (auto& it : componentEventListeners_)
			{
				it->OnGlobalAddToScene(scene, component);
			}
		}
		else if (ownerScene_)
		{
			ownerScene_->OnRemoveFromScene(this, component);
			for (auto& it : componentEventListeners_)
			{
				it->OnGlobalRemoveFromScene(ownerScene_, component);
			}
		}
	}
			
	for (auto& child : children_)
	{
		child->UpdateTreeDirty(scene);
	}

	SetOwnerScene(scene);
}

}
