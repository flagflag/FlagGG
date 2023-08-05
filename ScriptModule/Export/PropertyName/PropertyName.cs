using System;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine.Scripting;

namespace UnityEngine
{
    [UsedByNativeCode]
    [StructLayout(LayoutKind.Sequential)]
    public struct PropertyName : IEquatable<PropertyName>
    {
        internal int id;
#if UNITY_EDITOR
        internal int conflictIndex;
#endif

        public PropertyName(string name)
            : this(PropertyNameUtils.PropertyNameFromString(name))
        {
        }

        public PropertyName(PropertyName other)
        {
            id = other.id;
#if UNITY_EDITOR
            conflictIndex = other.conflictIndex;
#endif
        }

        public PropertyName(int id)
        {
            this.id = id;
#if UNITY_EDITOR
            this.conflictIndex = 0;
#endif
        }

        public static bool IsNullOrEmpty(PropertyName prop) { return prop.id == 0; }

        public static bool operator==(PropertyName lhs, PropertyName rhs)
        {
            return lhs.id == rhs.id;
        }

        public static bool operator!=(PropertyName lhs, PropertyName rhs)
        {
            return lhs.id != rhs.id;
        }

        public override int GetHashCode()
        {
            return id;
        }

        public override bool Equals(object other)
        {
            return other is PropertyName && Equals((PropertyName)other);
        }

        public bool Equals(PropertyName other)
        {
            return this == other;
        }

        public static implicit operator PropertyName(string name)
        {
            return new PropertyName(name);
        }

        public static implicit operator PropertyName(int id)
        {
            return new PropertyName(id);
        }

#if UNITY_EDITOR
        public override string ToString()
        {
            var conflictCount = PropertyNameUtils.ConflictCountForID(id);
            var msg = string.Format("{0}:{1}", PropertyNameUtils.StringFromPropertyName(this), id);
            if (conflictCount > 0)
            {
                StringBuilder sb = new StringBuilder(msg);
                sb.Append(" conflicts with ");
                for (int i = 0; i < conflictCount; i++)
                {
                    if (i == this.conflictIndex)
                        continue;

                    sb.AppendFormat("\"{0}\"", PropertyNameUtils.StringFromPropertyName(new PropertyName(id) {conflictIndex = i}));
                }
                msg = sb.ToString();
            }
            return msg;
        }

        #else
        public override string ToString()
        {
            return string.Format("Unknown:{0}", id);
        }

        #endif
    }
}
