#pragma once

#include "Container/Str.h"
#include "Container/Ptr.h"
#include "Container/EnumAsByte.h"
#include "Graphics/Material.h"
#include "Resource/Resource.h"
#include "Math/BoundingBox.h"

namespace FlagGG
{

enum class ParticleSignificanceLevel : UInt8;
enum class ParticleSystemInsignificanceReaction : UInt8;
class InterpCurveEdSetup;
class MaterialInterface;
class ParticleSystemComponent;
class ParticleEmitter;

enum ParticleSystemUpdateMode
{
	/** RealTime	- update via the delta time passed in				*/
	EPSUM_RealTime,
	/** FixedTime	- update via a fixed time step						*/
	EPSUM_FixedTime
};

enum ParticleSystemLODMethod
{
	// Automatically set the LOD level, checking every LODDistanceCheckTime seconds.
	PARTICLESYSTEMLODMETHOD_Automatic,

	// LOD level is directly set by the game code.
	PARTICLESYSTEMLODMETHOD_DirectSet,

	// LOD level is determined at Activation time, then left alone unless directly set by game code.
	PARTICLESYSTEMLODMETHOD_ActivateAutomatic
};

enum ParticleSystemOcclusionBoundsMethod
{
	/** Don't determine occlusion on this particle system */
	EPSOBM_None,
	/** Use the bounds of the particle system component when determining occlusion */
	EPSOBM_ParticleBounds,
	/** Use the custom occlusion bounds when determining occlusion */
	EPSOBM_CustomBounds
};

struct ParticleSystemLOD
{
	ParticleSystemLOD()
	{
	}

	static ParticleSystemLOD CreateParticleSystemLOD()
	{
		ParticleSystemLOD newLOD;
		return newLOD;
	}
};

struct LODSoloTrack
{
	PODVector<UInt8> SoloEnableSetting;
};

struct NamedEmitterMaterial
{
	String name_;

	SharedPtr<Material> material_;
};

/**
 * A ParticleSystem is a complete particle effect that contains any number of ParticleEmitters. By allowing multiple emitters
 * in a system, the designer can create elaborate particle effects that are held in a single system. Once created using
 * Cascade, a ParticleSystem can then be inserted into a level or created in script.
 */
class ParticleSystem : public Resource
{
	OBJECT_OVERRIDE(ParticleSystem, Resource);
public:

	ParticleSystem()
		: occlusionBoundsMethod_(EPSOBM_ParticleBounds)
	{

	}

	/** UpdateTime_FPS	- the frame per second to update at in FixedTime mode		*/
	float updateTime_FPS_;

	/** UpdateTime_Delta	- internal												*/
	float updateTime_Delta_;

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

	/** Emitters	- internal - the array of emitters in the system				*/
	Vector<SharedPtr<ParticleEmitter>> emitters_;

	/** The component used to preview the particle system in Cascade				*/
	SharedPtr<ParticleSystemComponent> previewComponent_;

#if WITH_EDITORONLY_DATA
	/** The angle to use when rendering the thumbnail image							*/
	FRotator ThumbnailAngle;

	/** The distance to place the system when rendering the thumbnail image			*/
	float ThumbnailDistance;

	/** The time to warm-up the system for the thumbnail image						*/
	float ThumbnailWarmup;

#endif // WITH_EDITORONLY_DATA
	/** Used for curve editor to remember curve-editing setup.						*/
	SharedPtr<InterpCurveEdSetup> curveEdSetup_;

	//
	//	LOD
	//
	/**
	 *	How often (in seconds) the system should perform the LOD distance check.
	 */
	float LODDistanceCheckTime_;

	/** World space radius that UVs generated with the ParticleMacroUV material node will tile based on. */
	float macroUVRadius_;

