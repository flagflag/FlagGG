using UnityEngine.UIElements.UIR;
using System.Text.RegularExpressions;

namespace UnityEngine.UIElements
{
    internal static class UIRUtility
    {
        public static readonly string k_DefaultShaderName = "Hidden/Internal-UIRDefault";

        public const float k_ClearZ = 0.99f; // Almost at the far plane
        public const float k_MeshPosZ = 0.995f; // The correct z value to use to draw a shape regularly (no clipping)
        public const float k_MaskPosZ = -0.995f; // The correct z value to use to mark a shape to be clipped

        public static Vector4 ToVector4(this Rect rc)
        {
            return new Vector4(rc.xMin, rc.yMin, rc.xMax, rc.yMax);
        }

        public static bool IsRoundRect(VisualElement ve)
        {
            var style = ve.resolvedStyle;
            return !(style.borderTopLeftRadius < Mathf.Epsilon &&
                style.borderTopRightRadius < Mathf.Epsilon &&
                style.borderBottomLeftRadius < Mathf.Epsilon &&
                style.borderBottomRightRadius < Mathf.Epsilon);
        }

        public static bool IsVectorImageBackground(VisualElement ve)
        {
            return ve.computedStyle.backgroundImage.value.vectorImage != null;
        }

        public static void Destroy(Object obj)
        {
            if (obj == null)
                return;
            //if (Application.isPlaying)
            //    Object.Destroy(obj);
            //else
                Object.DestroyImmediate(obj);
        }
    }
}
