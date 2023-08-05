using System;

namespace UnityEngine.UIElements
{
    public static class MouseCaptureController
    {
#pragma warning disable 414
        static bool m_IsMouseCapturedWarningEmitted = false;
        static bool m_ReleaseMouseWarningEmitted = false;
#pragma warning restore 414

        // TODO 2020.1 [Obsolete("Use PointerCaptureHelper.GetCapturingElement() instead.")]
        public static bool IsMouseCaptured()
        {
#if UNITY_EDITOR
            return EventDispatcher.editorDispatcher.pointerState.GetCapturingElement(PointerId.mousePointerId) != null;
#else
            if (!m_IsMouseCapturedWarningEmitted)
            {
                Debug.LogError("MouseCaptureController.IsMouseCaptured() can not be used in playmode. Please use PointerCaptureHelper.GetCapturingElement() instead.");
                m_IsMouseCapturedWarningEmitted = true;
            }

            return false;
#endif
        }

        public static bool HasMouseCapture(this IEventHandler handler)
        {
            VisualElement ve = handler as VisualElement;
            return ve.HasPointerCapture(PointerId.mousePointerId);
        }

        public static void CaptureMouse(this IEventHandler handler)
        {
            VisualElement ve = handler as VisualElement;
            if (ve != null)
            {
                ve.CapturePointer(PointerId.mousePointerId);
                ve.panel.ProcessPointerCapture(PointerId.mousePointerId);
            }
        }

        public static void ReleaseMouse(this IEventHandler handler)
        {
            VisualElement ve = handler as VisualElement;
            if (ve != null)
            {
                ve.ReleasePointer(PointerId.mousePointerId);
                ve.panel.ProcessPointerCapture(PointerId.mousePointerId);
            }
        }

        // TODO 2020.1 [Obsolete("Use PointerCaptureHelper.ReleasePointer() instead.")]
        public static void ReleaseMouse()
        {
#if UNITY_EDITOR
            PointerCaptureHelper.ReleaseEditorMouseCapture();
#else
            if (!m_ReleaseMouseWarningEmitted)
            {
                Debug.LogError("MouseCaptureController.ReleaseMouse() can not be used in playmode. Please use PointerCaptureHelper.GetCapturingElement() instead.");
                m_ReleaseMouseWarningEmitted = true;
            }
#endif
        }
    }
}
