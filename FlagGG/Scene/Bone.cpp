#include "Scene/Bone.h"
#include "IOFrame/Buffer/IOBufferAux.h"

namespace FlagGG
{
	namespace Scene
	{
		void Skeleton::Load(IOFrame::Buffer::IOBuffer* stream)
		{
			uint32_t boneNum = 0;
			stream->ReadUInt32(boneNum);
			bones_.Reserve(boneNum);

			for (uint32_t i = 0; i < boneNum; ++i)
			{
				Bone bone;
				IOFrame::Buffer::ReadString(stream, bone.name_);
				stream->ReadUInt32(bone.parentIndex_);
				IOFrame::Buffer::ReadVector3(stream, bone.initPosition_);
				IOFrame::Buffer::ReadQuaternion(stream, bone.initRotation_);
				IOFrame::Buffer::ReadVector3(stream, bone.initScale_);
				IOFrame::Buffer::ReadMatrix3x4(stream, bone.offsetMatrix);

				uint8_t byte;
				stream->ReadUInt8(byte);
				bone.collisionMask_ = Container::BoneCollisionShapeFlags(byte);
				if (bone.collisionMask_ & BONE_COLLISSION_SPHERE)
					stream->ReadFloat(bone.radius_);
				if (bone.collisionMask_ & BONE_COLLISSION_BOX)
				{
					Math::Vector3 ignore;
					IOFrame::Buffer::ReadVector3(stream, ignore);
					IOFrame::Buffer::ReadVector3(stream, ignore);
				}

				if (bone.parentIndex_ == i)
					rootBoneIndex_ = i;

				bones_.Push(bone);
			}
		}

		const Container::Vector<Bone>& Skeleton::GetBones() const
		{
			return  bones_;
		}

		Container::Vector<Bone>& Skeleton::GetBones()
		{
			return bones_;
		}

		void Skeleton::ResetNode()
		{
			for (auto& bone : bones_)
			{
				bone.node_.Reset();
			}
		}
	}
}
