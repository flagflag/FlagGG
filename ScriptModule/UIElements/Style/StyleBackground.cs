using System;
using UnityEngine.UIElements.StyleSheets;

namespace UnityEngine.UIElements
{
    public struct StyleBackground : IStyleValue<Background>, IEquatable<StyleBackground>
    {
        public Background value
        {
            get { return m_Keyword == StyleKeyword.Undefined ? m_Value : new Background(); }
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

        int IStyleValue<Background>.specificity
        {
            get { return specificity; }
            set { specificity = value; }
        }

        public StyleKeyword keyword
        {
            get { return m_Keyword; }
            set { m_Keyword = value; }
        }

        public StyleBackground(Background v)
            : this(v, StyleKeyword.Undefined)
        {}

        public StyleBackground(Texture2D v)
            : this(v, StyleKeyword.Undefined)
        {}

        public StyleBackground(VectorImage v)
            : this(v, StyleKeyword.Undefined)
        {}

        public StyleBackground(StyleKeyword keyword)
            : this(new Background(), keyword)
        {}

        internal StyleBackground(Texture2D v, StyleKeyword keyword)
            : this(Background.FromTexture2D(v), keyword)
        {}

        internal StyleBackground(VectorImage v, StyleKeyword keyword)
            : this(Background.FromVectorImage(v), keyword)
        {}

        internal StyleBackground(Background v, StyleKeyword keyword)
        {
            m_Specificity = StyleValueExtensions.UndefinedSpecificity;
            m_Keyword = keyword;
            m_Value = v;
        }

        internal bool Apply<U>(U other, StylePropertyApplyMode mode) where U : IStyleValue<Background>
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

        bool IStyleValue<Background>.Apply<U>(U other, StylePropertyApplyMode mode)
        {
            return Apply(other, mode);
        }

        private StyleKeyword m_Keyword;
        private Background m_Value;
        private int m_Specificity;

        public static bool operator==(StyleBackground lhs, StyleBackground rhs)
        {
            return lhs.m_Keyword == rhs.m_Keyword && lhs.m_Value == rhs.m_Value;
        }

        public static bool operator!=(StyleBackground lhs, StyleBackground rhs)
        {
            return !(lhs == rhs);
        }

        public static implicit operator StyleBackground(StyleKeyword keyword)
        {
            return new StyleBackground(keyword);
        }

        public static implicit operator StyleBackground(Background v)
        {
            return new StyleBackground(v);
        }

        public static implicit operator StyleBackground(Texture2D v)
        {
            return new StyleBackground(v);
        }

        public bool Equals(StyleBackground other)
        {
            return other == this;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is StyleBackground))
            {
                return false;
            }

            var v = (StyleBackground)obj;
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
