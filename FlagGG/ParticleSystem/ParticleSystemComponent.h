#pragma once

#include "Scene/DrawableComponent.h"
#include "ParticleSystem/ParticleSystem.h"

namespace FlagGG
{

class FlagGG_API ParticleSystemComponent : public DrawableComponent
{
	OBJECT_OVERRIDE(ParticleSystemComponent, DrawableComponent);
public:
	/**
	  * Decide which detail mode should be applied to this particle system. If we have an editor
	  * override specified, use that. Otherwise use the global cached value
	  */
	Int32 GetCurrentDetailMode() const;

	SharedPtr<ParticleSystem> template_;

	Vector<SharedPtr<Material>> emitterMaterials_;

	/** Indicates that the component has not been ticked since being registered. */
	UInt8 justRegistered_ : 1;

	UInt8 warmingUp_ : 1;

	/** This is created at start up and then added to each emitter */
	float emitterDelay_;

private:

};

}
