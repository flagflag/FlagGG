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
	for (auto& particleDataMesh : particleMeshDataSet_)
	{
		delete particleDataMesh;
		particleDataMesh = nullptr;
	}
	particleMeshDataSet_.Clear();

	for (auto& particleDataMesh : particleMeshDataPool_)
	{
		delete particleDataMesh;
		particleDataMesh = nullptr;
	}
	particleMeshDataPool_.Clear();
}

void ParticleMeshDataBuilder::BeginDataCollection()
{
	for (auto* particleMeshData : particleMeshDataSet_)
	{
		particleMeshDataPool_.Push(particleMeshData);
	}

	particleMeshDataSet_.Clear();
}

void ParticleMeshDataBuilder::EndDataCollection()
{
	if (particleMeshDataSet_.Empty())
		return;

// 按照顶点描述排序
	Sort(particleMeshDataSet_.Begin(), particleMeshDataSet_.End(), [](ParticleMeshData* _1, ParticleMeshData* _2)
	{
		return _1->vertexDesc_ < _2->vertexDesc_;
	});

// Pass1 - 计算出所有顶点需要使用的字节数
	UInt32 vertexBaseOffset = 0u;
	for (auto* particleMeshData : particleMeshDataSet_)
	{
		particleMeshData->geometry_->SetVertexBuffer(0, vertexBuffer_);
		particleMeshData->geometry_->SetIndexBuffer(indexBuffer_);
		particleMeshData->geometry_->SetDataRange(0, 0, 0, particleMeshData->vertexCount_, vertexBaseOffset);
		vertexBaseOffset += particleMeshData->vertexCount_ * particleMeshData->vertexDesc_->GetStrideSize();
	}

	if (vertexBaseOffset == 0)
		return;

// Pass2 - 分配顶点内存、同步顶点数据到GpuBuffer
	if (!vertexBuffer_)
		vertexBuffer_ = new VertexBuffer();
	if (!indexBuffer_)
		indexBuffer_ = new IndexBuffer();

	PODVector<VertexElement> elements;
	elements.Push(VertexElement(VE_INT, SEM_POSITION));
	UInt32 vertexCount = (vertexBaseOffset + 5) / 4;
	vertexBuffer_->SetSize(vertexCount, elements);

	char* data = (char*)vertexBuffer_->Lock(0, vertexCount);

	for (auto* particleMeshData : particleMeshDataSet_)
	{
		memcpy(data, particleMeshData->vertexData_.Buffer(), particleMeshData->vertexData_.Size());
		data += particleMeshData->vertexData_.Size();
	}

	vertexBuffer_->Unlock();
}

ParticleMeshData* ParticleMeshDataBuilder::Alloc(VertexDescription* vertexDesc)
{
	if (particleMeshDataPool_.Size())
	{
		ParticleMeshData* particleMeshData = particleMeshDataPool_.Back();
		particleMeshDataPool_.Pop();
		particleMeshDataSet_.Push(particleMeshData);

		particleMeshData->vertexDesc_ = vertexDesc;
		particleMeshData->vertexData_.Clear();
		particleMeshData->indexData_.Clear();
		return particleMeshData;
	}

	ParticleMeshData* particleMeshData = new ParticleMeshData();
	particleMeshData->vertexDesc_ = vertexDesc;
	particleMeshDataSet_.Push(particleMeshData);
	return particleMeshData;
}

Vector3 ParticleMeshDataBuilder::GetParticleSize(const BaseParticle& particle, ParticleModuleRequired* requireModule)
{
	Vector3 size(Abs(particle.size_.x_), Abs(particle.size_.y_), Abs(particle.size_.z_));
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
