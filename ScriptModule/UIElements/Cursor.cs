using System;
using System.Collections.Generic;

namespace UnityEngine.UIElements
{
    public struct Cursor : IEquatable<Cursor>
    {
        public Texture2D texture { get; set; }
        public Vector2 hotspot { get; set; }
        // Used to support default cursor in the editor (map to MouseCursor enum)
        internal int defaultCursorId { get; set; }

        public override bool Equals(object obj)
        {
            return obj is Cursor && Equals((Cursor)obj);
        }

        public bool Equals(Cursor other)
        {
            return EqualityComparer<Texture2D>.Default.Equals(texture, other.texture) &&
                hotspot.Equals(other.hotspot) &&
                defaultCursorId == other.defaultCursorId;
        }

        public override int GetHashCode()
        {
            var hashCode = 1500536833;
            hashCode = hashCode * -1521134295 + EqualityComparer<Texture2D>.Default.GetHashCode(texture);
            hashCode = hashCode * -1521134295 + EqualityComparer<Vector2>.Default.GetHashCode(hotspot);
            hashCode = hashCode * -1521134295 + defaultCursorId.GetHashCode();
            return hashCode;
        }

        public static bool operator==(Cursor style1, Cursor style2)
        {
            return style1.Equals(style2);
        }

        public static bool operator!=(Cursor style1, Cursor style2)
        {
            return !(style1 == style2);
        }

        public override string ToString()
        {
            return $"texture={texture}, hotspot={hotspot}";
        }
    }

    internal interface ICursorManager
    {
        void SetCursor(Cursor cursor);
        void ResetCursor();
    }

    internal class CursorManager : ICursorManager
    {
        public void SetCursor(Cursor cursor)
        {
            if (cursor.texture != null)
            {
                UnityEngine.Cursor.SetCursor(cursor.texture, cursor.hotspot, CursorMode.Auto);
            }
            else
            {
                if (cursor.defaultCursorId != 0)
                {
                    Debug.LogWarning(
                        "Runtime does not support setting a cursor without a texture. Use ResetCursor() to reset the cursor to the default cursor.");
                }

                ResetCursor();
            }
        }

        public void ResetCursor()
        {
            UnityEngine.Cursor.SetCursor(null, Vector2.zero, CursorMode.Auto);
        }
    }
}
