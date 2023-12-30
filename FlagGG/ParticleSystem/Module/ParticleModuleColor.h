#pragma once

#include "ParticleSystem/ParticleModule.h"
#include "Math/Distributions/DistributionFloat.h"
#include "Math/Distributions/DistributionVector.h"

namespace FlagGG
{

class InterpCurveEdSetup;
class ParticleEmitter;
struct CurveEdEntry;
struct ParticleEmitterInstance;

class FlagGG_API ParticleModuleColorBase : public ParticleModule
{
	OBJECT_OVERRIDE(ParticleModuleColorBase, ParticleModule);
public:
	ParticleModuleColorBase();
};

class ParticleModuleColorOverLife : public ParticleModuleColorBase
{
	OBJECT_OVERRIDE(ParticleModuleColorOverLife, ParticleModuleColorBase);
public:
	ParticleModuleColorOverLife();

	/** Initializes the default values for this property */
	void InitializeDefaults();


	//Begin UParticleModule Interface
	virtual	bool AddModuleCurvesToEditor(InterpCurveEdSetup* edSetup, PODVector<const CurveEdEntry*>& outCurveEntries) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual void CompileModule(struct ParticleEmitterBuildInfo& EmitterInfo) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* Owner) override;
	//End UParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/** The color to apply to the particle, as a function of the particle RelativeTime. */
	RawDistributionVector colorOverLife_;

	/** The alpha to apply to the particle, as a function of the particle RelativeTime. */
	RawDistributionFloat alphaOverLife_;

	/** If true, the alpha value will be clamped to the [0..1] range. */
	UInt32 clampAlpha_ : 1;
};

class ParticleModuleColorScaleOverLife : public ParticleModuleColorBase
{
	OBJECT_OVERRIDE(ParticleModuleColorScaleOverLife, ParticleModuleColorBase);
public:
	ParticleModuleColorScaleOverLife();

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin UParticleModule Interface
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo) override;
	virtual void Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase) override;
	virtual void Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime) override;
	virtual void SetToSensibleDefaults(ParticleEmitter* owner) override;
	//End UParticleModule Interface

	// 从XML加载
	bool LoadXML(const XMLElement& root) override;

	// 保存到XML中
	bool SaveXML(XMLElement& root) override;


	/** The scale factor for the color.													*/
	RawDistributionVector colorScaleOverLife_;

	/** The scale factor for the alpha.													*/
	RawDistributionFloat alphaScaleOverLife_;

	/** Whether it is EmitterTime or ParticleTime related.								*/
	UInt32 emitterTime_ : 1;
};


}
