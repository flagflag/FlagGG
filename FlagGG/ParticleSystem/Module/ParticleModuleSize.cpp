#include "ParticleModuleSize.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Math/Distributions/DistributionVectorConstant.h"
#include "Math/Distributions/DistributionVectorConstantCurve.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleLODLevel.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModuleSize);
REGISTER_TYPE_FACTORY(ParticleModuleSizeMultiplyLife);

/*-----------------------------------------------------------------------------
	Abstract base modules used for categorization.
-----------------------------------------------------------------------------*/
ParticleModuleSizeBase::ParticleModuleSizeBase()
{
}

/*-----------------------------------------------------------------------------
	ParticleModuleSize implementation.
-----------------------------------------------------------------------------*/

ParticleModuleSize::ParticleModuleSize()
{
	spawnModule_ = true;
	updateModule_ = false;
}

void ParticleModuleSize::InitializeDefaults()
{
	if (!startSize_.IsCreated())
	{
		SharedPtr<DistributionVectorUniform> distributionStartSize = MakeShared<DistributionVectorUniform>();
		distributionStartSize->min_ = Vector3(1.0f, 1.0f, 1.0f);
		distributionStartSize->max_ = Vector3(1.0f, 1.0f, 1.0f);
		startSize_.distribution_ = distributionStartSize;
	}
}

void ParticleModuleSize::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	float minSize = 0.0f;
	float maxSize = 0.0f;
	startSize_.GetValue();
	startSize_.GetOutRange(minSize, maxSize);
	emitterInfo.maxSize_.x_ *= maxSize;
	emitterInfo.maxSize_.y_ *= maxSize;
	emitterInfo.spawnModules_.Push(this);
	emitterInfo.sizeScale_.InitializeWithConstant(Vector3(1.0f, 1.0f, 1.0f));
}

void ParticleModuleSize::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleSize::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	Vector3 size  = startSize_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
	particle.size_ += size;

	AdjustParticleBaseSizeForUVFlipping(size, owner->currentLODLevel_->requiredModule_->UVFlippingMode_, *inRandomStream);
	particle.baseSize_ += size;
}

bool ParticleModuleSize::LoadXML(const XMLElement& root)
{
	if (XMLElement startSizeNode = root.GetChild("startSize"))
	{
		const String curveType = startSizeNode.GetAttribute("type");
		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionVectorUniform>();
			uniformCurve->min_ = startSizeNode.GetChild("min").GetVector3("value");
			uniformCurve->max_ = startSizeNode.GetChild("max").GetVector3("value");
			startSize_.distribution_ = uniformCurve;
		}
	}

	return true;
}

// 保存到XML中
bool ParticleModuleSize::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

/*-----------------------------------------------------------------------------
	ParticleModuleSizeMultiplyLife implementation.
-----------------------------------------------------------------------------*/
ParticleModuleSizeMultiplyLife::ParticleModuleSizeMultiplyLife()
{
	spawnModule_ = true;
	updateModule_ = true;
	multiplyX_ = true;
	multiplyY_ = true;
	multiplyZ_ = true;
}

void ParticleModuleSizeMultiplyLife::InitializeDefaults()
{
	if (!lifeMultiplier_.IsCreated())
	{
		lifeMultiplier_.distribution_ = MakeShared<DistributionVectorConstant>();
	}
}

void ParticleModuleSizeMultiplyLife::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	Vector3 axisScaleMask(
		multiplyX_ ? 1.0f : 0.0f,
		multiplyY_ ? 1.0f : 0.0f,
		multiplyZ_ ? 1.0f : 0.0f
	);
	Vector3 axisKeepMask(
		1.0f - axisScaleMask.x_,
		1.0f - axisScaleMask.y_,
		1.0f - axisScaleMask.z_
	);
	emitterInfo.sizeScale_.Initialize(lifeMultiplier_.distribution_);
	emitterInfo.sizeScale_.ScaleByConstantVector(axisScaleMask);
	emitterInfo.sizeScale_.AddConstantVector(axisKeepMask);
}

void ParticleModuleSizeMultiplyLife::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SPAWN_INIT;
	Vector3 sizeScale = lifeMultiplier_.GetValue(particle.relativeTime_, owner->component_);
	if (multiplyX_)
	{
		particle.size_.x_ *= sizeScale.x_;
	}
	if (multiplyY_)
	{
		particle.size_.y_ *= sizeScale.y_;
	}
	if (multiplyZ_)
	{
		particle.size_.z_ *= sizeScale.z_;
	}
}

