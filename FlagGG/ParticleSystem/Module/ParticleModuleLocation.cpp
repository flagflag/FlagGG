#include "ParticleModuleLocation.h"
#include "Math/Distributions/DistributionFloatUniform.h"
#include "Math/Distributions/DistributionVectorUniform.h"
#include "Math/Distributions/DistributionFloatConstant.h"
#include "Math/Distributions/DistributionVectorConstant.h"
#include "ParticleSystem/ParticleEmitterInstances.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Core/ObjectFactory.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(ParticleModulePivotOffset);
REGISTER_TYPE_FACTORY(ParticleModuleLocation);
REGISTER_TYPE_FACTORY(ParticleModuleLocationPrimitiveCylinder);
REGISTER_TYPE_FACTORY(ParticleModuleLocationPrimitiveSphere);

/*-----------------------------------------------------------------------------
	ParticleModulePivotOffset implementation.
-----------------------------------------------------------------------------*/

ParticleModulePivotOffset::ParticleModulePivotOffset()
{
	spawnModule_ = false;
	updateModule_ = false;
}

void ParticleModulePivotOffset::InitializeDefaults()
{
	pivotOffset_ = Vector2(0.0f, 0.0f);
}

void ParticleModulePivotOffset::CompileModule(ParticleEmitterBuildInfo& emitterInfo)
{
	emitterInfo.pivotOffset_ = pivotOffset_ - Vector2(0.5f, 0.5f);
}

bool ParticleModulePivotOffset::LoadXML(const XMLElement& root)
{
	return true;
}

bool ParticleModulePivotOffset::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}


/*-----------------------------------------------------------------------------
	ParticleModuleLocation implementation.
-----------------------------------------------------------------------------*/

ParticleModuleLocation::ParticleModuleLocation()
{
	spawnModule_ = true;
	supported3DDrawMode_ = true;
	distributeOverNPoints_ = 0.0f;
}

void ParticleModuleLocation::InitializeDefaults()
{
	if (!startLocation_.IsCreated())
	{
		startLocation_.distribution_ = MakeShared<DistributionVectorUniform>();
	}
}

void ParticleModuleLocation::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleLocation::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, struct RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;
	ParticleLODLevel* LODLevel	= owner->spriteTemplate_->GetCurrentLODLevel(owner);
	ASSERT(LODLevel);
	Vector3 locationOffset;

	// Avoid divide by zero.
	if ((distributeOverNPoints_ != 0.0f) && (distributeOverNPoints_ != 1.f))
	{
		float RandomNum = inRandomStream->FRand() * Fract(owner->emitterTime_);

		if(RandomNum > distributeThreshold_)
		{
			locationOffset = startLocation_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
		}
		else
		{
			Vector3 min, max;
			startLocation_.GetRange(min, max);
			Vector3 lerped = Lerp(min, max, Floor((inRandomStream->FRand() * (distributeOverNPoints_ - 1.0f)) + 0.5f)/(distributeOverNPoints_ - 1.0f));
			locationOffset.Set(lerped.x_, lerped.y_, lerped.z_);
		}
	}
	else
	{
		locationOffset = startLocation_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);
	}

	locationOffset = owner->emitterToSimulation_ * locationOffset;
	particle.location_ += locationOffset;
	CRY_ENSURE(!particle.location_.ContainsNaN(), "NaN in Particle Location. Template: %s", owner->component_ ? owner->component_->template_->GetName().CString() : "UNKNOWN");
}

bool ParticleModuleLocation::LoadXML(const XMLElement& root)
{
	if (XMLElement startLocationNode = root.GetChild("startLocation"))
	{
		const String curveType = startLocationNode.GetAttribute("type");

		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionVectorUniform>();
			uniformCurve->min_ = startLocationNode.GetChild("min").GetVector3("value");
			uniformCurve->max_ = startLocationNode.GetChild("max").GetVector3("value");
		}

		return true;
	}

	return false;
}

bool ParticleModuleLocation::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

