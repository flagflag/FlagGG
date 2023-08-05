using UnityEngine.Bindings;

namespace UnityEngine
{
    // General settings for how the GUI behaves
    [NativeHeader("Modules/IMGUI/GUISkin.bindings.h")]
    partial class GUISettings
    {
        private static extern float Internal_GetCursorFlashSpeed();
    }
}
