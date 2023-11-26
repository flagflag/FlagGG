#pragma once

#include "ResourceTranslation/ResourceTranslation.h"

namespace FlagGG
{

class XMLElement;

class SCEResourceTranslation : public ResourceTranslation
{
public:
	SCEResourceTranslation();

	~SCEResourceTranslation() override;

	SharedPtr<Image> LoadTexture(const String& path) override;

	bool LoadShader(const String& path, GenericPassDescription& desc) override;

	bool LoadMaterial(const String& path, GenericMaterialDescription& desc) override;

protected:
	/// 从xml加载
	bool LoadMaterialImpl(const XMLElement& source, GenericMaterialDescription& desc);

	/// 加载pass - step1
	bool LoadMaterialImpl_PassStep1(const XMLElement& source, GenericMaterialDescription& desc);

	/// 加载pass - step2
	bool LoadMaterialImpl_PassStep2(const XMLElement& source, GenericMaterialDescription& desc);
};

}
