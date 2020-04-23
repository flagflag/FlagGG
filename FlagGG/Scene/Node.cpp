#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Octree.h"

namespace FlagGG
{
	namespace Scene
	{
		Node::Node() :
			name_(""),
			nameHash_(0u),
			isTranspent_(false),
			parent_(nullptr),
			dirty_(true),
			position_(Math::Vector3::ZERO),
			rotation_(Math::Quaternion::IDENTITY),
			scale_(Math::Vector3::ONE),
			worldTransform_(Math::Matrix3x4::IDENTITY)
		{ }

		void Node::Update(const NodeUpdateContext& updateContext)
		{
			if (dirty_)
				UpdateWorldTransform();

			for (const auto& compoment : components_)
			{
				compoment->Update(updateContext.timeStep_);

				if (compoment->IsDrawable() && !IsTranspent())
				{
					updateContext.octree_->InsertElement(compoment);
				}
			}
		}

		void Node::Render(Container::PODVector<Graphics::RenderContext*>& renderContexts)
		{
			for (const auto& compoment : components_)
			{
				if (compoment->IsDrawable())
				{
					Graphics::RenderContext* renderContext = compoment->GetRenderContext();
					renderContexts.Push(renderContext);
				}
			}
		}

		void Node::SetAttribute(const Container::String& key, Container::FVariant&& value)
		{
			attribute_[key] = std::forward<Container::FVariant>(value);
		}

		Container::FVariant Node::DEFAULT_VALUE;

		const Container::FVariant& Node::GetAttribute(const Container::String& key) const
		{
			auto it = attribute_.Find(key);
			if (it == attribute_.End())
				return DEFAULT_VALUE;
			return it->second_;
		}

		void Node::AddComponent(Component* component)
		{
			Container::SharedPtr<Component> sharedComponent(component);
			for (const auto& comp : components_)
			{
				if (comp == sharedComponent)
				{
					return;
				}
			}
			components_.Push(sharedComponent);
			sharedComponent->SetNode(this);
			sharedComponent->UpdateTreeDirty();
		}

		Component* Node::GetComponent(Container::StringHash compClass)
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

		void Node::RemoveComponent(Component* component)
		{
			components_.Remove(Container::SharedPtr<Component>(component));
		}

		void Node::RemoveAllComponent()
		{
			components_.Clear();
		}

		void Node::AddChild(Node* node)
		{
			Container::SharedPtr<Node> sharedNode(node);
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
			
			// 更新dirty，类似的会去更新直接矩阵等
			node->UpdateTreeDirty();
		}

		void Node::RemoveChild(Node* node)
		{
			Container::SharedPtr<Node> sharedNode(node);
			children_.Remove(sharedNode); 
			node->parent_ = nullptr;
			node->UpdateTreeDirty();
		}

		void Node::RemoveFromParent()
		{
			if (parent_)
			{
				parent_->RemoveChild(this);
			}
		}

		void Node::RemoveAllChild()
		{
			for (const auto& child : children_)
			{
				child->parent_ = nullptr;
				child->UpdateTreeDirty();
			}
			children_.Clear();
		}

		Node* Node::GetChild(const Container::String& name, bool recursive)
		{
			return GetChild(Container::StringHash(name), recursive);
		}

		Node* Node::GetChild(Container::StringHash nameHash, bool recursive)
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

		Container::Vector<Container::SharedPtr<Node>>& Node::GetChildren()
		{
			return children_;
		}

		void Node::SetName(const Container::String& name)
		{
			name_ = name;
			nameHash_ = name_;
		}

		const Container::String& Node::GetName() const
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

		Container::StringHash Node::GetNameHash() const
		{
			return nameHash_;
		}

		void Node::SetPosition(const Math::Vector3& position)
		{
			position_ = position;

			UpdateTreeDirty();
		}

		const Math::Vector3& Node::GetPosition() const
		{
			return position_;
		}

		void Node::SetRotation(const Math::Quaternion& rotation)
		{
			rotation_ = rotation;

			UpdateTreeDirty();
		}

		const Math::Quaternion& Node::GetRotation() const
		{
			return rotation_;
		}

		void Node::SetScale(const Math::Vector3& scale)
		{
			scale_ = scale;

			UpdateTreeDirty();
		}

		const Math::Vector3& Node::GetScale() const
		{
			return scale_;
		}

		void Node::SetTransform(const Math::Vector3& position, const Math::Quaternion& rotation, const Math::Vector3& scale)
		{
			position_ = position;
			rotation_ = rotation;
			scale_ = scale;

			UpdateTreeDirty();
		}

		Math::Matrix3x4 Node::GetTransform() const
		{
			return Math::Matrix3x4(position_, rotation_, scale_);
		}

		const Math::Matrix3x4& Node::GetWorldTransform() const
		{
			if (dirty_)
				UpdateWorldTransform();
			return worldTransform_;
		}

		Math::Vector3 Node::GetWorldPosition() const
		{
			if (dirty_)
				UpdateWorldTransform();
			return worldTransform_.Translation();
		}

		void Node::SetWorldPosition(const Math::Vector3& position)
		{
			SetPosition(!parent_ ? position : parent_->GetWorldTransform().Inverse() * position);
		}

		Math::Quaternion Node::GetWorldRotation() const
		{
			if (dirty_)
				UpdateWorldTransform();
			return worldTransform_.Rotation();
		}

		void Node::SetWorldRotation(const Math::Quaternion& rotation)
		{
			SetRotation(!parent_ ? rotation : parent_->GetWorldRotation().Inverse() * rotation);
		}

		Math::Vector3 Node::GetWorldScale() const
		{
			if (dirty_)
				UpdateWorldTransform();
			return worldTransform_.Scale();
		}

		void Node::SetWorldScale(const Math::Vector3& scale)
		{
			SetScale(!parent_ ? scale : scale / parent_->GetWorldScale());
		}

		Math::Vector3 Node::GetWorldDirection() const
		{
			return GetWorldPosition() * Math::Vector3::FORWARD;
		}

		void Node::UpdateWorldTransform() const
		{
			Math::Matrix3x4 transform = GetTransform();
			
			if (!parent_)
			{
				worldTransform_ = transform;
			}
			else
			{
				worldTransform_ = parent_->GetWorldTransform() * transform;
			}

			dirty_ = false;
		}

		void Node::UpdateTreeDirty()
		{
			if (dirty_) return;

			dirty_ = true;

			for (auto& component : components_)
			{
				component->UpdateTreeDirty();
			}
			
			for (auto& child : children_)
			{
				child->UpdateTreeDirty();
			}
		}
	}
}
