#pragma once

#include "Scene/Node.h"
#include "Container/Ptr.h"
#include "Container/Str.h"

namespace FlagGG
{

FlagGG_API SharedPtr<Node> LoadPrefab(const String& path);

}
