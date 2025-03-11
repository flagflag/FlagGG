//
// Gpu纹理压缩
//

#pragma once

#include "Core/Subsystem.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class Texture2D;
class Shader;
class ShaderParameters;

class FlagGG_API GpuTextureCompression : public Subsystem<GpuTextureCompression>
{
public:
	GpuTextureCompression();

	~GpuTextureCompression() override;

	SharedPtr<Texture2D> CompressTexture(Texture2D* uncompressTexture);

private:
	SharedPtr<Shader> compressComputeShader_;

	SharedPtr<ShaderParameters> shaderParameters_;
};

}
