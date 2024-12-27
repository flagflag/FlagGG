#include "Application/Game/main.h"

#include <Memory/MemoryHook.h>

int main(int argc, const char* argv[])
{
	return EntryPoint(argc, argv);
}

IMPLEMENT_MODULE_USD("GameEntryPoint");
