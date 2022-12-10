#include "Scene/Bone.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{

void Skeleton::Load(IOFrame::Buffer::IOBuffer* stream)
{
	UInt32 boneNum = 0;
	stream->ReadUInt32(boneNum);
	bones_.Reserve(boneNum);

	for (UInt32 i = 0; i < boneNum; ++i)
	{
		Bone bone;
		IOFrame::Buffer::ReadString(stream, bone.name_);
		bone.nameHash_ = bone.name_;
		stream->ReadUInt32(bone.parentIndex_);
		IOFrame::Buffer::ReadVector3(stream, bone.initPosition_);
		IOFrame::Buffer::ReadQuaternion(stream, bone.initRotation_);
		IOFrame::Buffer::ReadVector3(stream, bone.initScale_);
		IOFrame::Buffer::ReadMatrix3x4(stream, bone.offsetMatrix);

		uint8_t byte;
		stream->ReadUInt8(byte);
		bone.collisionMask_ = BoneCollisionShapeFlags(byte);
		if (bone.collisionMask_ & BONE_COLLISSION_SPHERE)
			stream->ReadFloat(bone.radius_);
		if (bone.collisionMask_ & BONE_COLLISSION_BOX)
			IOFrame::Buffer::ReadBoundingBox(stream, bone.boundingBox_);

		if (bone.parentIndex_ == i)
			rootBoneIndex_ = i;

		bones_.Push(bone);
	}
}

Bone* Skeleton::GetBone(const String& name)
{
	return GetBone(StringHash(name));
}

Bone* Skeleton::GetBone(StringHash nameHash)
{
	for (auto& bone : bones_)
	{
		if (bone.nameHash_ == nameHash)
		{
			return &bone;
		}
	}
	return nullptr;
}

const Vector<Bone>& Skeleton::GetBones() const
{
	return  bones_;
}

Vector<Bone>& Skeleton::GetBones()
{
	return bones_;
}

Bone* Skeleton::GetRootBone()
{
	return &bones_[rootBoneIndex_];
}

void Skeleton::ResetNode()
{
	for (auto& bone : bones_)
	{
		bone.node_.Reset();
	}
}

}
