#pragma once

#include <Scene/Node.h>
#include <Container/Str.h>
#include <Container/Ptr.h>
#include <Core/Context.h>

using namespace FlagGG::Container;
using namespace FlagGG::Scene;
using namespace FlagGG::Core;

namespace Importer
{
	extern void SetContext(Context* context);

	extern SharedPtr<Node> ImportScene(const String& path);

	extern SharedPtr<Node> ImportModel(const String& path);
}

