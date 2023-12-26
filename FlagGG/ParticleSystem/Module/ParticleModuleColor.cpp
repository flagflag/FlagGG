#include "ParticleModuleColor.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionFloatConstantCurve.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"
#include "Math/Distributions/DistributionFloatParticleParameter.h"
#include "Math/Distributions/DistributionVectorParticleParameter.h"
#include "Math/InterpCurveEdSetup.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "ParticleSystem/ParticleEmitter.h"

namespace FlagGG
{

/*-----------------------------------------------------------------------------
	Abstract base modules used for categorization.
-----------------------------------------------------------------------------*/
ParticleModuleColorBase::ParticleModuleColorBase()
{
}

/*-----------------------------------------------------------------------------
	ParticleModuleColorOverLife implementation.
-----------------------------------------------------------------------------*/
ParticleModuleColorOverLife::ParticleModuleColorOverLife()
{
	spawnModule_ = true;
	updateModule_ = true;
	curvesAsColor_ = true;
	clampAlpha_ = true;
}

void ParticleModuleColorOverLife::InitializeDefaults()
{
	if (!colorOverLife_.IsCreated())
	{
		colorOverLife_.distribution_ = MakeShared<DistributionVectorConstantCurve>();
	}

	if (!alphaOverLife_.IsCreated())
	{
		SharedPtr<DistributionFloatConstant> distributionAlphaOverLife = MakeShared<DistributionFloatConstant>();
		distributionAlphaOverLife->constant_ = 1.0f;
		alphaOverLife_.distribution_ = distributionAlphaOverLife;
	}
}

void ParticleModuleColorOverLife::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	bool ScaleColor = true;
	bool ScaleAlpha = true;
	if (IsUsedInGPUEmitter())
	{
		if (colorOverLife_.distribution_->IsInstanceOf(DistributionVectorParticleParameter::GetTypeStatic()))
		{
			emitterInfo.dynamicColor_ = colorOverLife_;
#if WITH_EDITOR
			emitterInfo.dynamicColor_.distribution_->isDirty_ = true;
			emitterInfo.dynamicColor_.Initialize();
#endif
			ScaleColor = false;
			emitterInfo.colorScale_.InitializeWithConstant(Vector3(1.0f, 1.0f, 1.0f));
		}

		if (alphaOverLife_.distribution_->IsInstanceOf(DistributionFloatParticleParameter::GetTypeStatic()))
		{
			emitterInfo.dynamicAlpha_ = alphaOverLife_;
#if WITH_EDITOR
			emitterInfo.dynamicAlpha_.distribution_->bIsDirty = true;
			emitterInfo.dynamicAlpha_.Initialize();
#endif
			ScaleAlpha = false;
			emitterInfo.alphaScale_.InitializeWithConstant(1.0f);
		}
	}

	if (ScaleColor)
	{
		emitterInfo.colorScale_.Initialize(colorOverLife_.distribution_);
	}

	if (ScaleAlpha)
	{
		emitterInfo.alphaScale_.Initialize(alphaOverLife_.distribution_);
	}
}

bool ParticleModuleColorOverLife::AddModuleCurvesToEditor(InterpCurveEdSetup* edSetup, PODVector<const CurveEdEntry*>& outCurveEntries)
{
	bool bNewCurve = false;
#if WITH_EDITORONLY_DATA
	// Iterate over object and find any InterpCurveFloats or UDistributionFloats
	for (TFieldIterator<FStructProperty> It(GetClass()); It; ++It)
	{
		// attempt to get a distribution from a random struct property
		UObject* Distribution = FRawDistribution::TryGetDistributionObjectFromRawDistributionProperty(*It, (uint8*)this);
		if (Distribution)
		{
			FCurveEdEntry* Curve = NULL;
			if (Distribution->IsA(UDistributionFloat::StaticClass()))
			{
				// We are assuming that this is the alpha...
				if (bClampAlpha == true)
				{
					bNewCurve |= EdSetup->AddCurveToCurrentTab(Distribution, It->GetName(), ModuleEditorColor, &Curve, true, true, true, 0.0f, 1.0f);
				}
				else
				{
					bNewCurve |= EdSetup->AddCurveToCurrentTab(Distribution, It->GetName(), ModuleEditorColor, &Curve, true, true);
				}
			}
			else
			{
				// We are assuming that this is the color...
				bNewCurve |= EdSetup->AddCurveToCurrentTab(Distribution, It->GetName(), ModuleEditorColor, &Curve, true, true);
			}
			OutCurveEntries.Add(Curve);
		}
	}
#endif // WITH_EDITORONLY_DATA
	return bNewCurve;
}

