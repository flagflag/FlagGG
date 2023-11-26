#include "ResourceTranslation.h"
#include "SCE/SCEResourceTranslation.h"

namespace FlagGG
{

ResourceTranslation* CreateResourceTranslation(const String& targetEngine)
{
	const String targetEngineLower = targetEngine.ToLower();
	if (targetEngineLower == "sce")
	{
		return new SCEResourceTranslation();
	}
	return nullptr;
}

}
