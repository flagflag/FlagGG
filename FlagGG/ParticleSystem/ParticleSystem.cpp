#include "ParticleSystem.h"
#include "Math/InterpCurveEdSetup.h"
#include "ParticleSystem/ParticleSystemComponent.h"
#include "ParticleSystem/ParticleEmitter.h"

namespace FlagGG
{

ParticleSystem::ParticleSystem()
	: occlusionBoundsMethod_(EPSOBM_ParticleBounds)
{

}

ParticleSystem::~ParticleSystem() = default;

}