	/**
	 *	The array of distances for each LOD level in the system.
	 *	Used when LODMethod is set to PARTICLESYSTEMLODMETHOD_Automatic.
	 *
	 *	Example: System with 3 LOD levels
	 *		LODDistances(0) = 0.0
	 *		LODDistances(1) = 2500.0
	 *		LODDistances(2) = 5000.0
	 *
	 *		In this case, when the system is [   0.0 ..   2499.9] from the camera, LOD level 0 will be used.
	 *										 [2500.0 ..   4999.9] from the camera, LOD level 1 will be used.
	 *										 [5000.0 .. INFINITY] from the camera, LOD level 2 will be used.
	 *
	 */
	PODVector<float> LODDistances_;

#if WITH_EDITORONLY_DATA
	/** LOD setting for intepolation (set by Cascade) Range [0..100]				*/
	Int32 EditorLODSetting;

#endif // WITH_EDITORONLY_DATA

	Vector<ParticleSystemLOD> LODSettings_;

	/**	Fixed relative bounding box for particle system.							*/
	BoundingBox fixedRelativeBoundingBox_;

	/**
	 * Number of seconds of emitter not being rendered that need to pass before it
	 * no longer gets ticked/ becomes inactive.
	 */
	float secondsBeforeInactive_;

#if WITH_EDITORONLY_DATA
	//
	//	Cascade 'floor' mesh information.
	//
	String floorMesh_;

	Vector3 floorPosition_;

	Rotator floorRotation_;

	float floorScale_;

	Vector3 floorScale3D_;

	/** The background color to display in Cascade */
	Color backgroundColor_;

#endif // WITH_EDITORONLY_DATA

	/** How long this Particle system should delay when ActivateSystem is called on it. */
	float delay_;

	/** The low end of the emitter delay if using a range. */
	float delayLow_;

	/** If true, the system's Z axis will be oriented toward the camera				*/
	UInt8 orientZAxisTowardCamera_ : 1;

	/** Whether to use the fixed relative bounding box or calculate it every frame. */
	UInt8 useFixedRelativeBoundingBox_ : 1;

	/** EDITOR ONLY: Indicates that Cascade would like to have the PeakActiveParticles count reset */
	UInt8 shouldResetPeakCounts_ : 1;

	/** Set during load time to indicate that physics is used... */
	UInt8 hasPhysics_ : 1;

	/** Inidicates the old 'real-time' thumbnail rendering should be used	*/
	UInt8 useRealtimeThumbnail_ : 1;

	/** Internal: Indicates the PSys thumbnail image is out of date			*/
	UInt8 thumbnailImageOutOfDate_ : 1;

private:
	/** if true, this psys can tick in any thread **/
	UInt8 isElligibleForAsyncTick_ : 1;
	/** if true, bIsElligibleForAsyncTick is set up **/
	UInt8 isElligibleForAsyncTickComputed_ : 1;
public:

#if WITH_EDITORONLY_DATA
	/** Internal: The PSys thumbnail image									*/
	SharedPtr<Texture2D> thumbnailImage_;

#endif // WITH_EDITORONLY_DATA

	/**
	 *	If true, select the emitter delay from the range
	 *		[DelayLow..Delay]
	 */
	UInt8 useDelayRange_ : 1;

	UInt8 allowManagedTicking_ : 1;

	UInt8 autoDeactivate_ : 1;

	/**
	 *	Internal value that tracks the regenerate LOD levels preference.
	 *	If true, when autoregenerating LOD levels in code, the low level will
	 *	be a duplicate of the high.
	 */
	UInt8 regenerateLODDuplicate_ : 1;

	EnumAsByte<ParticleSystemUpdateMode> systemUpdateMode_;

	/**
	 *	The method of LOD level determination to utilize for this particle system
	 *	  PARTICLESYSTEMLODMETHOD_Automatic - Automatically set the LOD level, checking every LODDistanceCheckTime seconds.
	 *    PARTICLESYSTEMLODMETHOD_DirectSet - LOD level is directly set by the game code.
	 *    PARTICLESYSTEMLODMETHOD_ActivateAutomatic - LOD level is determined at Activation time, then left alone unless directly set by game code.
	 */
	EnumAsByte<ParticleSystemLODMethod> LODMethod_;

