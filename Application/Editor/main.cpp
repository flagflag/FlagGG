#include "EditorApplication.h"
#include "main.h"

#include <Memory/MemoryHook.h>

int EntryPoint(int argc, const char* argv[])
{
	EditorApplication app;
	app.Run();
	return 0;
}

IMPLEMENT_MODULE_USD("Editor");
