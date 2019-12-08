#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Container/Ptr.h"
#include "Container/StringHash.h"
#include "Container/FlagSet.h"
#include "Container/Vector.h"
#include "Scene/Node.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/BoundingBox.h"
#include "IOFrame/Buffer/IOBuffer.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Scene
	{
		enum BoneCollisionShape : uint8_t
		{
			BONE_COLLISSION_NONE = 0,
			BONE_COLLISSION_SPHERE = 1,
			BONE_COLLISSION_BOX = 2,
		};
	}

	namespace Container
	{
		FLAGGG_FLAGSET(Scene::BoneCollisionShape, BoneCollisionShapeFlags);
	}

	namespace Scene
	{
		struct FlagGG_API Bone
		{
			Container::String name_;
			Container::StringHash nameHash_;
			uint32_t parentIndex_{ 0 };
			
			Math::Vector3 initPosition_{ Math::Vector3::ZERO };
			Math::Quaternion initRotation_{ Math::Quaternion::IDENTITY };
			Math::Vector3 initScale_{ Math::Vector3::ONE };
			Math::Matrix3x4 offsetMatrix{ Math::Matrix3x4::IDENTITY };
			bool animated_{ true };
			Container::BoneCollisionShapeFlags collisionMask_{ BONE_COLLISSION_NONE };
			float radius_{ 0.0f };
			Container::WeakPtr<Node> node_;
			Math::BoundingBox boundingBox_;
		};

		class FlagGG_API Skeleton
		{
		public:
			void Load(IOFrame::Buffer::IOBuffer* stream);

			Bone* GetBone(const Container::String& name);

			Bone* GetBone(Container::StringHash nameHash);

			const Container::Vector<Bone>& GetBones() const;

			Container::Vector<Bone>& GetBones();

			Bone* GetRootBone();

			void ResetNode();

		private:
			Container::Vector<Bone> bones_;

			uint32_t rootBoneIndex_;
		};
	}
}
