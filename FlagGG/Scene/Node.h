#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/StringHash.h"
#include "Graphics/RenderContext.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{
	namespace Scene
	{
		class Component;

		class FlagGG_API Node : public Container::RefCounted
		{
		public:
			Node();

			virtual void Update(float timeStep);

			virtual void Render(Container::PODVector<Graphics::RenderContext*>& renderContexts);

			template < class ComponentType, class ... Args >
			ComponentType* CreateComponent(Args ... args)
			{
				Container::SharedPtr<ComponentType> comp(new ComponentType(args...));
				AddComponent(comp);
				return comp;
			}

			void AddComponent(Component* component);

			void RemoveComponent(Component* component);

			template < class ComponentType >
			ComponentType* GetComponent()
			{
				return static_cast<ComponentType*>(GetComponent(ComponentType::StaticClass()));
			}

			Component* GetComponent(Container::StringHash compClass);

			void RemoveAllComponent();

			void AddChild(Node* node);

			void RemoveChild(Node* node);

			void RemoveFromParent();

			void RemoveAllChild();

			Node* GetChild(const Container::String& name, bool recursive = false);

			Node* GetChild(Container::StringHash nameHash, bool recursive = false);

			Container::Vector<Container::SharedPtr<Node>>& GetChildren();

			void SetName(const Container::String& name);

			void SetPosition(const Math::Vector3& position);

			const Math::Vector3& GetPosition() const;

			void SetRotation(const Math::Quaternion& rotation);

			const Math::Quaternion& GetRotation() const;

			void SetScale(const Math::Vector3& scale);

			const Math::Vector3& GetScale() const;

			void SetTransform(const Math::Vector3& position, const Math::Quaternion& rotation, const Math::Vector3& scale);

			const Container::String& GetName() const;

			Container::StringHash GetNameHash() const;

			Math::Matrix3x4 GetTransform() const;

			const Math::Matrix3x4& GetWorldTransform() const;

			Math::Vector3 GetWorldPosition() const;

			Math::Quaternion GetWorldRotation() const;

			Math::Vector3 GetWorldScale() const;

			// 更新节点属性结构dirty状态
			virtual void UpdateTreeDirty();

		protected:
			void UpdateWorldTransform() const;

			// node名
			Container::String name_;
			Container::StringHash nameHash_;

			// 相对父节点的位置、旋转、缩放
			Math::Vector3		position_;
			Math::Quaternion	rotation_;
			Math::Vector3		scale_;

			// 相对父节点的坐标变换
			mutable Math::Matrix3x4		worldTransform_;
			mutable bool				dirty_;

		private:
			Container::Vector<Container::SharedPtr<Component>> components_;

			Container::Vector<Container::SharedPtr<Node>> children_;

			Node* parent_;
		};
	}
}