void ParticleModuleSizeMultiplyLife::Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime)
{
	if ((owner == NULL) || (owner->activeParticles_ <= 0) ||
		(owner->particleData_ == NULL) || (owner->particleIndices_ == NULL))
	{
		return;
	}
	const RawDistribution* fastDistribution = lifeMultiplier_.GetFastRawDistribution();
	// PlatformMisc::Prefetch(owner->particleData_, (owner->particleIndices_[0] * owner->particleStride_));
	// PlatformMisc::Prefetch(owner->particleData_, (owner->particleIndices_[0] * owner->particleStride_) + PLATFORM_CACHE_LINE_SIZE);
	if (multiplyX_ && multiplyY_ && multiplyZ_)
	{
		if (fastDistribution)
		{
			Vector3 sizeScale;
			// fast path
			BEGIN_UPDATE_LOOP;
			fastDistribution->GetValue3None(particle.relativeTime_, &sizeScale.x_);
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
			// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
			particle.size_.x_ *= sizeScale.x_;
			particle.size_.y_ *= sizeScale.y_;
			particle.size_.z_ *= sizeScale.z_;
			END_UPDATE_LOOP;
		}
		else
		{
			BEGIN_UPDATE_LOOP
			{
				Vector3 sizeScale(lifeMultiplier_.GetValue(particle.relativeTime_, owner->component_));
				// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
				// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
				particle.size_.x_ *= sizeScale.x_;
				particle.size_.y_ *= sizeScale.y_;
				particle.size_.z_ *= sizeScale.z_;
			}
			END_UPDATE_LOOP;
		}
	}
	else
	{
		if (
			(multiplyX_ && !multiplyY_ && !multiplyZ_) ||
			(!multiplyX_ && multiplyY_ && !multiplyZ_) ||
			(!multiplyX_ && !multiplyY_ && multiplyZ_)
			)
		{
			Int32 index = multiplyX_ ? 0 : (multiplyY_ ? 1 : 2);
			BEGIN_UPDATE_LOOP
			{
				Vector3 sizeScale = lifeMultiplier_.GetValue(particle.relativeTime_, owner->component_);
				// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
				// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
				particle.size_[index] *= sizeScale[index];
			}
			END_UPDATE_LOOP;
		}
		else
		{
			BEGIN_UPDATE_LOOP
			{
				Vector3 sizeScale(lifeMultiplier_.GetValue(particle.relativeTime_, owner->component_));
				// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride));
				// PlatformMisc::Prefetch(particleData, (particleIndices[i + 1] * particleStride) + PLATFORM_CACHE_LINE_SIZE);
				if (multiplyX_)
				{
					particle.size_.x_ *= sizeScale.x_;
				}
				if (multiplyY_)
				{
					particle.size_.y_ *= sizeScale.y_;
				}
				if (multiplyZ_)
				{
					particle.size_.z_ *= sizeScale.z_;
				}
			}
			END_UPDATE_LOOP;
		}
	}
}

void ParticleModuleSizeMultiplyLife::SetToSensibleDefaults(ParticleEmitter* owner)
{
	auto lifeMultiplierDist = MakeShared<DistributionVectorConstantCurve>();
	lifeMultiplier_.distribution_ = lifeMultiplierDist;
	if (lifeMultiplierDist)
	{
		// Add two points, one at time 0.0f and one at 1.0f
		for (Int32 key = 0; key < 2; key++)
		{
			Int32 keyIndex = lifeMultiplierDist->CreateNewKey(key * 1.0f);
			for (Int32 subIndex = 0; subIndex < 3; subIndex++)
			{
				lifeMultiplierDist->SetKeyOut(subIndex, keyIndex, 1.0f);
			}
		}
		lifeMultiplierDist->isDirty_ = true;
	}
}

bool ParticleModuleSizeMultiplyLife::LoadXML(const XMLElement& root)
{
	if (XMLElement startSizeNode = root.GetChild("startSize"))
	{
		const String curveType = startSizeNode.GetAttribute("type");
		// TODO
	}

	return true;
}

// 保存到XML中
bool ParticleModuleSizeMultiplyLife::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
