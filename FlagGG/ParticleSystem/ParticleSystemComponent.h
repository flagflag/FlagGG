#pragma once

#include "Core/Context.h"
#include "Scene/DrawableComponent.h"
#include "Scene/Node.h"
#include "ParticleSystem/ParticleSystem.h"
#include "Math/RandomStream.h"
#include "Container/Ptr.h"

namespace FlagGG
{

struct ParticleEmitterInstance;

enum ParticleSysParamType
{
	PSPT_None,
	PSPT_Scalar,
	PSPT_ScalarRand,
	PSPT_Vector,
	PSPT_VectorRand,
	PSPT_Color,
	PSPT_Node,
	PSPT_Material,
	PSPT_VectorUnitRand,
	PSPT_MAX,
};

struct ParticleSysParam
{
	ParticleSysParam()
		: paramType_(0)
		, scalar_(0)
		, scalar_Low_(0)
		, asyncDataCopyIsValid_(false)
	{
	}

	void UpdateAsyncActorCache()
	{
		ASSERT(GetSubsystem<Context>()->IsInGameThread());
		if(node_)
		{
			asyncActorToWorld_ = node_->GetWorldTransform();
			// asyncActorVelocity_ = Actor->GetVelocity();
		}
		
		asyncDataCopyIsValid_ = true;
	}

	void ResetAsyncActorCache()
	{
		// ASSERT(IsInGameThread());
		asyncDataCopyIsValid_ = false;
	}

	Matrix3x4 GetAsyncActorToWorld() const
	{
		if(asyncDataCopyIsValid_)
		{
			return asyncActorToWorld_;
		}
		else if(node_)
		{
			ASSERT(GetSubsystem<Context>()->IsInGameThread());
			return node_->GetWorldTransform();
		}

		return Matrix3x4::IDENTITY;
	}

	Vector3 GetAsyncActorVelocity() const
	{
		if (asyncDataCopyIsValid_)
		{
			return asyncActorVelocity_;
		}
		else if (node_)
		{
			ASSERT(GetSubsystem<Context>()->IsInGameThread());
			// return node_->GetVelocity();
		}

		return Vector3::ZERO;
	}


	String name_;

	/**
	 *	The type of parameters
	 *	PSPT_None       - There is no data type
	 *	PSPT_Scalar     - Use the scalar value
	 *	PSPT_ScalarRand - Select a scalar value in the range [Scalar_Low..Scalar)
	 *	PSPT_Vector     - Use the vector value
	 *	PSPT_VectorRand - Select a vector value in the range [Vector_Low..Vector)
	 *	PSPT_Color      - Use the color value
	 *	PSPT_Actor      - Use the actor value
	 *	PSPT_Material   - Use the material value
	 *	PSPT_VectorUnitRand - Select a random unit vector and scale along the range [Vector_Low..Vector)
	 */
	EnumAsByte<ParticleSysParamType> paramType_;

	float scalar_;

	float scalar_Low_;

	Vector3 vector_;

	Vector3 vector_Low_;

	Color color_;

	SharedPtr<Node> node_;

	SharedPtr<Material> material_;

private:
	Matrix3x4 asyncActorToWorld_;
	Vector3 asyncActorVelocity_;
	bool asyncDataCopyIsValid_;

};

class FlagGG_API ParticleSystemComponent : public DrawableComponent
{
	OBJECT_OVERRIDE(ParticleSystemComponent, DrawableComponent);
public:
	ParticleSystemComponent();

	~ParticleSystemComponent() override;

	// Init system.
	void InitializeSystem();

	// If particles have not already been initialised (ie. emitterInstances_ created) do it now.
	void InitParticles();

	// Reset particles.
	void ResetParticles(bool clearInstances = false);

	// Reset burst particles.
	void ResetBurstLists();

	/**
	  * Decide which detail mode should be applied to this particle system. If we have an editor
	  * override specified, use that. Otherwise use the global cached value
	  */
	Int32 GetCurrentDetailMode() const;

