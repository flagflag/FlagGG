#pragma once

#include "ParticleSystem/ParticleModule.h"

namespace FlagGG
{

class ParticleModuleOrbitBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleOrbitBase, ParticleModule);
public:

	/** 
	 *	If true, distribution values will be retrieved using the EmitterTime.
	 *	If false (default), they will be retrieved using the Particle.RelativeTime.
	 */
	UInt32 useEmitterTime_ : 1;

};

class ParticleModuleOrbit : public ParticleModuleOrbitBase
{
	OBJECT_OVERRIDE(ParticleModuleOrbit, ParticleModuleOrbitBase);
public:

};

}
