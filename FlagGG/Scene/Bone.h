#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Container/FlagSet.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Scene
	{
		enum BoneCollisionShape : uint8_t
		{
			BONE_COLLISION_NONE		= 0,
			BONE_COLISSION_SPHERE	= 1,
			BONE_COLLISION_BOX		= 2,
		};

		FLAGGG_FLAGSET(BoneCollisionShape, BoneCollisionShapeFlags);

		namespace Container
		{
			template<> struct IsFlagSet<BoneCollisionShape> { constexpr static bool value_ = true; };
		}
		using flagsetName = FlagGG::Container::FlagSet<BoneCollisionShape>;

		struct FlagGG_API Bone
		{
			Container::String name_;
			uint32_t parentIndex_;
			
			Math::Vector3 initPosition_;
			Math::Quaternion initRotation_;
			Math::Vector3 initScale_;
			Math::Matrix3x4 offsetMatrix;


		};

		class FlagGG_API Skeleton
		{

		};
	}
}
