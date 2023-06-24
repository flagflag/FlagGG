using UnityEngine.Bindings;

namespace UnityEngine
{
    [VisibleToOtherModules("UnityEngine.UIElementsModule")]
    internal struct EventInterests
    {
        public bool wantsMouseMove { get; set; }
        public bool wantsMouseEnterLeaveWindow { get; set; }

        public bool WantsEvent(EventType type)
        {
            switch (type)
            {
                case EventType.MouseMove:
                    return wantsMouseMove;
                case EventType.MouseEnterWindow:
                case EventType.MouseLeaveWindow:
                    return wantsMouseEnterLeaveWindow;
                default:
                    return true;
            }
        }
    }
}
