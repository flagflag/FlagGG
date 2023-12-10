#pragma once

#include "Core/BaseTypes.h"
#include "Core/Object.h"
#include "Container/EnumAsByte.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"
#include "ParticleSystem/ParticleModule.h"
#include "ParticleSystem/SubUVAnimation.h"
#include "ParticleSystem/Module/ParticleModuleOrientation.h"

namespace FlagGG
{

class InterpCurveEdSetup;
class ParticleLODLevel;
class ParticleSystemComponent;
class Material;

//~=============================================================================
//	Burst emissions
//~=============================================================================
enum ParticleBurstMethod
{
	EPBM_Instant,
	EPBM_Interpolated,
	EPBM_MAX,
};

//~=============================================================================
//	SubUV-related
//~=============================================================================
enum ParticleSubUVInterpMethod
{
	PSUVIM_None,
	PSUVIM_Linear,
	PSUVIM_Linear_Blend,
	PSUVIM_Random,
	PSUVIM_Random_Blend,
	PSUVIM_MAX,
};

//~=============================================================================
//	Cascade-related
//~=============================================================================
enum EmitterRenderMode
{
	ERM_Normal,
	ERM_Point,
	ERM_Cross,
	ERM_LightsOnly,
	ERM_None,
	ERM_MAX,
};

struct ParticleBurst
{
	ParticleBurst()
		: count_(0)
		, countLow_(-1)		// Disabled by default...
		, time_(0.0f)
	{
	}

	/** The number of particles to burst */
	Int32 count_;

	/** If >= 0, use as a range [CountLow..Count] */
	Int32 countLow_;

	/** The time at which to burst them (0..1: emitter lifetime) */
	float time_;	
};

//DECLARE_STATS_GROUP(TEXT("Emitters"), STATGROUP_Emitters, STATCAT_Advanced);
//DECLARE_STATS_GROUP(TEXT("Emitters"), STATGROUP_EmittersRT, STATCAT_Advanced);
//DECLARE_DWORD_COUNTER_STAT_EXTERN(TEXT("STAT_EmittersStatGroupTester"), STAT_EmittersStatGroupTester, STATGROUP_Emitters, FlagGG_API);
//DECLARE_DWORD_COUNTER_STAT_EXTERN(TEXT("STAT_EmittersRTStatGroupTester"), STAT_EmittersRTStatGroupTester, STATGROUP_EmittersRT, FlagGG_API);

/* detail modes for emitters are now flags instead of a single enum
	an emitter is shown if it is flagged for the current system scalability level
 */
#define NUM_DETAILMODE_FLAGS 3

class ParticleEmitter : public Object
{
	OBJECT_OVERRIDE(ParticleEmitter, Object);
public:
	ParticleEmitter();

	// @todo document
	virtual ParticleEmitterInstance* CreateInstance(ParticleSystemComponent* inComponent);

	// Sets up this emitter with sensible defaults so we can see some particles as soon as its created.
	virtual void SetToSensibleDefaults() {}

	// @todo document
	virtual void UpdateModuleLists();

	// @todo document
	FlagGG_API void SetEmitterName(const String& name);

	// @todo document
	FlagGG_API const String& GetEmitterName();

	// @todo document
	virtual	void						SetLODCount(Int32 LODCount);

	// For Cascade
	// @todo document
	void	AddEmitterCurvesToEditor(InterpCurveEdSetup* edSetup);

	// @todo document
	FlagGG_API void	RemoveEmitterCurvesFromEditor(InterpCurveEdSetup* edSetup);

	// @todo document
	FlagGG_API void	ChangeEditorColor(Color& color, InterpCurveEdSetup* edSetup);

	// @todo document
	FlagGG_API void	AutoPopulateInstanceProperties(ParticleSystemComponent* pSysComp);

	/** CreateLODLevel
	 *	Creates the given LOD level.
	 *	Intended for editor-time usage.
	 *	Assumes that the given LODLevel will be in the [0..100] range.
	 *	
	 *	@return The index of the created LOD level
	 */
	FlagGG_API Int32		CreateLODLevel(Int32 LODLevel, bool generateModuleData = true);