void ParticleModuleColorOverLife::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SPAWN_INIT;
	Vector3 colorVec	= colorOverLife_.GetValue(particle.relativeTime_, owner->component_);
	float	fAlpha		= alphaOverLife_.GetValue(particle.relativeTime_, owner->component_);
	particle.color_.r_	   = colorVec.x_;
	particle.baseColor_.r_ = colorVec.x_;
	particle.color_.g_	   = colorVec.y_;
	particle.baseColor_.g_ = colorVec.y_;
	particle.color_.b_	   = colorVec.z_;
	particle.baseColor_.b_ = colorVec.z_;
	particle.color_.a_	   = fAlpha;
	particle.baseColor_.a_ = fAlpha;
}

void ParticleModuleColorOverLife::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	if ((owner == NULL) || (owner->activeParticles_ <= 0) ||
		(owner->particleData_ == NULL) || (owner->particleIndices_ == NULL))
	{
		return;
	}
	const RawDistribution* fastColorOverLife = colorOverLife_.GetFastRawDistribution();
	const RawDistribution* fastAlphaOverLife = alphaOverLife_.GetFastRawDistribution();
	// PlatformMisc::Prefetch(owner->particleData_, (owner->particleIndices_[0] * owner->particleStride_));
	// PlatformMisc::Prefetch(owner->particleData_, (owner->particleIndices_[0] * owner->particleStride_) + PLATFORM_CACHE_LINE_SIZE);
	if (fastColorOverLife && fastAlphaOverLife)
	{
		// fast path
		BEGIN_UPDATE_LOOP;
		{
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
			fastColorOverLife->GetValue3None(particle.relativeTime_, &particle.color_.r_);
			fastAlphaOverLife->GetValue1None(particle.relativeTime_, &particle.color_.a_);
		}
		END_UPDATE_LOOP;
	}
	else
	{
		Vector3 ColorVec;
		float	fAlpha;
		BEGIN_UPDATE_LOOP;
		{
			ColorVec = colorOverLife_.GetValue(particle.relativeTime_, owner->component_);
			fAlpha = alphaOverLife_.GetValue(particle.relativeTime_, owner->component_);
			// PlatformMisc::Prefetch(particleData, (particleIndices[i+1] * particleStride));
			// PlatformMisc::Prefetch(particleData, (particleIndices[i+1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
			particle.color_.r_ = ColorVec.x_;
			particle.color_.g_ = ColorVec.y_;
			particle.color_.b_ = ColorVec.z_;
			particle.color_.a_ = fAlpha;
		}
		END_UPDATE_LOOP;
	}
}

