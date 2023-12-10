#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Container/EnumAsByte.h"

namespace FlagGG
{

enum ParticleAxisLock
{
	/** No locking to an axis...							*/
	EPAL_NONE,
	/** Lock the sprite facing towards the positive X-axis	*/
	EPAL_X,
	/** Lock the sprite facing towards the positive Y-axis	*/
	EPAL_Y,
	/** Lock the sprite facing towards the positive Z-axis	*/
	EPAL_Z,
	/** Lock the sprite facing towards the negative X-axis	*/
	EPAL_NEGATIVE_X,
	/** Lock the sprite facing towards the negative Y-axis	*/
	EPAL_NEGATIVE_Y,
	/** Lock the sprite facing towards the negative Z-axis	*/
	EPAL_NEGATIVE_Z,
	/** Lock the sprite rotation on the X-axis				*/
	EPAL_ROTATE_X,
	/** Lock the sprite rotation on the Y-axis				*/
	EPAL_ROTATE_Y,
	/** Lock the sprite rotation on the Z-axis				*/
	EPAL_ROTATE_Z,
	EPAL_MAX,
};

class FlagGG_API ParticleModuleOrientationBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleOrientationBase, ParticleModule);
public:
};

class ParticleModuleOrientationAxisLock : public ParticleModuleOrientationBase
{
	OBJECT_OVERRIDE(ParticleModuleOrientationAxisLock, ParticleModuleOrientationBase);
public:
	ParticleModuleOrientationAxisLock();

	//~ Begin UParticleModule Interface
	virtual void	Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void	Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	//~ End UParticleModule Interface

	//@todo document
	virtual void	SetLockAxis(ParticleAxisLock eLockFlags);

	/**
	 *	The lock axis flag setting.
	 *	Can be one of the following:
	 *		EPAL_NONE			No locking to an axis.
	 *		EPAL_X				Lock the sprite facing towards +X.
	 *		EPAL_Y				Lock the sprite facing towards +Y.
	 *		EPAL_Z				Lock the sprite facing towards +Z.
	 *		EPAL_NEGATIVE_X		Lock the sprite facing towards -X.
	 *		EPAL_NEGATIVE_Y		Lock the sprite facing towards -Y.
	 *		EPAL_NEGATIVE_Z		Lock the sprite facing towards -Z.
	 *		EPAL_ROTATE_X		Lock the sprite rotation on the X-axis.
	 *		EPAL_ROTATE_Y		Lock the sprite rotation on the Y-axis.
	 *		EPAL_ROTATE_Z		Lock the sprite rotation on the Z-axis.
	 */
	EnumAsByte<ParticleAxisLock> lockAxisFlags_;
};


}