	/** The reaction this system takes when all emitters are insignificant. */
	ParticleSystemInsignificanceReaction insignificantReaction_;

	/**
	 *	Which occlusion bounds method to use for this particle system.
	 *	EPSOBM_None - Don't determine occlusion for this system.
	 *	EPSOBM_ParticleBounds - Use the bounds of the component when determining occlusion.
	 */
	EnumAsByte<ParticleSystemOcclusionBoundsMethod> occlusionBoundsMethod_;

private:
	/** Does any emitter loop forever? */
	UInt8 anyEmitterLoopsForever_ : 1;

public:

	/** The maximum level of significance for emitters in this system. Any emitters with a higher significance will be capped at this significance level. */
	ParticleSignificanceLevel maxSignificanceLevel_;

	UInt32 minTimeBetweenTicks_;

	/** Time delay between all emitters becoming insignificant and the systems insignificant reaction. */
	float insignificanceDelay_;

	/** Local space position that UVs generated with the ParticleMacroUV material node will be centered on. */
	Vector3 macroUVPosition_;

	/** The occlusion bounds to use if OcclusionBoundsMethod is set to EPSOBM_CustomBounds */
	BoundingBox customOcclusionBounds_;

	Vector<LODSoloTrack> soloTracking_;

	/**
	*	Array of named material slots for use by emitters of this system.
	*	Emitters can use these instead of their own materials by providing the name to the NamedMaterialOverrides property of their required module.
	*	These materials can be overridden using CreateNamedDynamicMaterialInstance() on a ParticleSystemComponent.
	*/
	Vector<NamedEmitterMaterial> namedMaterialSlots_;

	void PrecachePSOs();

	bool CanBePooled()const;

	// @todo document
	void UpdateColorModuleClampAlpha(class ParticleModuleColorBase* colorModule);

	/**
	 *	CalculateMaxActiveParticleCounts
	 *	Determine the maximum active particles that could occur with each emitter.
	 *	This is to avoid reallocation during the life of the emitter.
	 *
	 *	@return	true	if the numbers were determined for each emitter
	 *			false	if not be determined
	 */
	virtual bool		CalculateMaxActiveParticleCounts();

	/**
	 *	Retrieve the parameters associated with this particle system.
	 *
	 *	@param	ParticleSysParamList	The list of FParticleSysParams used in the system
	 *	@param	ParticleParameterList	The list of ParticleParameter distributions used in the system
	 */
	FlagGG_API void GetParametersUtilized(Vector<Vector<String>>& particleSysParamList,
		Vector<Vector<String>>& particleParameterList);

	/**
	 *	Setup the soloing information... Obliterates all current soloing.
	 */
	FlagGG_API void SetupSoloing();

	/**
	 *	Toggle the bIsSoloing flag on the given emitter.
	 *
	 *	@param	InEmitter		The emitter to toggle.
	 *
	 *	@return	bool			true if ANY emitters are set to soloing, false if none are.
	 */
	FlagGG_API bool ToggleSoloing(class ParticleEmitter* inEmitter);

	/**
	 *	Turn soloing off completely - on every emitter
	 *
	 *	@return	bool			true if successful, false if not.
	 */
	FlagGG_API bool TurnOffSoloing();

	/**
	 *	Editor helper function for setting the LOD validity flags used in Cascade.
	 */
	FlagGG_API void SetupLODValidity();

	/**
	 * Set the time to delay spawning the particle system
	 */
	FlagGG_API void SetDelay(float InDelay);

#if WITH_EDITOR
	/**
	 *	Remove all duplicate modules.
	 *
	 *	@param	bInMarkForCooker	If true, mark removed objects to not cook out.
	 *	@param	OutRemovedModules	Optional map to fill in w/ removed modules...
	 *
	 *	@return	bool				true if successful, false if not
	 */
	FlagGG_API bool RemoveAllDuplicateModules(bool inMarkForCooker, HashMap<Object*, bool>* outRemovedModules);

