#pragma once

#include "Container/Str.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"
#include "Graphics/GraphicsDef.h"
#include "ResourceTranslation/TextureTranslation.h"
#include "Resource/Image.h"

namespace FlagGG
{

struct GenericPassDescription
{
	String name_;
	
	String vertexShaderName_;
	String pixelShaderName_;
	String vertexDefines_;
	String pixelDefines_;

	RasterizerState rasterizerState_;
	DepthStencilState depthStencilState_;
};

struct GenericMaterialDescription
{
	Vector<GenericPassDescription> passDescs_;
	HashMap<UInt32, SharedPtr<Image>> textureDescs_;
};

}