/*-----------------------------------------------------------------------------
	ParticleModuleLocationPrimitiveBase implementation.
-----------------------------------------------------------------------------*/
ParticleModuleLocationPrimitiveBase::ParticleModuleLocationPrimitiveBase()
{
	spawnModule_ = true;
	positive_X_ = true;
	positive_Y_ = true;
	positive_Z_ = true;
	negative_X_ = true;
	negative_Y_ = true;
	negative_Z_ = true;
	surfaceOnly_ = false;
	velocity_ = false;
}

void ParticleModuleLocationPrimitiveBase::InitializeDefaults()
{
	if (!velocityScale_.IsCreated())
	{
		auto distributionVelocityScale = MakeShared<DistributionFloatConstant>();
		distributionVelocityScale->constant_ = 1.0f;
		velocityScale_.distribution_ = distributionVelocityScale;
	}

	if (!startLocation_.IsCreated())
	{
		auto distributionStartLocation = MakeShared<DistributionVectorConstant>();
		distributionStartLocation->constant_ = Vector3::ZERO;
		startLocation_.distribution_ = distributionStartLocation;
	}
}

void ParticleModuleLocationPrimitiveBase::DetermineUnitDirection(ParticleEmitterInstance* owner, Vector3& vUnitDir, struct RandomStream* inRandomStream)
{
	Vector3 vRand;

	// Grab 3 random numbers for the axes
	vRand.x_ = inRandomStream->GetFraction();
	vRand.y_ = inRandomStream->GetFraction();
	vRand.z_ = inRandomStream->GetFraction();

	// Set the unit dir
	if (positive_X_ && negative_X_)
	{
		vUnitDir.x_ = vRand.x_ * 2 - 1;
	}
	else if (positive_X_)
	{
		vUnitDir.x_ = vRand.x_;
	}
	else if (negative_X_)
	{
		vUnitDir.x_ = -vRand.x_;
	}
	else
	{
		vUnitDir.x_ = 0.0f;
	}

	if (positive_Y_ && negative_Y_)
	{
		vUnitDir.y_ = vRand.y_ * 2 - 1;
	}
	else if (positive_Y_)
	{
		vUnitDir.y_ = vRand.y_;
	}
	else if (negative_Y_)
	{
		vUnitDir.y_ = -vRand.y_;
	}
	else
	{
		vUnitDir.y_ = 0.0f;
	}

	if (positive_Z_ && negative_Z_)
	{
		vUnitDir.z_ = vRand.z_ * 2 - 1;
	}
	else if (positive_Z_)
	{
		vUnitDir.z_ = vRand.z_;
	}
	else if (negative_Z_)
	{
		vUnitDir.z_ = -vRand.z_;
	}
	else
	{
		vUnitDir.z_ = 0.0f;
	}
}

bool ParticleModuleLocationPrimitiveBase::LoadXML(const XMLElement& root)
{
	if (XMLElement velocityScaleNode = root.GetChild("velocityScale"))
	{
		const String curveType = velocityScaleNode.GetAttribute("type");
		if (curveType == "uniform")
		{
			auto uniformCurve = MakeShared<DistributionFloatUniform>();
			uniformCurve->min_ = velocityScaleNode.GetChild("min").GetFloat("value");
			uniformCurve->max_ = velocityScaleNode.GetChild("max").GetFloat("value");
			velocityScale_.distribution_ = uniformCurve;
		}
	}

	if (XMLElement startLocationNode = root.GetChild("startLocation"))
	{
		const String curveType = startLocationNode.GetAttribute("type");
		if (curveType == "constant")
		{
			auto constant = MakeShared<DistributionVectorConstant>();
			constant->constant_ = startLocationNode.GetChild("constant").GetVector3("value");
			startLocation_.distribution_ = constant;
		}
	}

	return true;
}

bool ParticleModuleLocationPrimitiveBase::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

/*-----------------------------------------------------------------------------
	ParticleModuleLocationPrimitiveCylinder implementation.
-----------------------------------------------------------------------------*/
ParticleModuleLocationPrimitiveCylinder::ParticleModuleLocationPrimitiveCylinder()
{
	radialVelocity_ = true;
	supported3DDrawMode_ = true;
	heightAxis_ = PMLPC_HEIGHTAXIS_Z;
}

