//
// Metal图形层Shader
//

#pragma once

#include "GfxDevice/GfxShader.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

class GfxShaderMetal : public GfxShader
{
	OBJECT_OVERRIDE(GfxShaderMetal, GfxShader);
public:
	explicit GfxShaderMetal();

	~GfxShaderMetal() override;

	// 编译
	bool Compile() override;

	// 获取Metal shader
	mtlpp::Library& GetMetalShader() { return mtlShader_; }

	// 获取Metal function
	mtlpp::Function& GetMetalFunction() { return mtlFunction_; }

private:
	mtlpp::Library mtlShader_;

	mtlpp::Function mtlFunction_;

	String mtlShaderCode_;
};

}
