#pragma once

#include "Core/Object.h"
#include "Core/BaseMacro.h"
#include "Container/Ptr.h"
#include "Container/Str.h"
#include "Math/Color.h"
#include "ParticleSystem/ParticleHelper.h"

namespace FlagGG
{

class Distribution;
class DistributionFloat;
class DistributionVector;
class InterpCurveEdSetup;
class ParticleEmitter;
class ParticleLODLevel;
class ParticleModuleTypeDataBase;
class ParticleSystemComponent;
struct CurveEdEntry;
struct ParticleEmitterInstance;

/** ModuleType
 *	Indicates the kind of emitter the module can be applied to.
 *	ie, EPMT_Beam - only applies to beam emitters.
 *
 *	The TypeData field is present to speed up finding the TypeData module.
 */
enum ModuleType
{
	/** General - all emitter types can use it			*/
	EPMT_General,
	/** TypeData - TypeData modules						*/
	EPMT_TypeData,
	/** Beam - only applied to beam emitters			*/
	EPMT_Beam,
	/** Trail - only applied to trail emitters			*/
	EPMT_Trail,
	/** Spawn - all emitter types REQUIRE it			*/
	EPMT_Spawn,
	/** Required - all emitter types REQUIRE it			*/
	EPMT_Required,
	/** Event - event related modules					*/
	EPMT_Event,
	/** Light related modules							*/
	EPMT_Light,
	/** SubUV related modules							*/
	EPMT_SubUV,
	EPMT_MAX,
};

/**
 *	Particle Selection Method, for any emitters that utilize particles
 *	as the source points.
 */
enum ParticleSourceSelectionMethod
{
	/** Random		- select a particle at random		*/
	EPSSM_Random,
	/** Sequential	- select a particle in order		*/
	EPSSM_Sequential,
	EPSSM_MAX,
};

struct ParticleCurvePair
{
	String curveName_;

	SharedPtr<Object> curveObject_;

	ParticleCurvePair() = default;
};

struct ParticleRandomSeedInfo
{
	ParticleRandomSeedInfo()
		: getSeedFromInstance_(false)
		, instanceSeedIsIndex_(false)
		, resetSeedOnEmitterLooping_(true)
		, randomlySelectSeedArray_(false)
	{
	}

	FORCEINLINE Int32 GetInstancePayloadSize() const
	{
		return ((randomSeeds_.Size() > 0) ? sizeof(ParticleRandomSeedInstancePayload) : 0);
	}

	/** The name to expose to the placed instances for setting this seed */
	String parameterName_;

	/**
	 *	If true, the module will attempt to get the seed from the owner
	 *	instance. If that fails, it will fall back to getting it from
	 *	the RandomSeeds array.
	 */
	UInt8 getSeedFromInstance_ : 1;

	/**
	 *	If true, the seed value retrieved from the instance will be an
	 *	index into the array of seeds.
	 */
	UInt8 instanceSeedIsIndex_ : 1;

	/**
	 *	If true, then reset the seed upon the emitter looping.
	 *	For looping environmental effects this should likely be set to false to avoid
	 *	a repeating pattern.
	 */
	UInt8 resetSeedOnEmitterLooping_ : 1;

	/**
	*	If true, then randomly select a seed entry from the RandomSeeds array
	*/
	UInt8 randomlySelectSeedArray_ : 1;

	/**
	 *	The random seed values to utilize for the module.
	 *	More than 1 means the instance will randomly select one.
	 */
	Vector<Int32> randomSeeds_;
};

class ParticleModule : public Object
{
	OBJECT_OVERRIDE(ParticleModule, Object);
public:
	ParticleModule();

	/**
	 * Called once to compile the effects of this module on runtime simulation.
	 * @param EmitterInfo - Information needed for runtime simulation.
	 */
	virtual void CompileModule(struct ParticleEmitterBuildInfo& emitterInfo);

