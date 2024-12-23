using UnityEngine.Scripting;
using UnityEngine.Bindings;

namespace UnityEngine
{
    // Behaviours are Components that can be enabled or disabled.
    [UsedByNativeCode]
    [NativeHeader("Runtime/Mono/MonoBehaviour.h")]
    public class Behaviour : Component
    {
        // Enabled Behaviours are Updated, disabled Behaviours are not.
        [RequiredByNativeCode] // GetFixedBehaviourManager is directly used by fixed update in the player loop
        [NativeProperty]
        extern public bool enabled { get; set; }

        [NativeProperty]
        extern public bool isActiveAndEnabled
        {
            [NativeMethod("IsAddedToManager")]
            get;
        }
    }
}
