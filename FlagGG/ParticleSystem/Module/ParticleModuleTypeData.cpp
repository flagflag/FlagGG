#include "ParticleModuleTypeData.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Resource/ResourceCache.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleModuleTypeDataMesh implementation.
-----------------------------------------------------------------------------*/

ParticleModuleTypeDataMesh::ParticleModuleTypeDataMesh()
{
	castShadows_ = false;
	doCollisions_ = false;
	meshAlignment_ = PSMA_MeshFaceCameraWithRoll;
	axisLockOption_ = EPAL_NONE;
	cameraFacingUpAxisOption_DEPRECATED_ = CameraFacing_NoneUP;
	cameraFacingOption_ = XAxisFacing_NoUp;
	collisionsConsiderPartilceSize_ = true;
	useStaticMeshLODs_ = true;
	LODSizeScale_ = 1.0f;
}

ParticleEmitterInstance* ParticleModuleTypeDataMesh::CreateInstance(ParticleEmitter* InEmitterParent, ParticleSystemComponent* InComponent)
{
	SetToSensibleDefaults(InEmitterParent);
	ParticleEmitterInstance* instance = new ParticleMeshEmitterInstance();
	ASSERT(instance);

	instance->InitParameters(InEmitterParent, InComponent);

	CreateDistribution();

	return instance;
}

void ParticleModuleTypeDataMesh::SetToSensibleDefaults(ParticleEmitter* Owner)
{
	if (!mesh_)
	{
		mesh_ = GetSubsystem<ResourceCache>()->GetResource<Model>("/Engine/EngineMeshes/ParticleCube.ParticleCube");
	}
}

void ParticleModuleTypeDataMesh::CreateDistribution()
{
	if (!rollPitchYawRange_.IsCreated())
	{
		rollPitchYawRange_.distribution_ = new DistributionVectorUniform();
	}
}

}