	/**
	 *	Called on a particle that is freshly spawned by the emitter.
	 *
	 *	@param	Owner		The ParticleEmitterInstance that spawned the particle.
	 *	@param	Offset		The modules offset into the data payload of the particle.
	 *	@param	SpawnTime	The time of the spawn.
	 */
	virtual void	Spawn(ParticleEmitterInstance* owner, Int32 offset, float spawnTime, BaseParticle* particleBase);
	/**
	 *	Called on a particle that is being updated by its emitter.
	 *
	 *	@param	Owner		The ParticleEmitterInstance that 'owns' the particle.
	 *	@param	Offset		The modules offset into the data payload of the particle.
	 *	@param	DeltaTime	The time since the last update.
	 */
	virtual void	Update(ParticleEmitterInstance* owner, Int32 offset, float deltaTime);
	/**
	 *	Called on an emitter when all other update operations have taken place
	 *	INCLUDING bounding box cacluations!
	 *
	 *	@param	Owner		The ParticleEmitterInstance that 'owns' the particle.
	 *	@param	Offset		The modules offset into the data payload of the particle.
	 *	@param	DeltaTime	The time since the last update.
	 */
	virtual void	FinalUpdate(ParticleEmitterInstance* owner, Int32 offset, float deltaTime);

	/**
	 *	Returns the number of bytes that the module requires in the particle payload block.
	 *
	 *	@param	TypeData	The ParticleModuleTypeDataBase for the emitter that contains this module
	 *
	 *	@return	UInt32		The number of bytes the module needs per particle.
	 */
	virtual UInt32	RequiredBytes(ParticleModuleTypeDataBase* typeData);
	/**
	 *	Returns the number of bytes the module requires in the emitters 'per-instance' data block.
	 *
	 *	@return UInt32		The number of bytes the module needs per emitter instance.
	 */
	virtual UInt32	RequiredBytesPerInstance();

	/**
	 *	Allows the module to prep its 'per-instance' data block.
	 *
	 *	@param	Owner		The ParticleEmitterInstance that 'owns' the particle.
	 *	@param	InstData	Pointer to the data block for this module.
	 */
	virtual UInt32	PrepPerInstanceBlock(ParticleEmitterInstance* owner, void* instData);

	// For Cascade
	/**
	 *	Called when the module is created, this function allows for setting values that make
	 *	sense for the type of emitter they are being used in.
	 *
	 *	@param	Owner			The ParticleEmitter that the module is being added to.
	 */
	virtual void SetToSensibleDefaults(ParticleEmitter* owner);

	/**
	 *	Fill an array with each Object property that fulfills the FCurveEdInterface interface.
	 *
	 *	@param	OutCurve	The array that should be filled in.
	 */
	virtual void	GetCurveObjects(Vector<ParticleCurvePair>& outCurves);
	/**
	 * Add all curve-editable Objects within this module to the curve editor.
	 *
	 * @param	EdSetup		The CurveEd setup to use for adding curved.
	 * @param	OutCurveEntries	The curves which are for this graph node
	 *
	 * @return	true, if new curves were added to the graph, otherwise they were already present
	 */
	virtual	bool	AddModuleCurvesToEditor(InterpCurveEdSetup* edSetup, PODVector<const CurveEdEntry*>& outCurveEntries);
	/**
	 *	Remove all curve-editable Objects within this module from the curve editor.
	 *
	 *	@param	EdSetup		The CurveEd setup to remove the curve from.
	 */
	void	RemoveModuleCurvesFromEditor(InterpCurveEdSetup* edSetup);
	/**
	 *	Does the module contain curves?
	 *
	 *	@return	bool		true if it does, false if not.
	 */
	bool	ModuleHasCurves();
	/**
	 *	Are the modules curves displayed in the curve editor?
	 *
	 *	@param	EdSetup		The CurveEd setup to check.
	 *
	 *	@return	bool		true if they are, false if not.
	 */
	bool	IsDisplayedInCurveEd(InterpCurveEdSetup* edSetup);
	/**
	 *	Helper function for updating the curve editor when the module editor color changes.
	 *
	 *	@param	Color		The new color the module is using.
	 *	@param	EdSetup		The CurveEd setup for the module.
	 */
	void		ChangeEditorColor(Color& color, InterpCurveEdSetup* edSetup);

