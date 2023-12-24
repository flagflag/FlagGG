#pragma once

#include "Unit/Actor.h"

using namespace FlagGG;

class ParticleActor : public Actor
{
	OBJECT_OVERRIDE(ParticleActor, Actor);
public:
	ParticleActor();

	~ParticleActor() override;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	bool BeginSave(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndSave() override;

};
