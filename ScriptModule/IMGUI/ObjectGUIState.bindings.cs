using System;
using UnityEngine.Bindings;

namespace UnityEngine
{
    [NativeHeader("Modules/IMGUI/GUIState.h")]
    [VisibleToOtherModules("UnityEngine.UIElementsModule")]
    internal class ObjectGUIState : IDisposable
    {
        internal IntPtr m_Ptr;

        public ObjectGUIState()
        {
            m_Ptr = Internal_Create();
        }

        public void Dispose()
        {
            Destroy();
            GC.SuppressFinalize(this);
        }

        ~ObjectGUIState()
        {
            Destroy();
        }

        void Destroy()
        {
            if (m_Ptr != IntPtr.Zero)
            {
                Internal_Destroy(m_Ptr);
                m_Ptr = IntPtr.Zero;
            }
        }

        private static extern IntPtr Internal_Create();

        [NativeMethod(IsThreadSafe = true)]
        private static extern void Internal_Destroy(IntPtr ptr);
    }
}