	/**
	 *	Render the modules 3D visualization helper primitive.
	 *
	 *	@param	Owner		The ParticleEmitterInstance that 'owns' the module.
	 *	@param	View		The scene view that is being rendered.
	 *	@param	PDI			The FPrimitiveDrawInterface to use for rendering.
	 */
	// virtual void Render3DPreview(ParticleEmitterInstance* Owner, const FSceneView* View, FPrimitiveDrawInterface* PDI) {};

	/**
	 *	Retrieve the ModuleType of this module.
	 *
	 *	@return	ModuleType		The type of module this is.
	 */
	virtual ModuleType	GetModuleType() const { return EPMT_General; }

	/**
	 *	Helper function used by the editor to auto-populate a placed AEmitter with any
	 *	instance parameters that are utilized.
	 *
	 *	@param	PSysComp		The particle system component to be populated.
	 */
	virtual void	AutoPopulateInstanceProperties(ParticleSystemComponent* pSysComp);

	/**
	 *	Helper function used by the editor to auto-generate LOD values from a source module
	 *	and a percentage value used to scale its values.
	 *
	 *	@param	SourceModule		The module to use as the source for values
	 *	@param	Percentage			The percentage of the source values to set
	 *	@param	LODLevel			The LOD level being generated
	 *
	 *	@return	true	if successful
	 *			false	if failed
	 */
	virtual bool	GenerateLODModuleValues(ParticleModule* sourceModule, float percentage, ParticleLODLevel* LODLevel);

	/**
	 *	Store the given percentage of the SourceFloat distribution in the FloatDist
	 *
	 *	@param	FloatDist			The distribution to put the result into.
	 *	@param	SourceFloatDist		The distribution of use as the source.
	 *	@param	Percentage			The percentage of the source value to use [0..100]
	 *
	 *	@return	bool				true if successful, false if not.
	 */
	virtual bool	ConvertFloatDistribution(DistributionFloat* floatDist, DistributionFloat* sourceFloatDist, float percentage);
	/**
	 *	Store the given percentage of the SourceVector distribution in the VectorDist
	 *
	 *	@param	VectorDist			The distribution to put the result into.
	 *	@param	SourceVectorDist	The distribution of use as the source.
	 *	@param	Percentage			The percentage of the source value to use [0..100]
	 *
	 *	@return	bool				true if successful, false if not.
	 */
	virtual bool	ConvertVectorDistribution(DistributionVector* vectorDist, DistributionVector* sourceVectorDist, float percentage);
	/**
	 *	Returns whether the module is SizeMultipleLife or not.
	 *
	 *	@return	bool	true if the module is a UParticleModuleSizeMultipleLife
	 *					false if not
	 */
	virtual bool   IsSizeMultiplyLife() { return false; };

	/**
	 *	Returns whether the module supports the RandomSeed functionality
	 *
	 *	@return	bool	true if it supports it; false if not
	 */
	bool SupportsRandomSeed() const
	{
		return supportsRandomSeed_;
	}

	/**
	 *	Returns whether the module requires notification when an emitter loops.
	 *
	 *	@return	bool	true if the module required looping notification
	 */
	bool RequiresLoopingNotification() const
	{
		return requiresLoopingNotification_;
	}

	/**
	 *	Called when an emitter instance is looping...
	 *
	 *	@param	Owner					The emitter instance that owns this module
	 */
	virtual void EmitterLoopingNotify(ParticleEmitterInstance* owner)
	{
	}

	/**
	 *	Generates a new module for LOD levels, setting the values appropriately.
	 *	Note that the module returned could simply be the module it was called on.
	 *
	 *	@param	SourceLODLevel		The source LODLevel
	 *	@param	DestLODLevel		The destination LODLevel
	 *	@param	Percentage			The percentage value that should be used when setting values
	 *
	 *	@return	ParticleModule*	The generated module, or this if percentage == 100.
	 */
	virtual ParticleModule* GenerateLODModule(ParticleLODLevel* sourceLODLevel, ParticleLODLevel* destLODLevel, float percentage,
		bool generateModuleData, bool forceModuleConstruction = false);