void ParticleModuleLocationPrimitiveCylinder::InitializeDefaults()
{
	if (!startRadius_.IsCreated())
	{
		auto distributionStartRadius = MakeShared<DistributionFloatConstant>();
		distributionStartRadius->constant_ = 50.0f;
		startRadius_.distribution_ = distributionStartRadius;
	}

	if (!startHeight_.IsCreated())
	{
		auto distributionStartHeight = MakeShared<DistributionFloatConstant>();
		distributionStartHeight->constant_ = 50.0f;
		startHeight_.distribution_ = distributionStartHeight;
	}
}

void ParticleModuleLocationPrimitiveCylinder::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleLocationPrimitiveCylinder::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;

	Int32 radialIndex0 = 0;	//X
	Int32 radialIndex1 = 1;	//Y
	Int32 heightIndex = 2;	//Z

	switch (heightAxis_)
	{
	case PMLPC_HEIGHTAXIS_X:
		radialIndex0 = 1;	//Y
		radialIndex1 = 2;	//Z
		heightIndex = 0;	//X
		break;
	case PMLPC_HEIGHTAXIS_Y:
		radialIndex0 = 0;	//X
		radialIndex1 = 2;	//Z
		heightIndex = 1;	//Y
		break;
	case PMLPC_HEIGHTAXIS_Z:
		break;
	}

	// Determine the start location for the sphere
	Vector3 vStartLoc = startLocation_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);

	Vector3 vOffset;
	float fStartRadius = startRadius_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
	float fStartHeight = startHeight_.GetValue(owner->emitterTime_, owner->component_, inRandomStream) / 2.0f;


	// Determine the unit direction
	Vector3 vUnitDir, vUnitDirTemp;

	bool foundValidValue = false;
	Int32 numberOfAttempts = 0;
	float radiusSquared = fStartRadius * fStartRadius;
	while (!foundValidValue)
	{
		DetermineUnitDirection(owner, vUnitDirTemp, inRandomStream);
		vUnitDir[radialIndex0] = vUnitDirTemp[radialIndex0];
		vUnitDir[radialIndex1] = vUnitDirTemp[radialIndex1];
		vUnitDir[heightIndex] = vUnitDirTemp[heightIndex];

		Vector2 checkVal(vUnitDir[radialIndex0] * fStartRadius, vUnitDir[radialIndex1] * fStartRadius);
		if (checkVal.LengthSquared() <= radiusSquared)
		{
			foundValidValue = true;
		}
		else if (numberOfAttempts >= 50)
		{
			// Just pass the value thru. 
			// It will clamp to the 'circle' but we tried...
			foundValidValue = true;
		}
		numberOfAttempts++;
	}

	Vector3 vNormalizedDir = vUnitDir;
	vNormalizedDir.Normalize();

	Vector2 vUnitDir2D(vUnitDir[radialIndex0], vUnitDir[radialIndex1]);
	Vector2 vNormalizedDir2D = vUnitDir2D.Normalized();

	// Determine the position
	// Always want Z in the [-Height, Height] range
	vOffset[heightIndex] = vUnitDir[heightIndex] * fStartHeight;

	vNormalizedDir[radialIndex0] = vNormalizedDir2D.x_;
	vNormalizedDir[radialIndex1] = vNormalizedDir2D.y_;

	if (surfaceOnly_)
	{
		// Clamp the X,Y to the outer edge...
		if (IsNearlyZero(Abs(vOffset[heightIndex]) - fStartHeight))
		{
			// On the caps, it can be anywhere within the 'circle'
			vOffset[radialIndex0] = vUnitDir[radialIndex0] * fStartRadius;
			vOffset[radialIndex1] = vUnitDir[radialIndex1] * fStartRadius;
		}
		else
		{
			// On the sides, it must be on the 'circle'
			vOffset[radialIndex0] = vNormalizedDir[radialIndex0] * fStartRadius;
			vOffset[radialIndex1] = vNormalizedDir[radialIndex1] * fStartRadius;
		}
	}
	else
	{
		vOffset[radialIndex0] = vUnitDir[radialIndex0] * fStartRadius;
		vOffset[radialIndex1] = vUnitDir[radialIndex1] * fStartRadius;
	}

	// Clamp to the radius...
	Vector3	vMax;

	vMax[radialIndex0] = Abs(vNormalizedDir[radialIndex0]) * fStartRadius;
	vMax[radialIndex1] = Abs(vNormalizedDir[radialIndex1]) * fStartRadius;
	vMax[heightIndex] = fStartHeight;

	vOffset[radialIndex0] = Clamp<float>(vOffset[radialIndex0], -vMax[radialIndex0], vMax[radialIndex0]);
	vOffset[radialIndex1] = Clamp<float>(vOffset[radialIndex1], -vMax[radialIndex1], vMax[radialIndex1]);
	vOffset[heightIndex] = Clamp<float>(vOffset[heightIndex], -vMax[heightIndex], vMax[heightIndex]);

	// Add in the start location
	vOffset[radialIndex0] += vStartLoc[radialIndex0];
	vOffset[radialIndex1] += vStartLoc[radialIndex1];
	vOffset[heightIndex] += vStartLoc[heightIndex];

	particle.location_ += owner->emitterToSimulation_ * vOffset;

	if (velocity_)
	{
		Vector3 vVelocity;
		vVelocity[radialIndex0] = vOffset[radialIndex0] - vStartLoc[radialIndex0];
		vVelocity[radialIndex1] = vOffset[radialIndex1] - vStartLoc[radialIndex1];
		vVelocity[heightIndex] = vOffset[heightIndex] - vStartLoc[heightIndex];

		if (radialVelocity_)
		{
			vVelocity[heightIndex] = 0.0f;
		}
		vVelocity *= velocityScale_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
		vVelocity = owner->emitterToSimulation_ * vVelocity;

		particle.velocity_ += vVelocity;
		particle.baseVelocity_ += vVelocity;
	}
	CRY_ENSURE(!particle.location_.ContainsNaN(), "NaN in Particle Location. Template: %s", owner->component_ ? owner->component_->template_->GetName().CString() : "UNKNOWN");
	CRY_ENSURE(!particle.velocity_.ContainsNaN(), "NaN in Particle Velocity. Template: %s", owner->component_ ? owner->component_->template_->GetName().CString() : "UNKNOWN");
}

