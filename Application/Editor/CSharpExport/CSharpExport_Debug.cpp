#include "CSharpExport.h"
#include "EditorExport.h"

#include <Math/Vector3.h>
#include <Math/Color.h>
#include <Log.h>

using namespace FlagGG;

CSharp_API void DebugDrawLine(Vector3 start, Vector3 end, Color color, float duration, bool depthTest)
{
	FLAGGG_LOG_INFO("DebugDrawLine");
}
