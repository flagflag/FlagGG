#include "ParticleSystemRenderer.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/Module/ParticleModuleRequired.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleMeshDataBuilder implementation.
-----------------------------------------------------------------------------*/

ParticleMeshDataBuilder::~ParticleMeshDataBuilder()
{

}

void ParticleMeshDataBuilder::BeginDataCollection()
{
	vertexBuffer_.Clear();
}

void ParticleMeshDataBuilder::EndDataCollection()
{
	vertexBuffer_.CommitToGPU();
}

ParticleMeshDataBuilder::ParticleMeshData ParticleMeshDataBuilder::Allocate(UInt32 vertexSizeInByte, UInt32 indexCount)
{
	ParticleMeshData meshData;
	meshData.vertexAllocation_ = vertexBuffer_.Allocate(vertexSizeInByte);
	meshData.indexAllocation_ = indexBuffer_.Allocate(indexCount);
	return meshData;
}

Vector2 ParticleMeshDataBuilder::GetParticleSize(const BaseParticle& particle, ParticleModuleRequired* requireModule)
{
	Vector2 size(Abs(particle.size_.x_), Abs(particle.size_.y_));
	if (requireModule->screenAlignment_ == PSA_Square ||
		requireModule->screenAlignment_ == PSA_FacingCameraPosition ||
		requireModule->screenAlignment_ == PSA_FacingCameraDistanceBlend)
	{
		size.x_ = size.y_;
	}
	return size;
}

void ParticleMeshDataBuilder::ApplyOrbitToPosition(Int32 orbitModuleOffset, const BaseParticle& particle, bool useLocalSpace, const Matrix3x4& localToWorld, Vector3& particlePosition, Vector3& particleOldPosition)
{
	if (orbitModuleOffset != 0)
	{
		Int32 currentOffset = orbitModuleOffset;
		const UInt8* particleBase = (const UInt8*)&particle;
		PARTICLE_ELEMENT(OrbitChainModuleInstancePayload, orbitPayload);

		if (useLocalSpace)
		{
			particlePosition += orbitPayload.offset_;
			particleOldPosition += orbitPayload.previousOffset_;
		}
		else
		{
			particlePosition += localToWorld * orbitPayload.offset_;
			particleOldPosition += localToWorld * orbitPayload.previousOffset_;
		}
	}
}

Vector3 ParticleMeshDataBuilder::GetCameraOffsetFromPayload(Int32 cameraPayloadOffset, const BaseParticle& particle, const Vector3& particlePosition, const Vector3& cameraPosition)
{
	ASSERT(cameraPayloadOffset > 0);

	Vector3 dirToCamera = cameraPosition - particlePosition;
	CameraOffsetParticlePayload* cameraPayload = ((CameraOffsetParticlePayload*)((UInt8*)(&particle) + cameraPayloadOffset));

	return GetCameraOffset(cameraPayload->offset_, dirToCamera);
}

Vector3 ParticleMeshDataBuilder::GetCameraOffset(float cameraPayloadOffset, Vector3 dirToCamera)
{
	float checkSize = dirToCamera.LengthSquared();
	dirToCamera.Normalize();

	if (checkSize > (cameraPayloadOffset * cameraPayloadOffset))
	{
		return dirToCamera * cameraPayloadOffset;
	}
	else
	{
		// If the offset will push the particle behind the camera, then push it 
		// WAY behind the camera. This is a hack... but in the case of 
		// PSA_Velocity, it is required to ensure that the particle doesn't 
		// 'spin' flat and come into view.
		return dirToCamera * cameraPayloadOffset * F_INFINITY * 0.5f;
	}
}

void ParticleMeshDataBuilder::GetDynamicValueFromPayload(Int32 dynamicPayloadOffset, const BaseParticle& particle, Vector4& outDynamicData)
{
	ASSERT(dynamicPayloadOffset > 0);
	// TODO => EmitterDynamicParameterPayload
	//const EmitterDynamicParameterPayload* dynPayload = ((const EmitterDynamicParameterPayload*)((UInt8*)(&particle) + dynamicPayloadOffset));
	//outDynamicData.x_ = dynPayload->dynamicParameterValue_[0];
	//outDynamicData.y_ = dynPayload->dynamicParameterValue_[1];
	//outDynamicData.z_ = dynPayload->dynamicParameterValue_[2];
	//outDynamicData.w_ = dynPayload->dynamicParameterValue_[3];
};

/*-----------------------------------------------------------------------------
	ParticleSystemRenderer implementation.
-----------------------------------------------------------------------------*/

ParticleSystemRenderer::ParticleSystemRenderer()
{

}

ParticleSystemRenderer::~ParticleSystemRenderer()
{

}

void ParticleSystemRenderer::PostToRenderUpdateQueue(ParticleSystemComponent* particleSystemComponent)
{
	particleSystemComponents_.Push(particleSystemComponent);
}

void ParticleSystemRenderer::RenderUpdate(const RenderPiplineContext* renderPiplineContext)
{
	particleMeshDataBuilder_.BeginDataCollection();

	for (auto* particleSystemComponent : particleSystemComponents_)
	{
		particleSystemComponent->RenderUpdate(renderPiplineContext, &particleMeshDataBuilder_);
	}

	particleMeshDataBuilder_.EndDataCollection();

	particleSystemComponents_.Clear();
}

}