bool ParticleModuleLocationPrimitiveCylinder::LoadXML(const XMLElement& root)
{
	if (XMLElement superModule = root.GetChild("module"))
	{
		if (superModule.GetAttribute("type") != "ParticleModuleLocationPrimitiveBase")
		{
			return false;
		}

		if (!ParticleModuleLocationPrimitiveBase::LoadXML(superModule))
		{
			return false;
		}
	}

	if (XMLElement radialVelocityNode = root.GetChild("radialVelocity"))
	{
		radialVelocity_ = radialVelocityNode.GetBool("value");
	}

	if (XMLElement adjustForWorldSpaceNode = root.GetChild("adjustForWorldSpace"))
	{
		
	}

	if (XMLElement heightAxisNode = root.GetChild("heightAxis"))
	{
		heightAxis_ = CylinderHeightAxis(heightAxisNode.GetUInt("value"));
	}

	if (XMLElement startRadiusNode = root.GetChild("startRadius"))
	{
		const String curveType = startRadiusNode.GetAttribute("type");
		if (curveType == "constant")
		{
			auto constant = MakeShared<DistributionFloatConstant>();
			constant->constant_ = startRadiusNode.GetChild("constant").GetFloat("value");
			startRadius_.distribution_ = constant;
		}
	}

	if (XMLElement startHeightNode = root.GetChild("startHeight"))
	{
		const String curveType = startHeightNode.GetAttribute("type");
		if (curveType == "constant")
		{
			auto constant = MakeShared<DistributionFloatConstant>();
			constant->constant_ = startHeightNode.GetChild("constant").GetFloat("value");
			startHeight_.distribution_ = constant;
		}
	}

	return true;
}

bool ParticleModuleLocationPrimitiveCylinder::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

/*-----------------------------------------------------------------------------
	ParticleModuleLocationPrimitiveSphere implementation.
-----------------------------------------------------------------------------*/
ParticleModuleLocationPrimitiveSphere::ParticleModuleLocationPrimitiveSphere()
{
	supported3DDrawMode_ = true;
}