void ParticleModuleColorOverLife::SetToSensibleDefaults(ParticleEmitter* owner)
{
	colorOverLife_.distribution_ = MakeShared<DistributionVectorConstantCurve>();
	DistributionVectorConstantCurve* ColorOverLifeDist = RTTICast<DistributionVectorConstantCurve>(colorOverLife_.distribution_);
	if (ColorOverLifeDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 Key = 0; Key < 2; Key++)
		{
			Int32	KeyIndex = ColorOverLifeDist->CreateNewKey(Key * 1.0f);
			for (Int32 SubIndex = 0; SubIndex < 3; SubIndex++)
			{
				if (Key == 0)
				{
					ColorOverLifeDist->SetKeyOut(SubIndex, KeyIndex, 1.0f);
				}
				else
				{
					ColorOverLifeDist->SetKeyOut(SubIndex, KeyIndex, 0.0f);
				}
			}
		}
		ColorOverLifeDist->isDirty_ = true;
	}

	alphaOverLife_.distribution_ = MakeShared<DistributionFloatConstantCurve>();
	DistributionFloatConstantCurve* alphaOverLifeDist = RTTICast<DistributionFloatConstantCurve>(alphaOverLife_.distribution_);
	if (alphaOverLifeDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 Key = 0; Key < 2; Key++)
		{
			Int32	KeyIndex = alphaOverLifeDist->CreateNewKey(Key * 1.0f);
			if (Key == 0)
			{
				alphaOverLifeDist->SetKeyOut(0, KeyIndex, 1.0f);
			}
			else
			{
				alphaOverLifeDist->SetKeyOut(0, KeyIndex, 0.0f);
			}
		}
		alphaOverLifeDist->isDirty_ = true;
	}
}

/*-----------------------------------------------------------------------------
	ParticleModuleColorScaleOverLife implementation.
-----------------------------------------------------------------------------*/
ParticleModuleColorScaleOverLife::ParticleModuleColorScaleOverLife()
{
	spawnModule_ = true;
	updateModule_ = true;
	curvesAsColor_ = true;
}

void ParticleModuleColorScaleOverLife::InitializeDefaults()
{
	if (!colorScaleOverLife_.IsCreated())
	{
		colorScaleOverLife_.distribution_ = MakeShared<DistributionVectorConstantCurve>();
	}

	if (!alphaScaleOverLife_.IsCreated())
	{
		auto distributionAlphaScaleOverLife = MakeShared<DistributionFloatConstant>();
		distributionAlphaScaleOverLife->constant_ = 1.0f;
		alphaScaleOverLife_.distribution_ = distributionAlphaScaleOverLife;
	}
}

void ParticleModuleColorScaleOverLife::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	bool ScaleColor = true;
	bool ScaleAlpha = true;

	if (IsUsedInGPUEmitter())
	{
		if (colorScaleOverLife_.distribution_->IsInstanceOf(DistributionVectorParticleParameter::GetTypeStatic()))
		{
			emitterInfo.dynamicColorScale_ = colorScaleOverLife_;
#if WITH_EDITOR
			emitterInfo.dynamicColorScale_.distribution_->isDirty_ = true;
			emitterInfo.dynamicColorScale_.Initialize();
#endif
			ScaleColor = false;
		}

		if (alphaScaleOverLife_.distribution_->IsInstanceOf(DistributionFloatParticleParameter::GetTypeStatic()))
		{
			emitterInfo.dynamicAlphaScale_ = alphaScaleOverLife_;
#if WITH_EDITOR
			emitterInfo.dynamicAlphaScale_.distribution_->isDirty_ = true;
			emitterInfo.dynamicAlphaScale_.Initialize();
#endif
			ScaleAlpha = false;
		}
	}

	if (ScaleColor)
	{
		emitterInfo.colorScale_.ScaleByVectorDistribution(colorScaleOverLife_.distribution_);
	}

	if (ScaleAlpha)
	{
		emitterInfo.alphaScale_.ScaleByDistribution(alphaScaleOverLife_.distribution_);
	}
}

void ParticleModuleColorScaleOverLife::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SPAWN_INIT;
	Vector3 colorVec;
	float	fAlpha;

	if (emitterTime_)
	{
		colorVec = colorScaleOverLife_.GetValue(owner->emitterTime_, owner->component_);
		fAlpha = alphaScaleOverLife_.GetValue(owner->emitterTime_, owner->component_);
	}
	else
	{
		colorVec = colorScaleOverLife_.GetValue(particle.relativeTime_, owner->component_);
		fAlpha = alphaScaleOverLife_.GetValue(particle.relativeTime_, owner->component_);
	}

	particle.color_.r_ *= colorVec.x_;
	particle.color_.g_ *= colorVec.y_;
	particle.color_.b_ *= colorVec.z_;
	particle.color_.a_ *= fAlpha;
}

