using System;
using System.Globalization;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine.UIElements.StyleSheets;

namespace UnityEngine.UIElements
{
    public struct StyleEnum<T> : IStyleValue<T>, IEquatable<StyleEnum<T>> where T : struct, IConvertible
    {
        public T value
        {
            get { return m_Keyword == StyleKeyword.Undefined ? m_Value : default(T); }
            set
            {
                m_Value = value;
                m_Keyword = StyleKeyword.Undefined;
            }
        }

        internal int specificity
        {
            get { return m_Specificity; }
            set { m_Specificity = value; }
        }

        int IStyleValue<T>.specificity
        {
            get { return specificity; }
            set { specificity = value; }
        }

        public StyleKeyword keyword
        {
            get { return m_Keyword; }
            set { m_Keyword = value; }
        }

        public StyleEnum(T v)
            : this(v, StyleKeyword.Undefined)
        {}

        public StyleEnum(StyleKeyword keyword)
            : this(default(T), keyword)
        {}

        internal StyleEnum(T v, StyleKeyword keyword)
        {
            m_Specificity = StyleValueExtensions.UndefinedSpecificity;
            m_Keyword = keyword;
            m_Value = v;
        }

        internal bool Apply<U>(U other, StylePropertyApplyMode mode) where U : IStyleValue<T>
        {
            if (StyleValueExtensions.CanApply(specificity, other.specificity, mode))
            {
                value = other.value;
                keyword = other.keyword;
                specificity = other.specificity;
                return true;
            }
            return false;
        }

        bool IStyleValue<T>.Apply<U>(U other, StylePropertyApplyMode mode)
        {
            return Apply(other, mode);
        }

        private StyleKeyword m_Keyword;
        private T m_Value;
        private int m_Specificity;

        public static bool operator==(StyleEnum<T> lhs, StyleEnum<T> rhs)
        {
            return lhs.m_Keyword == rhs.m_Keyword && UnsafeUtility.EnumToInt(lhs.m_Value) == UnsafeUtility.EnumToInt(rhs.m_Value);
        }

        public static bool operator!=(StyleEnum<T> lhs, StyleEnum<T> rhs)
        {
            return !(lhs == rhs);
        }

        public static implicit operator StyleEnum<T>(StyleKeyword keyword)
        {
            return new StyleEnum<T>(keyword);
        }

        public static implicit operator StyleEnum<T>(T v)
        {
            return new StyleEnum<T>(v);
        }

        public bool Equals(StyleEnum<T> other)
        {
            return other == this;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is StyleEnum<T>))
            {
                return false;
            }

            var v = (StyleEnum<T>)obj;
            return v == this;
        }

        public override int GetHashCode()
        {
            var hashCode = 917506989;
            hashCode = hashCode * -1521134295 + m_Keyword.GetHashCode();
            hashCode = hashCode * -1521134295 + m_Value.GetHashCode();
            hashCode = hashCode * -1521134295 + m_Specificity.GetHashCode();
            return hashCode;
        }

        public override string ToString()
        {
            return this.DebugString();
        }
    }
}