void ParticleModuleLocationPrimitiveSphere::InitializeDefaults()
{
	if (!startRadius_.IsCreated())
	{
		auto distributionStartRadius = MakeShared<DistributionFloatConstant>();
		distributionStartRadius->constant_ = 50.0f;
		startRadius_.distribution_ = distributionStartRadius;
	}
}

void ParticleModuleLocationPrimitiveSphere::Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase)
{
	SpawnEx(owner, offset, spawnTime, &GetRandomStream(owner), particleBase);
}

void ParticleModuleLocationPrimitiveSphere::SpawnEx(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, RandomStream* inRandomStream, BaseParticle* particleBase)
{
	SPAWN_INIT;

	// Determine the start location for the sphere
	Vector3 vStartLoc = startLocation_.GetValue(owner->emitterTime_, owner->component_, 0, inRandomStream);

	// Determine the unit direction
	Vector3 vUnitDir;
	DetermineUnitDirection(owner, vUnitDir, inRandomStream);

	Vector3 vNormalizedDir = vUnitDir;
	vNormalizedDir.Normalize();

	// If we want to cover just the surface of the sphere...
	if (surfaceOnly_)
	{
		vUnitDir.Normalize();
	}

	// Determine the position
	float fStartRadius = startRadius_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
	Vector3 vStartRadius(fStartRadius, fStartRadius, fStartRadius);
	Vector3 vOffset = vUnitDir * vStartRadius;

	// Clamp to the radius...
	Vector3	vMax;

	vMax.x_ = Abs(vNormalizedDir.x_) * fStartRadius;
	vMax.y_ = Abs(vNormalizedDir.y_) * fStartRadius;
	vMax.z_ = Abs(vNormalizedDir.z_) * fStartRadius;

	if (positive_X_ || negative_X_)
	{
		vOffset.x_ = Clamp<float>(vOffset.x_, -vMax.x_, vMax.x_);
	}
	else
	{
		vOffset.x_ = 0.0f;
	}
	if (positive_Y_ || negative_Y_)
	{
		vOffset.y_ = Clamp<float>(vOffset.y_, -vMax.y_, vMax.y_);
	}
	else
	{
		vOffset.y_ = 0.0f;
	}
	if (positive_Z_ || negative_Z_)
	{
		vOffset.z_ = Clamp<float>(vOffset.z_, -vMax.z_, vMax.z_);
	}
	else
	{
		vOffset.z_ = 0.0f;
	}

	vOffset += vStartLoc;
	particle.location_ += owner->emitterToSimulation_ * vOffset;

	if (velocity_)
	{
		Vector3 vVelocity = (vOffset - vStartLoc) * velocityScale_.GetValue(owner->emitterTime_, owner->component_, inRandomStream);
		vVelocity = owner->emitterToSimulation_ * vVelocity;
		particle.velocity_ += vVelocity;
		particle.baseVelocity_ += vVelocity;
	}
	CRY_ENSURE(!particle.location_.ContainsNaN(), "NaN in Particle Location. Template: %s", owner->component_ ? owner->component_->template_->GetName().CString() : "UNKNOWN");
	CRY_ENSURE(!particle.velocity_.ContainsNaN(), "NaN in Particle Velocity. Template: %s", owner->component_ ? owner->component_->template_->GetName().CString() : "UNKNOWN");
}

bool ParticleModuleLocationPrimitiveSphere::LoadXML(const XMLElement& root)
{
	if (XMLElement superModule = root.GetChild("module"))
	{
		if (superModule.GetAttribute("type") != "ParticleModuleLocationPrimitiveBase")
		{
			return false;
		}

		if (!ParticleModuleLocationPrimitiveBase::LoadXML(superModule))
		{
			return false;
		}
	}

	if (XMLElement startRadiusNode = root.GetChild("startRadius"))
	{
		const String curveType = startRadiusNode.GetAttribute("type");
		if (curveType == "constant")
		{
			auto constant = MakeShared<DistributionFloatConstant>();
			constant->constant_ = startRadiusNode.GetChild("constant").GetFloat("value");
			startRadius_.distribution_ = constant;
		}
	}

	return true;
}

bool ParticleModuleLocationPrimitiveSphere::SaveXML(XMLElement& root)
{
	// TODO
	return false;
}

}
