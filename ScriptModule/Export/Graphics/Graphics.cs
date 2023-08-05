using UnityEngine.Bindings;

namespace UnityEngine
{
    [VisibleToOtherModules("UnityEngine.IMGUIModule")]
    internal struct Internal_DrawTextureArguments
    {
        public Rect screenRect, sourceRect;
        public int leftBorder, rightBorder, topBorder, bottomBorder;
        public Color leftBorderColor, rightBorderColor, topBorderColor, bottomBorderColor;
        public Color color;
        public Vector4 borderWidths;
        public Vector4 cornerRadiuses;
        public bool smoothCorners;
        public int pass;
        public Texture texture;
        public Material mat;
    }
}
