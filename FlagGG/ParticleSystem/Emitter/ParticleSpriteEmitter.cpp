#include "ParticleSystem/Emitter/ParticleSpriteEmitter.h"
#include "ParticleSystem/Module/ParticleModuleTypeData.h"
#include "ParticleSystem/Module/ParticleModuleLifetime.h"
#include "ParticleSystem/Module/ParticleModuleSize.h"
#include "ParticleSystem/Module/ParticleModuleVelocity.h"
#include "ParticleSystem/Module/ParticleModuleColor.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatConstantCurve.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	ParticleSpriteEmitter implementation.
-----------------------------------------------------------------------------*/
ParticleSpriteEmitter::ParticleSpriteEmitter()
{
}


ParticleEmitterInstance* ParticleSpriteEmitter::CreateInstance(ParticleSystemComponent* inComponent)
{
	// If this emitter was cooked out or has no valid LOD levels don't create an instance for it.
	if ((cookedOut_ == true) || (LODLevels_.Size() == 0))
	{
		return NULL;
	}

	ParticleEmitterInstance* instance = 0;

	ParticleLODLevel* LODLevel	= GetLODLevel(0);
	ASSERT(LODLevel);

	if (LODLevel->typeDataModule_)
	{
		//@todo. This will NOT work for trails/beams!
		instance = LODLevel->typeDataModule_->CreateInstance(this, inComponent);
	}
	else
	{
		ASSERT(inComponent);
		instance = new ParticleSpriteEmitterInstance();
		ASSERT(instance);
		instance->InitParameters(this, inComponent);
	}

	if (instance)
	{
		instance->currentLODLevelIndex_	= 0;
		instance->currentLODLevel_		= LODLevels_[instance->currentLODLevelIndex_];
		instance->Init();
	}

	return instance;
}

void ParticleSpriteEmitter::SetToSensibleDefaults()
{
	ParticleLODLevel* LODLevel = LODLevels_[0];

	// Spawn rate
	LODLevel->spawnModule_->LODValidity_ = 1;
	DistributionFloatConstant* spawnRateDist = RTTICast<DistributionFloatConstant>(LODLevel->spawnModule_->rate_.distribution_);
	if (spawnRateDist)
	{
		spawnRateDist->constant_ = 20.f;
	}

	// Create basic set of modules

	// Lifetime module
	SharedPtr<ParticleModuleLifetime> lifetimeModule = MakeShared<ParticleModuleLifetime>();
	DistributionFloatUniform* lifetimeDist = RTTICast<DistributionFloatUniform>(lifetimeModule->lifetime_.distribution_);
	if (lifetimeDist)
	{
		lifetimeDist->min_ = 1.0f;
		lifetimeDist->max_ = 1.0f;
		lifetimeDist->isDirty_ = true;
	}
	lifetimeModule->LODValidity_ = 1;
	LODLevel->modules_.Push(lifetimeModule);

	// Size module
	SharedPtr<ParticleModuleSize> sizeModule = MakeShared<ParticleModuleSize>();
	DistributionVectorUniform* SizeDist = RTTICast<DistributionVectorUniform>(sizeModule->startSize_.distribution_);
	if (SizeDist)
	{
		SizeDist->min_ = Vector3(25.f, 25.f, 25.f);
		SizeDist->max_ = Vector3(25.f, 25.f, 25.f);
		SizeDist->isDirty_ = true;
	}
	sizeModule->LODValidity_ = 1;
	LODLevel->modules_.Push(sizeModule);

	// Initial velocity module
	SharedPtr<ParticleModuleVelocity> velModule = MakeShared<ParticleModuleVelocity>();
	DistributionVectorUniform* VelDist = RTTICast<DistributionVectorUniform>(velModule->startVelocity_.distribution_);
	if (VelDist)
	{
		VelDist->min_ = Vector3(-10.f, -10.f, 50.f);
		VelDist->max_ = Vector3(10.f, 10.f, 100.f);
		VelDist->isDirty_ = true;
	}
	velModule->LODValidity_ = 1;
	LODLevel->modules_.Push(velModule);

	// Color over life module
	SharedPtr<ParticleModuleColorOverLife> colorModule = MakeShared<ParticleModuleColorOverLife>();
	DistributionVectorConstantCurve* colorCurveDist = RTTICast<DistributionVectorConstantCurve>(colorModule->colorOverLife_.distribution_);
	if (colorCurveDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 key = 0; key < 2; key++)
		{
			Int32 keyIndex = colorCurveDist->CreateNewKey(key * 1.0f);
			for (Int32 subIndex = 0; subIndex < 3; subIndex++)
			{
				colorCurveDist->SetKeyOut(subIndex, keyIndex, 1.0f);
			}
		}
		colorCurveDist->isDirty_ = true;
	}
	colorModule->alphaOverLife_.distribution_ = MakeShared<DistributionFloatConstantCurve>();
	DistributionFloatConstantCurve* alphaCurveDist = RTTICast<DistributionFloatConstantCurve>(colorModule->alphaOverLife_.distribution_);
	if (alphaCurveDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 key = 0; key < 2; key++)
		{
			Int32 keyIndex = alphaCurveDist->CreateNewKey(key * 1.0f);
			if (key == 0)
			{
				alphaCurveDist->SetKeyOut(0, keyIndex, 1.0f);
			}
			else
			{
				alphaCurveDist->SetKeyOut(0, keyIndex, 0.0f);
			}
		}
		alphaCurveDist->isDirty_ = true;
	}
	colorModule->LODValidity_ = 1;
	LODLevel->modules_.Push(colorModule);
}

}