void ParticleModuleColorScaleOverLife::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	const RawDistribution* fastColorScaleOverLife = colorScaleOverLife_.GetFastRawDistribution();
	const RawDistribution* fastAlphaScaleOverLife = alphaScaleOverLife_.GetFastRawDistribution();
	Vector3 colorVec;	// LWC_TODO: WRONG? Force colorVec to floats for serialization. particle.Color is Vector3. Update to Vector3 also?
	float	fAlpha;
	if (fastColorScaleOverLife && fastAlphaScaleOverLife)
	{
		// fast path
		if (emitterTime_)
		{
			BEGIN_UPDATE_LOOP;
			{
				fastColorScaleOverLife->GetValue3None(owner->emitterTime_, &colorVec.x_);
				fastAlphaScaleOverLife->GetValue1None(owner->emitterTime_, &fAlpha);
				particle.color_.r_ *= colorVec.x_;
				particle.color_.g_ *= colorVec.y_;
				particle.color_.b_ *= colorVec.z_;
				particle.color_.a_ *= fAlpha;
			}
			END_UPDATE_LOOP;
		}
		else
		{
			BEGIN_UPDATE_LOOP;
			{
				fastColorScaleOverLife->GetValue3None(particle.relativeTime_, &colorVec.x_);
				fastAlphaScaleOverLife->GetValue1None(particle.relativeTime_, &fAlpha);
				particle.color_.r_ *= colorVec.x_;
				particle.color_.g_ *= colorVec.y_;
				particle.color_.b_ *= colorVec.z_;
				particle.color_.a_ *= fAlpha;
			}
			END_UPDATE_LOOP;
		}
	}
	else
	{
		if (emitterTime_)
		{
			BEGIN_UPDATE_LOOP;
			colorVec = Vector3(colorScaleOverLife_.GetValue(owner->emitterTime_, owner->component_));
			fAlpha = alphaScaleOverLife_.GetValue(owner->emitterTime_, owner->component_);
			particle.color_.r_ *= colorVec.x_;
			particle.color_.g_ *= colorVec.y_;
			particle.color_.b_ *= colorVec.z_;
			particle.color_.a_ *= fAlpha;
			END_UPDATE_LOOP;
		}
		else
		{
			BEGIN_UPDATE_LOOP;
			colorVec = Vector3(colorScaleOverLife_.GetValue(particle.relativeTime_, owner->component_));
			fAlpha = alphaScaleOverLife_.GetValue(particle.relativeTime_, owner->component_);
			particle.color_.r_ *= colorVec.x_;
			particle.color_.g_ *= colorVec.y_;
			particle.color_.b_ *= colorVec.z_;
			particle.color_.a_ *= fAlpha;
			END_UPDATE_LOOP;
		}
	}
}

void ParticleModuleColorScaleOverLife::SetToSensibleDefaults(ParticleEmitter* owner)
{
	
	auto colorScaleOverLifeDist = MakeShared<DistributionVectorConstantCurve>();
	colorScaleOverLife_.distribution_ = colorScaleOverLifeDist;
	if (colorScaleOverLifeDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 key = 0; key < 2; key++)
		{
			Int32 keyIndex = colorScaleOverLifeDist->CreateNewKey(key * 1.0f);
			for (Int32 subIndex = 0; subIndex < 3; subIndex++)
			{
				colorScaleOverLifeDist->SetKeyOut(subIndex, keyIndex, 1.0f);
			}
		}
		colorScaleOverLifeDist->isDirty_ = true;
	}
}

}