	/** IsLODLevelValid
	 *	Returns true if the given LODLevel is one of the array entries.
	 *	Intended for editor-time usage.
	 *	Assumes that the given LODLevel will be in the [0..(NumLODLevels - 1)] range.
	 *	
	 *	@return false if the requested LODLevel is not valid.
	 *			true if the requested LODLevel is valid.
	 */
	FlagGG_API bool	IsLODLevelValid(Int32 LODLevel);

	/** GetCurrentLODLevel
	*	Returns the currently set LODLevel. Intended for game-time usage.
	*	Assumes that the given LODLevel will be in the [0..# LOD levels] range.
	*	
	*	@return NULL if the requested LODLevel is not valid.
	*			The pointer to the requested UParticleLODLevel if valid.
	*/
	ParticleLODLevel* GetCurrentLODLevel(ParticleEmitterInstance* instance);


	/**
	 * This will update the LOD of the particle in the editor.
	 *
	 * @see GetCurrentLODLevel(FParticleEmitterInstance* instance)
	 */
	FlagGG_API void EditorUpdateCurrentLOD(ParticleEmitterInstance* instance);

	/** GetLODLevel
	 *	Returns the given LODLevel. Intended for game-time usage.
	 *	Assumes that the given LODLevel will be in the [0..# LOD levels] range.
	 *	
	 *	@param	LODLevel - the requested LOD level in the range [0..# LOD levels].
	 *
	 *	@return NULL if the requested LODLevel is not valid.
	 *			The pointer to the requested UParticleLODLevel if valid.
	 */
	FlagGG_API ParticleLODLevel*	GetLODLevel(Int32 LODLevel);
	
	/**
	 *	Autogenerate the lowest LOD level...
	 *
	 *	@param	bDuplicateHighest	If true, make the level an exact copy of the highest
	 *
	 *	@return	bool				true if successful, false if not.
	 */
	virtual bool		AutogenerateLowestLODLevel(bool duplicateHighest = false);
	
	/**
	 *	CalculateMaxActiveParticleCount
	 *	Determine the maximum active particles that could occur with this emitter.
	 *	This is to avoid reallocation during the life of the emitter.
	 *
	 *	@return	true	if the number was determined
	 *			false	if the number could not be determined
	 */
	virtual bool		CalculateMaxActiveParticleCount();

	/**
	 *	Retrieve the parameters associated with this particle system.
	 *
	 *	@param	ParticleSysParamList	The list of FParticleSysParams used in the system
	 *	@param	ParticleParameterList	The list of ParticleParameter distributions used in the system
	 */
	void GetParametersUtilized(Vector<String>& particleSysParamList, Vector<String>& particleParameterList);

	/**
	 * Builds data needed for simulation by the emitter from all modules.
	 */
	void Build();

	/** Pre-calculate data size/offset and other info from modules in this Emitter */
	void CacheEmitterModuleInfo();

	/**
	 *   Calculate spawn rate multiplier based on global effects quality level and emitter's quality scale
 	 */
	float GetQualityLevelSpawnRateMult();

	/** Returns true if the is emitter has any enabled LODs, false otherwise. */
	bool HasAnyEnabledLODs()const;

	/** Returns if this emitter is considered significant for the passed requirement. */
	FlagGG_API bool IsSignificant(ParticleSignificanceLevel requiredSignificance);


	//~=============================================================================
	//	General variables
	//~=============================================================================
	/** The name of the emitter. */
	String emitterName_;

	Int32 subUVDataOffset_;

	/**
	 *	How to render the emitter particles. Can be one of the following:
	 *		ERM_Normal	- As the intended sprite/mesh
	 *		ERM_Point	- As a 2x2 pixel block with no scaling and the color set in EmitterEditorColor
	 *		ERM_Cross	- As a cross of lines, scaled to the size of the particle in EmitterEditorColor
	 *		ERM_None	- Do not render
	 */
	EnumAsByte<EmitterRenderMode> emitterRenderMode_;

