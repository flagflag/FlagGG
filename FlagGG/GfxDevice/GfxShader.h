//
// 抽象图形层Shader
//

#pragma once

#include "GfxDevice/GfxObject.h"
#include "Container/ArrayPtr.h"
#include "Container/Ptr.h"
#include "Graphics/GraphicsDef.h"

namespace FlagGG
{

struct PreProcessShaderInfo;

class FlagGG_API GfxShader : public GfxObject
{
	OBJECT_OVERRIDE(GfxShader, GfxObject);
public:
	explicit GfxShader();

	~GfxShader() override;
	
	// 设置shader类型
	virtual void SetShaderType(ShaderType shaderType);

	// 设置shader代码
	virtual void SetShaderInfo(PreProcessShaderInfo* shaderInfo);

	// 设置宏定义
	virtual void SetDefines(const Vector<String>& defines);

	// 编译
	virtual bool Compile();

protected:
	// shader类型
	ShaderType shaderType_{};

	SharedPtr<PreProcessShaderInfo> shaderInfo_;

	// 宏
	Vector<String> defines_;
	String definesString_;
};

}
