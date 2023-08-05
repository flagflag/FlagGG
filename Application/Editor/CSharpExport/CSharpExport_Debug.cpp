#include "CSharpExport.h"
#include "EditorExport.h"

#include <Math/Vector3.h>
#include <Math/Color.h>
#include <Log.h>

using namespace FlagGG;

CSharp_API void DebugExport_DebugDrawLine(Vector3 start, Vector3 end, Color color, float duration, bool depthTest)
{
	FLAGGG_LOG_INFO("DebugExport_DebugDrawLine");
}

CSharp_API void DebugExport_PauseEditor()
{
	FLAGGG_LOG_INFO("DebugExport_PauseEditor");
}

CSharp_API void DeveloperConsole_OpenConsoleFile()
{
	FLAGGG_LOG_INFO("DeveloperConsole_OpenConsoleFile");
}
