#pragma once

#include "Core/Object.h"
#include "Math/Color.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "ParticleSystem/ParticleSystem.h"
#include "ParticleSystem/Module/ParticleModuleRequired.h"
#include "ParticleSystem/Module/ParticleModuleSpawn.h"
#include "ParticleSystem/Module/ParticleModuleOrbit.h"

namespace FlagGG
{

class InterpCurveEdSetup;
class ParticleModule;
class Material;
struct ParticleEmitterBuildInfo;

class ParticleLODLevel : public Object
{
	OBJECT_OVERRIDE(ParticleLODLevel, Object);
public:
	ParticleLODLevel();

	~ParticleLODLevel() override;

	// @todo document
	virtual void	UpdateModuleLists();

	// @todo document
	virtual bool	GenerateFromLODLevel(ParticleLODLevel* sourceLODLevel, float percentage, bool generateModuleData = true);

	/**
	 *	CalculateMaxActiveParticleCount
	 *	Determine the maximum active particles that could occur with this emitter.
	 *	This is to avoid reallocation during the life of the emitter.
	 *
	 *	@return		The maximum active particle count for the LOD level.
	 */
	virtual Int32	CalculateMaxActiveParticleCount();

	/** Update to the new SpawnModule method */
	void	ConvertToSpawnModule();

	/** @return the index of the given module if it is contained in the LOD level */
	Int32		GetModuleIndex(ParticleModule* inModule);

	/** @return the module at the given index if it is contained in the LOD level */
	FlagGG_API ParticleModule* GetModuleAtIndex(Int32 inIndex);

	/**
	 *	Sets the LOD 'Level' to the given value, properly updating the modules LOD validity settings.
	 *	This function assumes that any error-checking of values was done by the caller!
	 *	It also assumes that when inserting an LOD level, indices will be shifted from lowest to highest...
	 *	When removing one, the will go from highest to lowest.
	 */
	virtual void	SetLevelIndex(Int32 inLevelIndex);

	// For Cascade
	void	AddCurvesToEditor(InterpCurveEdSetup* edSetup);
	void	RemoveCurvesFromEditor(InterpCurveEdSetup* edSetup);
	void	ChangeEditorColor(Color& color, InterpCurveEdSetup* edSetup);

	/**
	 *	Return true if the given module is editable for this LOD level.
	 *
	 *	@param	InModule	The module of interest.
	 *	@return	true		If it is editable for this LOD level.
	 *			false		If it is not.
	 */
	FlagGG_API bool	IsModuleEditable(ParticleModule* inModule);

	/**
	 * Compiles all modules for this LOD level.
	 * @param EmitterBuildInfo - Where to store emitter information.
	 */
	void CompileModules(struct ParticleEmitterBuildInfo& emitterBuildInfo);

	/**
	 * Append all used materials to the material list.
	 * @param OutMaterials - the material list.
	 * @param Slots - the material slot names
	 * @param EmitterMaterials - the material slot materials.
	 */
	void GetUsedMaterials(PODVector<Material*>& outMaterials, const Vector<NamedEmitterMaterial>& namedMaterialSlots, const PODVector<Material*>& emitterMaterials) const;


	/** The index value of the LOD level												*/
	Int32 level_;

	/** True if the LOD level is enabled, meaning it should be updated and rendered.	*/
	UInt32 enabled_ : 1;

	/** The required module for this LOD level											*/
	SharedPtr<ParticleModuleRequired> requiredModule_;

	/** An array of particle modules that contain the adjusted data for the LOD level	*/
	Vector<SharedPtr<ParticleModule>> modules_;

	// Module<SINGULAR> used for emitter type "extension".
	SharedPtr<ParticleModuleTypeDataBase> typeDataModule_;

	/** The SpawnRate/Burst module - required by all emitters. */
	SharedPtr<ParticleModuleSpawn> spawnModule_;

	/** The optional EventGenerator module. */
	// SharedPtr<ParticleModuleEventGenerator> eventGenerator_;

	/** SpawningModules - These are called to determine how many particles to spawn.	*/
	Vector<SharedPtr<ParticleModuleSpawnBase>> spawningModules_;

	/** SpawnModules - These are called when particles are spawned.						*/
	Vector<SharedPtr<ParticleModule>> spawnModules_;

	/** UpdateModules - These are called when particles are updated.					*/
	Vector<SharedPtr<ParticleModule>> updateModules_;

	/** OrbitModules
	 *	These are used to do offsets of the sprite from the particle location.
	 */
	Vector<SharedPtr<ParticleModuleOrbit>> orbitModules_;

	/** Event receiver modules only! */
	// Vector<SharedPtr<ParticleModuleEventReceiverBase>> eventReceiverModules_;

	UInt32 convertedModules_ : 1;

	Int32 peakActiveParticles_;
};


}
