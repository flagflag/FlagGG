#include "ParticleModuleOrientation.h"

#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleOrientationAxisLock);

/*-----------------------------------------------------------------------------
	ParticleModuleOrientationAxisLock implementation.
-----------------------------------------------------------------------------*/
ParticleModuleOrientationAxisLock::ParticleModuleOrientationAxisLock()
{
	spawnModule_ = true;
	updateModule_ = true;
}

//    uint8 LockAxisFlags;
//    FVector LockAxis;
void ParticleModuleOrientationAxisLock::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
}

void ParticleModuleOrientationAxisLock::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
}

void ParticleModuleOrientationAxisLock::SetLockAxis(ParticleAxisLock eLockFlags)
{
	lockAxisFlags_ = eLockFlags;
}

bool ParticleModuleOrientationAxisLock::LoadXML(const XMLElement& root)
{
	if (XMLElement lockAxisFlagsNode = root.GetChild("lockAxisFlags"))
	{
		lockAxisFlags_ = ParticleAxisLock(lockAxisFlagsNode.GetUInt("value"));

		return true;
	}

	return false;
}

bool ParticleModuleOrientationAxisLock::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