	/** The significance level required of this emitter's owner for this emitter to be active. */
	ParticleSignificanceLevel significanceLevel_;

	EnumAsByte<ParticleAxisLock> lockAxisFlags_;

	/** If true, maintains some legacy spawning behavior. */
	UInt8 useLegacySpawningBehavior_ : 1;

	//////////////////////////////////////////////////////////////////////////
	// Below is information udpated by calling CacheEmitterModuleInfo

	UInt8 requiresLoopNotification_ : 1;
	UInt8 axisLockEnabled_ : 1;
	UInt8 meshRotationActive_ : 1;

	UInt8 convertedModules_ : 1;

	/** If true, then show only this emitter in the editor */
	UInt8 isSoloing_ : 1;

	/**
	 *	If true, then this emitter was 'cooked out' by the cooker.
	 *	This means it was completely disabled, but to preserve any
	 *	indexing schemes, it is left in place.
	 */
	UInt8 cookedOut_ : 1;

	/** When true, if the current LOD is disabled the emitter will be kept alive. Otherwise, the emitter will be considered complete if the current LOD is disabled. */
	UInt8 disabledLODsKeepEmitterAlive_ : 1;

	/** When true, emitters deemed insignificant will have their tick and render disabled Instantly. When false they will simple stop spawning new particles. */
	UInt8 disableWhenInsignficant_ : 1;

	UInt8 removeHMDRollInVR_ : 1;

#if WITH_EDITORONLY_DATA

	/** This value indicates the emitter should be drawn 'collapsed' in Cascade */
	UInt8 collapsed_ : 1;

	/** If detail mode is >= system detail mode, primitive won't be rendered. */
	EnumAsByte<DetailMode> detailMode_DEPRECATED_;

	/**
	 *	The color of the emitter in the curve editor and debug rendering modes.
	 */
	Color emitterEditorColor_;

#endif // WITH_EDITORONLY_DATA
	//~=============================================================================
	//	'Private' data - not required by the editor
	//~=============================================================================
	Vector<SharedPtr<ParticleLODLevel>> LODLevels_;

	Int32 peakActiveParticles_;

	//~=============================================================================
	//	Performance/LOD Data
	//~=============================================================================

	/**
	 *	Initial allocation count - overrides calculated peak count if > 0
	 */
	Int32 initialAllocationCount_;

	float qualityLevelSpawnRateScale_;

	/** Detail mode: Set flags reflecting which system detail mode you want the emitter to be ticked and rendered in */
	UInt32 detailModeBitmask_;

#if WITH_EDITORONLY_DATA
	String detailModeDisplay_;
#endif // WITH_EDITORONLY_DATA

	/** Map module pointers to their offset into the particle data.		*/
	HashMap<ParticleModule*, UInt32> moduleOffsetMap_;

	/** Map module pointers to their offset into the instance data.		*/
	HashMap<ParticleModule*, UInt32> moduleInstanceOffsetMap_;

	/** Map module pointers to their offset into the instance data.		*/
	HashMap<ParticleModule*, UInt32> moduleRandomSeedInstanceOffsetMap_;

	/** Materials collected from any MeshMaterial modules */
	PODVector<Material*> meshMaterials_;

	Int32 dynamicParameterDataOffset_;
	Int32 lightDataOffset_;
	float lightVolumetricScatteringIntensity_;
	Int32 cameraPayloadOffset_;
	Int32 particleSize_;
	Int32 reqInstanceBytes_;
	Vector2 pivotOffset_;
	Int32 typeDataOffset_;
	Int32 typeDataInstanceOffset_;

	float minFacingCameraBlendDistance_;
	float maxFacingCameraBlendDistance_;

	// Array of modules that want emitter instance data
	PODVector<ParticleModule*> modulesNeedingInstanceData_;

	// Array of modules that want emitter random seed instance data
	PODVector<ParticleModule*> modulesNeedingRandomSeedInstanceData_;

	/** SubUV animation asset to use for cutout geometry. */
	SubUVAnimation* RESTRICT subUVAnimation_;
};


}

