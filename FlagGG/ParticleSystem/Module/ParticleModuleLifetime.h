#pragma once

#include "ParticleSystem/ParticleModule.h"

namespace FlagGG
{

struct ParticleEmitterInstance;

class ParticleModuleLifetimeBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleLifetimeBase, ParticleModule);
public:

	/** Return the maximum lifetime this module would return. */
	virtual float GetMaxLifetime()
	{
		return 0.0f;
	}

	/**
		*	Return the lifetime value at the given time.
		*
		*	@param	Owner		The emitter instance that owns this module
		*	@param	InTime		The time input for retrieving the lifetime value
		*	@param	Data		The data associated with the distribution
		*
		*	@return	float		The Lifetime value
		*/
	virtual float GetLifetimeValue(ParticleEmitterInstance* owner, float inTime, Object* data = NULL)
		PURE_VIRTUAL(ParticleModuleLifetimeBase::GetLifetimeValue, return 0.0f;);
};

}