	/**
	 *	Returns true if the results of LOD generation for the given percentage will result in a
	 *	duplicate of the module.
	 *
	 *	@param	SourceLODLevel		The source LODLevel
	 *	@param	DestLODLevel		The destination LODLevel
	 *	@param	Percentage			The percentage value that should be used when setting values
	 *
	 *	@return	bool				true if the generated module will be a duplicate.
	 *								false if not.
	 */
	virtual bool WillGeneratedModuleBeIdentical(ParticleLODLevel* sourceLODLevel, ParticleLODLevel* destLODLevel, float percentage)
	{
		// The assumption is that at 100%, ANY module will be identical...
		// (Although this is virtual to allow over-riding that assumption on a case-by-case basis!)

		if (percentage != 100.0f)
		{
			return LODDuplicate_;
		}

		return true;
	}

	/**
	 *	Returns true if the module validiy flags indicate this module is used in the given LOD level.
	 *
	 *	@param	SourceLODIndex		The index of the source LODLevel
	 *
	 *	@return	bool				true if the generated module is used, false if not.
	 */
	virtual bool IsUsedInLODLevel(Int32 sourceLODIndex) const;

	/**
	 *	Retrieve the ParticleSysParams associated with this module.
	 *
	 *	@param	ParticleSysParamList	The list of FParticleSysParams to add to
	 */
	virtual void GetParticleSysParamsUtilized(Vector<String>& particleSysParamList);

	/**
	 *	Retrieve the distributions that use ParticleParameters in this module.
	 *
	 *	@param	ParticleParameterList	The list of ParticleParameter distributions to add to
	 */
	virtual void GetParticleParametersUtilized(Vector<String>& particleParameterList);

	/**
	 *	Refresh the module...
	 */
	virtual void RefreshModule(InterpCurveEdSetup* edSetup, ParticleEmitter* inEmitter, Int32 inLODLevel) {}

	/**
	 *	Return true if this module impacts rotation of Mesh emitters
	 *	@return	bool		true if the module impacts mesh emitter rotation
	 */
	virtual bool	TouchesMeshRotation() const { return false; }

	/**
	 *	Prepare a random seed instance payload...
	 *
	 *	@param	Owner					The emitter instance that owns this module
	 *	@param	InRandSeedPayload		The random seed instance payload to initialize
	 *	@param	InRandSeedInfo			The random seed info of the module
	 *
	 *	@return	UInt32					0xffffffff is failed
	 */
	virtual UInt32	PrepRandomSeedInstancePayload(ParticleEmitterInstance* owner, ParticleRandomSeedInstancePayload* randSeedPayload, const ParticleRandomSeedInfo& randSeedInfo);

	/**
	 *	Retrieve the random seed info for this module.
	 *
	 *	@return	ParticleRandomSeedInfo*	The random seed info; NULL if not supported
	 */
	virtual ParticleRandomSeedInfo* GetRandomSeedInfo()
	{
		return NULL;
	}

	/**
	 *	Set the random seed info entry at the given index to the given seed
	 *
	 *	@param	InIndex			The index of the entry to set
	 *	@param	InRandomSeed	The seed to set the entry to
	 *
	 *	@return	bool			true if successful; false if not (not found, etc.)
	 */
	virtual bool SetRandomSeedEntry(Int32 inIndex, Int32 inRandomSeed);

	/** Return false if this emitter requires a game thread tick **/
	virtual bool CanTickInAnyThread()
	{
		return true;
	}

	/** Returns whether this module is used in any GPU emitters. */
	bool IsUsedInGPUEmitter()const;

	/**
	 * Retreive the random stream that should be used for the provided instance.
	 *
	 * @param	Owner			The emitter instance that owns this module
	 *
	 * @return FRandomStream&	The random stream to use for the provided instance.
	 */
	RandomStream& GetRandomStream(ParticleEmitterInstance* owner);

#if WITH_EDITOR
	virtual void PostLoadSubobjects(FObjectInstancingGraph* outerInstanceGraph) override;