	/**
	 *	Retrieve the Float parameter value for the given name.
	 *
	 *	@param	inName		Name of the parameter
	 *	@param	outFloat	The value of the parameter found
	 *
	 *	@return	true		Parameter was found - outFloat is valid
	 *			false		Parameter was not found - outFloat is invalid
	 */
	virtual bool GetFloatParameter(const String& inName, float& outFloat);

	/**
	 *	Retrieve the Vector parameter value for the given name.
	 *
	 *	@param	inName		Name of the parameter
	 *	@param	outVector	The value of the parameter found
	 *
	 *	@return	true		Parameter was found - outVector is valid
	 *			false		Parameter was not found - outVector is invalid
	 */
	virtual bool GetVectorParameter(const String& inName, Vector3& outVector);

	/**
	 *	Retrieve the Vector parameter value for the given name...also looks for colors and floats and returns those
	 *
	 *	@param	inName		Name of the parameter
	 *	@param	outVector	The value of the parameter found
	 *
	 *	@return	true		Parameter was found - outVector is valid
	 *			false		Parameter was not found - outVector is invalid
	 */
	virtual bool GetAnyVectorParameter(const String& inName, Vector3& outVector);

	/**
	 *	Retrieve the Color parameter value for the given name.
	 *
	 *	@param	inName		Name of the parameter
	 *	@param	outColor	The value of the parameter found
	 *
	 *	@return	true		Parameter was found - outColor is valid
	 *			false		Parameter was not found - outColor is invalid
	 */
	virtual bool GetColorParameter(const String& inName, Color& outColor);

	/**
	 *	Retrieve the Actor parameter value for the given name.
	 *
	 *	@param	inName		Name of the parameter
	 *	@param	outNode	The value of the parameter found
	 *
	 *	@return	true		Parameter was found - outNode is valid
	 *			false		Parameter was not found - outNode is invalid
	 */
	virtual bool GetNodeParameter(const String& inName, class Node*& outNode);

	/**
	 *	Retrieve the Material parameter value for the given name.
	 *
	 *	@param	inName		Name of the parameter
	 *	@param	outMaterial	The value of the parameter found
	 *
	 *	@return	true		Parameter was found - outMaterial is valid
	 *			false		Parameter was not found - outMaterial is invalid
	 */
	virtual bool GetMaterialParameter(const String& inName, Material*& outMaterial);

	// 是否可渲染
	bool IsRenderable() override { return true; }

	// 获取DrawableFlags
	UInt32 GetDrawableFlags() const override { return DRAWABLE_GEOMETRY; }

	// 更新包围盒
	void OnUpdateWorldBoundingBox() override;

	// 帧更新
	void Update(Real timeStep) override;


	SharedPtr<ParticleSystem> template_;

	Vector<SharedPtr<Material>> emitterMaterials_;

	UInt8 wasCompleted_ : 1;

	/** Indicates that the component has not been ticked since being registered. */
	UInt8 justRegistered_ : 1;

	UInt8 warmingUp_ : 1;

	/**
	 * WarmupTime - the time to warm-up the particle system when first rendered
	 * Warning: WarmupTime is implemented by simulating the particle system for the time requested upon activation.
	 * This is extremely prone to cause hitches, especially with large particle counts - use with caution.
	 */
	float warmupTime_;

	/**	WarmupTickRate - the time step for each tick during warm up.
		Increasing this improves performance. Decreasing, improves accuracy.
		Set to 0 to use the default tick time.										*/
	float warmupTickRate_;

	/** This is created at start up and then added to each emitter */
	float emitterDelay_;

	/** Stream of random values to use with this component */
	RandomStream randomStream_;

private:
	Int32 LODLevel_;

	Vector<ParticleSysParam> instanceParameters_;

	Vector<ParticleEmitterInstance*> emitterInstances_;
};

}
