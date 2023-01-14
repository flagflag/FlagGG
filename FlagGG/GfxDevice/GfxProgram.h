//
// 抽象图形层GPU程序
//

#pragma once

#include "GfxDevice/GfxObject.h"
#include "GfxDevice/GfxShader.h"
#include "Container/Ptr.h"

namespace FlagGG
{

class GfxProgram : public GfxObject
{
	OBJECT_OVERRIDE(GfxProgram, GfxObject);
public:
	explicit GfxProgram();

	~GfxProgram() override;

	// link vs ps
	virtual void Link(GfxShader* vertexShader, GfxShader* pixelShader);

	// link cs
	virtual void LinkComputeShader(GfxShader* computeShader);


	// Get vertex shader
	GfxShader* GetVertexShader() const { return vertexShader_; }

	// Get pixel shader
	GfxShader* GetPixelShader() const { return pixelShader_; }

	// Get compute shader
	GfxShader* GetComputeShader() const { return computeShader_; }

private:
	// vertex shader
	SharedPtr<GfxShader> vertexShader_;

	// pixel shader
	SharedPtr<GfxShader> pixelShader_;

	// compute shader
	SharedPtr<GfxShader> computeShader_;
};

}
