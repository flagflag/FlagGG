#pragma once

#include "ParticleSystem/ParticleEmitter.h"

namespace FlagGG
{

class ParticleSystemComponent;

enum ParticleScreenAlignment
{
	PSA_FacingCameraPosition,
	PSA_Square,
	PSA_Rectangle,
	PSA_Velocity,
	PSA_AwayFromCenter,
	PSA_TypeSpecific,
	PSA_FacingCameraDistanceBlend,
	PSA_MAX,
};

class ParticleSpriteEmitter : public ParticleEmitter
{
	OBJECT_OVERRIDE(ParticleSpriteEmitter, ParticleEmitter);
public:
	ParticleSpriteEmitter();

	//~ Begin ParticleEmitter Interface
	virtual ParticleEmitterInstance* CreateInstance(ParticleSystemComponent* InComponent) override;
	virtual void SetToSensibleDefaults() override;
	//~ End ParticleEmitter Interface
};

}
