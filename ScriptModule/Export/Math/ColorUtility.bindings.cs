using UnityEngine.Bindings;

namespace UnityEngine
{
    [NativeHeader("Runtime/Export/Math/ColorUtility.bindings.h")]
    public partial class ColorUtility
    {
        [FreeFunction]
        extern internal static bool DoTryParseHtmlColor(string htmlString, out Color32 color);
    }
}
