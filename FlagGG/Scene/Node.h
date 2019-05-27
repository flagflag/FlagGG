#pragma once

#include "Export.h"
#include "Container/RefCounted.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Scene/Component.h"
#include "Graphics/RenderContext.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"

namespace FlagGG
{
	namespace Scene
	{
		class FlagGG_API Node : public Container::RefCounted
		{
		public:
			Node();

			virtual void Update(float timeStep);

			virtual void Render(Container::Vector<Graphics::RenderContext>& renderContexts);

			void AddComponent(Component* component);

			void RemoveComponent(Component* component);

			void RemoveAllComponent();

			void AddChild(Node* node);

			void RemoveChild(Node* node);

			void RemoveFromParent();

			Container::Vector<Container::SharedPtr<Node>>& GetChildren();

			void SetPosition(const Math::Vector3& position);

			const Math::Vector3& GetPosition() const;

			void SetRotation(const Math::Quaternion& rotation);

			const Math::Quaternion& GetRotation() const;

			void SetScale(const Math::Vector3& scale);

			const Math::Vector3& GetScale() const;

			Math::Matrix3x4 GetTransform() const;

			Math::Matrix3x4 GetWorldTransform() const;

		protected:
			void UpdateWorldTransform() const;

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
