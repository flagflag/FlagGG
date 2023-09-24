#pragma once

#include <Scene/Node.h>
#include <Container/Str.h>
#include <Container/Ptr.h>
#include <Core/Context.h>

using namespace FlagGG;

namespace Importer
{

extern SharedPtr<Node> ImportScene(const String& path);

extern SharedPtr<Node> ImportModel(const String& path);

}

