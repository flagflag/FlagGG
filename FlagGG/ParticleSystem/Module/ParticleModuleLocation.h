#pragma once

#include "ParticleSystem/ParticleModule.h"

namespace FlagGG
{

class ParticleLODLevel;

class FlagGG_API ParticleModuleLocationBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleLocationBase, ParticleModule);
public:
};

class FlagGG_API ParticleModulePivotOffset : public ParticleModuleLocationBase
{
	OBJECT_OVERRIDE(ParticleModulePivotOffset, ParticleModuleLocationBase);
public:

	/** Offset applied in UV space to the particle vertex positions. Defaults to (0.5,0.5) putting the pivot in the centre of the partilce. */
	Vector2 pivotOffset_;

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin UParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& EmitterInfo) override;
	//End UParticleModule Interface

#if WITH_EDITOR
	virtual bool IsValidForLODLevel(ParticleLODLevel* LODLevel, String& OutErrorString) override;
#endif
};

}
