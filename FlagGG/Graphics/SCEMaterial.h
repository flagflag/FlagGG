#pragma once

#include "Graphics/Material.h"

namespace FlagGG
{

class SCEMaterial : public Material
{
	OBJECT_OVERRIDE(SCEMaterial, Material);
public:
	SCEMaterial();

	SCEMaterial(bool skinnedMaterial);

	~SCEMaterial() override;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

private:
	bool skinnedMaterial_;
};

}
