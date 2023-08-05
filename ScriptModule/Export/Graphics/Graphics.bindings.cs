using UnityEngine.Bindings;
using System.Runtime.InteropServices;

#if UNITY_EDITOR
namespace UnityEngine
{
    public sealed partial class Screen
    {
        public static int width = 1600;
        public static int height = 900;
    }

    [NativeHeader("Runtime/Camera/LightProbeProxyVolume.h")]
    [NativeHeader("Runtime/Graphics/ColorGamut.h")]
    [NativeHeader("Runtime/Graphics/CopyTexture.h")]
    [NativeHeader("Runtime/Graphics/GraphicsScriptBindings.h")]
    [NativeHeader("Runtime/Shaders/ComputeShader.h")]
    [NativeHeader("Runtime/Misc/PlayerSettings.h")]
    public partial class Graphics
    {
        [FreeFunction("GraphicsScripting::DrawTexture")]
        [VisibleToOtherModules("UnityEngine.IMGUIModule")]
        [DllImport("Editor.dll", EntryPoint = "Graphics_DrawTexture")]
        extern internal static void Internal_DrawTexture(ref Internal_DrawTextureArguments args);
    }
}
#endif // UNITY_EDITOR
