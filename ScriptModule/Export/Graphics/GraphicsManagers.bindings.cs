using UnityEngine.Bindings;

namespace UnityEngine
{
    // both desiredColorSpace/activeColorSpace should be deprecated
    [NativeHeader("Runtime/Misc/PlayerSettings.h")]
    public sealed partial class QualitySettings : Object
    {
        extern public static ColorSpace desiredColorSpace
        {
            [StaticAccessor("GetPlayerSettings()", StaticAccessorType.Dot)][NativeName("GetColorSpace")] get;
        }
        extern public static ColorSpace activeColorSpace
        {
            [StaticAccessor("GetPlayerSettings()", StaticAccessorType.Dot)][NativeName("GetColorSpace")] get;
        }
    }
}

namespace UnityEngine.Experimental.GlobalIllumination
{
    [NativeHeader("Runtime/Camera/RenderSettings.h")]
    [StaticAccessor("GetRenderSettings()", StaticAccessorType.Dot)]
    public partial class RenderSettings
    {
        extern public static bool useRadianceAmbientProbe { get; set; }
    }
}
