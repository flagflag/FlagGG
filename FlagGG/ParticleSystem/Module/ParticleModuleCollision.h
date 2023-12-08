#pragma once

namespace FlagGG
{

namespace ParticleCollisionResponse
{
	enum Type
	{
		/** The particle will bounce off of the surface. */
		Bounce,
		/** The particle will stop on the surface. */
		Stop,
		/** The particle will be killed. */
		Kill
	};
}

namespace ParticleCollisionMode
{
	enum Type
	{
		SceneDepth,
		DistanceField
	};
}

}