	/**
	 *	Custom Cascade module menu entries support
	 */
	 /**
	  *	Get the number of custom entries this module has. Maximum of 3.
	  *
	  *	@return	Int32		The number of custom menu entries
	  */
	virtual Int32 GetNumberOfCustomMenuOptions() const { return 0; }

	/**
	 *	Get the display name of the custom menu entry.
	 *
	 *	@param	InEntryIndex		The custom entry index (0-2)
	 *	@param	OutDisplayString	The string to display for the menu
	 *
	 *	@return	bool				true if successful, false if not
	 */
	virtual bool GetCustomMenuEntryDisplayString(Int32 inEntryIndex, String& outDisplayString) const { return false; }

	/**
	 *	Perform the custom menu entry option.
	 *
	 *	@param	InEntryIndex		The custom entry index (0-2) to perform
	 *
	 *	@return	bool				true if successful, false if not
	 */
	virtual bool PerformCustomMenuEntry(Int32 inEntryIndex) { return false; }

	/** Returns true if the module is valid for the provided LOD level. */
	virtual bool IsValidForLODLevel(ParticleLODLevel* LODLevel, String& outErrorString)
	{
		return true;
	}

	/**
	 *	Gets a list of the names of distributions not allowed on GPU emitters.
	 */
	static  void GetDistributionsRestrictedOnGPU(Vector<String>& outRestrictedDistributions);

	/**
	 *	Checks if a distribution is allowed on the GPU.
	 *
	 *	@param	Distribution		The Distribution to test.
	 *	@return	bool				true if allowed on the GPU, false if not.
	 */
	static bool IsDistributionAllowedOnGPU(const Distribution* distribution);

	/**
	 *	Generates the FText to display to the user informing them that a module is using a distribution that is not allowed on GPU emitters.
	 *
	 *	@param	ModuleName		The name of the module the distribution is in.
	 *	@param	PropertyName	The name of the distribution's property.
	 *	@return	FText			The generated FText.
	 */
	static FText GetDistributionNotAllowedOnGPUText(const String& moduleName, const String& propertyName);

	/**
	 *	Set the transaction flag on the module and any members which require it
	 */
	void SetTransactionFlag();
#endif

	/** If true, the module performs operations on particles during Spawning		*/
	UInt8 spawnModule_ : 1;

	/** If true, the module performs operations on particles during Updating		*/
	UInt8 updateModule_ : 1;

	/** If true, the module performs operations on particles during final update	*/
	UInt8 finalUpdateModule_ : 1;

	/** If true, the module performs operations on particles during update and/or final update for GPU emitters*/
	UInt8 updateForGPUEmitter_ : 1;

	/** If true, the module displays FVector curves as colors						*/
	UInt8 curvesAsColor_ : 1;

	/** If true, the module should render its 3D visualization helper				*/
	UInt8 _3DDrawMode_ : 1;

	/** If true, the module supports rendering a 3D visualization helper			*/
	UInt8 supported3DDrawMode_ : 1;

	/** If true, the module is enabled												*/
	UInt8 enabled_ : 1;

	/** If true, the module has had editing enabled on it							*/
	UInt8 editable_ : 1;

	/**
	*	If true, this flag indicates that auto-generation for LOD will result in
	*	an exact duplicate of the module, regardless of the percentage.
	*	If false, it will result in a module with different settings.
	*/
	UInt8 LODDuplicate_ : 1;

	/** If true, the module supports RandomSeed setting */
	UInt8 supportsRandomSeed_ : 1;

	/** If true, the module should be told when looping */
	UInt8 requiresLoopingNotification_ : 1;

	/**
	 *	The LOD levels this module is present in.
	 *	Bit-flags are used to indicate validity for a given LOD level.
	 *	For example, if
	 *		((1 << Level) & LODValidity) != 0
	 *	then the module is used in that LOD.
	 */
	UInt8 LODValidity_;

#if WITH_EDITOR
	/** The color to draw the modules curves in the curve editor.
	 *	If bCurvesAsColor is true, it overrides this value.
	 */
	Color moduleEditorColor_;

#endif // WITH_EDITOR
};

}
