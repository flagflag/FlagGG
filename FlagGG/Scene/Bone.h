//
// 骨骼、骨架
//

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

namespace FlagGG
{

enum BoneCollisionShape : UInt8
{
	BONE_COLLISSION_NONE = 0,
	BONE_COLLISSION_SPHERE = 1,
	BONE_COLLISSION_BOX = 2,
};

FLAGGG_FLAGSET(BoneCollisionShape, BoneCollisionShapeFlags);

struct FlagGG_API Bone
{
	String name_;
	StringHash nameHash_;
	UInt32 parentIndex_{ 0 };
			
	Vector3 initPosition_{ Vector3::ZERO };
	Quaternion initRotation_{ Quaternion::IDENTITY };
	Vector3 initScale_{ Vector3::ONE };
	Matrix3x4 offsetMatrix_{ Matrix3x4::IDENTITY };
	bool animated_{ true };
	BoneCollisionShapeFlags collisionMask_{ BONE_COLLISSION_NONE };
	Real radius_{ 0.0f };
	WeakPtr<Node> node_;
	BoundingBox boundingBox_;
};

class FlagGG_API Skeleton
{
public:
	void Load(IOFrame::Buffer::IOBuffer* stream);

	const Bone* GetBone(const String& name) const;

	const Bone* GetBone(StringHash nameHash) const;

	const Vector<Bone>& GetBones() const;

	Vector<Bone>& GetBones();

	const Bone* GetRootBone() const;

	void ResetNode();

private:
	Vector<Bone> bones_;

	UInt32 rootBoneIndex_;
};

}
