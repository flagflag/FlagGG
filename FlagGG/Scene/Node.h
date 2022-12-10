#pragma once

#include "Export.h"
#include "Core/Object.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Container/StringHash.h"
#include "Container/Variant.h"
#include "Container/HashMap.h"
#include "Graphics/RenderContext.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Scene/Component.h"

namespace FlagGG
{
	namespace Scene
	{
		// class Component;
		class Scene;
		class Octree;

		struct NodeUpdateContext
		{
			Real timeStep_;
			Scene* scene_;
			Octree* octree_;
		};

		class FlagGG_API Node : public Core::Object
		{
			OBJECT_OVERRIDE(Node, Object);
		public:
			Node();

			virtual void Update(const NodeUpdateContext& updateContext);

			virtual void Render(Container::PODVector<Graphics::RenderContext*>& renderContexts);

			void SetAttribute(const Container::String& key, Container::FVariant&& value);

			const Container::FVariant& GetAttribute(const Container::String& key) const;

			template < class ComponentType, class ... Args >
			ComponentType* CreateComponent(Args&& ... args)
			{
				Container::SharedPtr<ComponentType> comp(new ComponentType(std::forward<Args>(args)...));
				AddComponent(comp);
				return comp;
			}

			void AddComponent(Component* component);

			void RemoveComponent(Component* component);

			template < class ComponentType >
			ComponentType* GetComponent()
			{
				return static_cast<ComponentType*>(GetComponent(ComponentType::GetTypeStatic()));
			}

			Component* GetComponent(Container::StringHash compClass);

			void RemoveAllComponent();

			void AddChild(Node* node);

			void RemoveChild(Node* node);

			void RemoveFromParent();

			void RemoveAllChild();

			Node* GetChild(const Container::String& name, bool recursive = false);

			Node* GetChild(Container::StringHash nameHash, bool recursive = false);

			Node* GetParent() const;

			Container::Vector<Container::SharedPtr<Node>>& GetChildren();

			void SetName(const Container::String& name);

			void SetTranspent(bool transpent);

			bool IsTranspent() const;

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

			void SetWorldPosition(const Math::Vector3& position);

			Math::Quaternion GetWorldRotation() const;

			void SetWorldRotation(const Math::Quaternion& rotation);

			Math::Vector3 GetWorldScale() const;

			void SetWorldScale(const Math::Vector3& scale);

			Math::Vector3 GetWorldDirection() const;

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

			bool isTranspent_;

			Container::HashMap<Container::String, Container::FVariant> attribute_;
			static Container::FVariant DEFAULT_VALUE;
		};
	}
}
