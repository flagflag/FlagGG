#include "Scene/Node.h"
#include "Scene/Component.h"

namespace FlagGG
{
	namespace Scene
	{
		Node::Node() :
			parent_(nullptr),
			dirty_(true),
			position_(Math::Vector3::ZERO),
			rotation_(Math::Quaternion::IDENTITY),
			scale_(Math::Vector3::ONE),
			worldTransform_(Math::Matrix3x4::IDENTITY)
		{ }

		void Node::Update(float timeStep)
		{
			if (dirty_)
				UpdateWorldTransform();

			for (const auto& compoment : components_)
			{
				compoment->Update(timeStep);
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
		}

		Component* Node::GetComponent(Container::StringHash compClass)
		{
			for (const auto& comp : components_)
			{
				if (comp->Class() == compClass)
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

		Container::Vector<Container::SharedPtr<Node>>& Node::GetChildren()
		{
			return children_;
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

		Math::Quaternion Node::GetWorldRotation() const
		{
			if (dirty_)
				UpdateWorldTransform();
			return worldTransform_.Rotation();
		}

		Math::Vector3 Node::GetWorldScale() const
		{
			if (dirty_)
				UpdateWorldTransform();
			return worldTransform_.Scale();
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