	/**
	 *	Update all emitter module lists
	 */
	FlagGG_API void UpdateAllModuleLists();
#endif
	/** Return the currently set LOD method											*/
	virtual enum ParticleSystemLODMethod GetCurrentLODMethod();

	/**
	 *	Return the number of LOD levels for this particle system
	 *
	 *	@return	The number of LOD levels in the particle system
	 */
	virtual Int32 GetLODLevelCount();

	/**
	 *	Return the distance for the given LOD level
	 *
	 *	@param	LODLevelIndex	The LOD level that the distance is being retrieved for
	 *
	 *	@return	-1.0f			If the index is invalid
	 *			Distance		The distance set for the LOD level
	 */
	virtual float GetLODDistance(Int32 LODLevelIndex);

	/**
	 *	Set the LOD method
	 *
	 *	@param	InMethod		The desired method
	 */
	virtual void SetCurrentLODMethod(ParticleSystemLODMethod inMethod);

	/**
	 *	Set the distance for the given LOD index
	 *
	 *	@param	LODLevelIndex	The LOD level to set the distance of
	 *	@param	InDistance		The distance to set
	 *
	 *	@return	true			If successful
	 *			false			Invalid LODLevelIndex
	 */
	virtual bool SetLODDistance(Int32 LODLevelIndex, float inDistance);

	/**
	*	Checks if any of the emitters have motion blur at a specific lod level.
	*
	*	@param	LODLevelIndex	The LOD level to check motion blur availability
	*
	*	@return	true			If any emitter has motion blur
	*			false			None of the emitters have motion blur
	*/
	bool DoesAnyEmitterHaveMotionBlur(Int32 LODLevelIndex) const;

	/**
	 * Builds all emitters in the particle system.
	 */
	FlagGG_API void BuildEmitters();

	/** return true if this psys can tick in any thread */
	FORCEINLINE bool CanTickInAnyThread()
	{
		if (!isElligibleForAsyncTickComputed_)
		{
			ComputeCanTickInAnyThread();
		}
		return isElligibleForAsyncTick_;
	}
	/** Decide if this psys can tick in any thread, and set bIsElligibleForAsyncTick */
	FlagGG_API void ComputeCanTickInAnyThread();

	/** Returns true if this system contains any GPU emitters. */
	bool HasGPUEmitter() const;

	/**
	Returns true if this system contains an emitter of the pasesd type.
	@ param TypeData - The emitter type to check for. Must be a child class of UParticleModuleTypeDataBase
	*/
	// bool ContainsEmitterType(UClass* TypeData);

	/** Returns true if the particle system is looping (contains one or more looping emitters) */
	bool IsLooping() const { return anyEmitterLoopsForever_; }
	bool IsImmortal() const { return isImmortal_; }
	bool WillBecomeZombie() const { return willBecomeZombie_; }

	ParticleSignificanceLevel GetHighestSignificance()const { return highestSignificance_; }
	ParticleSignificanceLevel GetLowestSignificance()const { return lowestSignificance_; }
	bool ShouldManageSignificance()const { return shouldManageSignificance_; }

	FORCEINLINE bool AllowManagedTicking()const { return allowManagedTicking_; }
private:

	/** The highest significance of any emitter. Clamped by MaxSignificanceLevel.*/
	ParticleSignificanceLevel highestSignificance_;
	/** The lowest significance of any emitter. Clamped by MaxSignificanceLevel.*/
	ParticleSignificanceLevel lowestSignificance_;

	UInt8 shouldManageSignificance_ : 1;

	/** Does any emitter never die due to infinite looping AND indefinite duration? */
	UInt8 isImmortal_ : 1;
	/** Does any emitter ever become a zombie (is immortal AND stops spawning at some point, i.e. is burst only)? */
	UInt8 willBecomeZombie_ : 1;
};

}
