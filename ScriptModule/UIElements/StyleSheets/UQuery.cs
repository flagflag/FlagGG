using System;
using System.Collections.Generic;
using UnityEngine.UIElements.StyleSheets;
using UnityEngine.UIElements;

namespace UnityEngine.UIElements
{
    internal struct RuleMatcher
    {
        public StyleSheet sheet;
        public StyleComplexSelector complexSelector;

        public RuleMatcher(StyleSheet sheet, StyleComplexSelector complexSelector, int styleSheetIndexInStack)
        {
            this.sheet = sheet;
            this.complexSelector = complexSelector;
        }

        public override string ToString()
        {
            return complexSelector.ToString();
        }
    }

    public static class UQuery
    {
        //This scheme saves us 20 bytes instead of saving a Func<object, bool> directly (12 vs 32 bytes)
        internal interface IVisualPredicateWrapper
        {
            bool Predicate(object e);
        }

        internal class IsOfType<T> : IVisualPredicateWrapper where T : VisualElement
        {
            public static IsOfType<T> s_Instance = new IsOfType<T>();

            public bool Predicate(object e)
            {
                return e is T;
            }
        }

        internal class PredicateWrapper<T> : IVisualPredicateWrapper where T : VisualElement
        {
            private Func<T, bool> predicate;
            public PredicateWrapper(Func<T, bool> p)
            {
                predicate = p;
            }

            public bool Predicate(object e)
            {
                T element = e as T;
                if (element != null)
                {
                    return predicate(element);
                }
                return false;
            }
        }

        internal abstract class UQueryMatcher : HierarchyTraversal
        {
            internal List<RuleMatcher> m_Matchers;

            protected UQueryMatcher()
            {
            }

            public override void Traverse(VisualElement element)
            {
                base.Traverse(element);
            }

            protected virtual bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                return false;
            }

            static void NoProcessResult(VisualElement e, MatchResultInfo i) {}

            public override void TraverseRecursive(VisualElement element, int depth)
            {
                int originalCount = m_Matchers.Count;

                int count = m_Matchers.Count; // changes while we iterate so save

                for (int j = 0; j < count; j++)
                {
                    RuleMatcher matcher = m_Matchers[j];

                    if (StyleSelectorHelper.MatchRightToLeft(element, matcher.complexSelector, (e, i) => NoProcessResult(e, i)))
                    {
                        // use by uQuery to determine if we need to stop
                        if (OnRuleMatchedElement(matcher, element))
                        {
                            return;
                        }
                    }
                }

                Recurse(element, depth);

                // Remove all matchers that we could possibly have added at this level of recursion
                if (m_Matchers.Count > originalCount)
                {
                    m_Matchers.RemoveRange(originalCount, m_Matchers.Count - originalCount);
                }
            }

            public virtual void Run(VisualElement root, List<RuleMatcher> matchers)
            {
                m_Matchers = matchers;
                Traverse(root);
            }
        }

        internal abstract class SingleQueryMatcher : UQueryMatcher
        {
            public VisualElement match { get; set; }

            public override void Run(VisualElement root, List<RuleMatcher> matchers)
            {
                match = null;
                base.Run(root, matchers);
            }
        }

        internal class FirstQueryMatcher : SingleQueryMatcher
        {
            protected override bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                if (match == null)
                    match = element;
                return true;
            }
        }

        internal class LastQueryMatcher : SingleQueryMatcher
        {
            protected override bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                match = element;
                return false;
            }
        }

        internal class IndexQueryMatcher : SingleQueryMatcher
        {
            private int matchCount = -1;
            private int _matchIndex;

            public int matchIndex
            {
                get { return _matchIndex; }
                set
                {
                    matchCount = -1;
                    _matchIndex = value;
                }
            }

            public override void Run(VisualElement root, List<RuleMatcher> matchers)
            {
                matchCount = -1;
                base.Run(root, matchers);
            }

            protected override bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                ++matchCount;
                if (matchCount == _matchIndex)
                {
                    match = element;
                }

                return matchCount >= _matchIndex;
            }
        }
    }

    public struct UQueryState<T> : IEquatable<UQueryState<T>> where T : VisualElement
    {
        //this makes it non-thread safe. But saves on allocations...
        private static UQuery.FirstQueryMatcher s_First = new UQuery.FirstQueryMatcher();
        private static UQuery.LastQueryMatcher s_Last = new UQuery.LastQueryMatcher();
        private static UQuery.IndexQueryMatcher s_Index = new UQuery.IndexQueryMatcher();
        private static ActionQueryMatcher s_Action = new ActionQueryMatcher();

        private readonly VisualElement m_Element;
        internal readonly List<RuleMatcher> m_Matchers;

        internal UQueryState(VisualElement element, List<RuleMatcher> matchers)
        {
            m_Element = element;
            m_Matchers = matchers;
        }

        public UQueryState<T> RebuildOn(VisualElement element)
        {
            return new UQueryState<T>(element, m_Matchers);
        }

        public T First()
        {
            s_First.Run(m_Element, m_Matchers);

            // We need to make sure we don't leak a ref to the VisualElement.
            var match = s_First.match as T;
            s_First.match = null;
            return match;
        }

        public T Last()
        {
            s_Last.Run(m_Element, m_Matchers);

            // We need to make sure we don't leak a ref to the VisualElement.
            var match = s_Last.match as T;
            s_Last.match = null;
            return match;
        }

        private class ListQueryMatcher : UQuery.UQueryMatcher
        {
            public List<T> matches { get; set; }

            protected override bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                matches.Add(element as T);
                return false;
            }

            public void Reset()
            {
                matches = null;
            }
        }

        private static readonly ListQueryMatcher s_List = new ListQueryMatcher();

        public void ToList(List<T> results)
        {
            s_List.matches = results;
            s_List.Run(m_Element, m_Matchers);
            s_List.Reset();
        }

        public List<T> ToList()
        {
            List<T> result = new List<T>();
            ToList(result);
            return result;
        }

        public T AtIndex(int index)
        {
            s_Index.matchIndex = index;
            s_Index.Run(m_Element, m_Matchers);

            // We need to make sure we don't leak a ref to the VisualElement.
            var match = s_Index.match as T;
            s_Index.match = null;
            return match;
        }

        //Convoluted trick so save on allocating memory for delegates or lambdas
        private class ActionQueryMatcher : UQuery.UQueryMatcher
        {
            internal Action<T> callBack { get; set; }

            protected override bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                T castedElement = element as T;

                if (castedElement != null)
                {
                    callBack(castedElement);
                }

                return false;
            }
        };

        public void ForEach(Action<T> funcCall)
        {
            var act = s_Action;

            if (act.callBack != null)
            {
                //we're inside a ForEach callback already. we need to allocate :(
                act = new ActionQueryMatcher();
            }

            try
            {
                act.callBack = funcCall;
                act.Run(m_Element, m_Matchers);
            }
            finally
            {
                act.callBack = null;
            }
        }

        private class DelegateQueryMatcher<TReturnType> : UQuery.UQueryMatcher
        {
            public Func<T, TReturnType> callBack { get; set; }

            public List<TReturnType> result { get; set; }

            public static DelegateQueryMatcher<TReturnType> s_Instance = new DelegateQueryMatcher<TReturnType>();

            protected override bool OnRuleMatchedElement(RuleMatcher matcher, VisualElement element)
            {
                T castedElement = element as T;

                if (castedElement != null)
                {
                    result.Add(callBack(castedElement));
                }

                return false;
            }
        }
        public void ForEach<T2>(List<T2> result, Func<T, T2> funcCall)
        {
            var matcher = DelegateQueryMatcher<T2>.s_Instance;

            if (matcher.callBack != null)
            {
                //we're inside a call to ForEach already!, we need to allocate :(
                matcher = new DelegateQueryMatcher<T2>();
            }

            try
            {
                matcher.callBack = funcCall;
                matcher.result = result;
                matcher.Run(m_Element, m_Matchers);
            }
            finally
            {
                matcher.callBack = null;
                matcher.result = null;
            }
        }

        public List<T2> ForEach<T2>(Func<T, T2> funcCall)
        {
            List<T2> result = new List<T2>();
            ForEach(result, funcCall);
            return result;
        }

        public bool Equals(UQueryState<T> other)
        {
            return ReferenceEquals(m_Element, other.m_Element) &&
                EqualityComparer<List<RuleMatcher>>.Default.Equals(m_Matchers, other.m_Matchers);
        }

        public override bool Equals(object obj)
        {
            if (!(obj is UQueryState<T>))
            {
                return false;
            }

            return Equals((UQueryState<T>)obj);
        }

        public override int GetHashCode()
        {
            var hashCode = 488160421;
            hashCode = hashCode * -1521134295 + EqualityComparer<VisualElement>.Default.GetHashCode(m_Element);
            hashCode = hashCode * -1521134295 + EqualityComparer<List<RuleMatcher>>.Default.GetHashCode(m_Matchers);
            return hashCode;
        }

        public static bool operator==(UQueryState<T> state1, UQueryState<T> state2)
        {
            return state1.Equals(state2);
        }

        public static bool operator!=(UQueryState<T> state1, UQueryState<T> state2)
        {
            return !(state1 == state2);
        }
    }

    public struct UQueryBuilder<T> : IEquatable<UQueryBuilder<T>> where T : VisualElement
    {
        private List<StyleSelector> m_StyleSelectors;
        private List<StyleSelector> styleSelectors { get { return m_StyleSelectors ?? (m_StyleSelectors = new List<StyleSelector>()); } }

        private List<StyleSelectorPart> m_Parts;
        private List<StyleSelectorPart> parts { get { return m_Parts ?? (m_Parts = new List<StyleSelectorPart>()); } }
        private VisualElement m_Element;
        private List<RuleMatcher> m_Matchers;
        private StyleSelectorRelationship m_Relationship;

        private int pseudoStatesMask;
        private int negatedPseudoStatesMask;

        public UQueryBuilder(VisualElement visualElement)
            : this()
        {
            m_Element = visualElement;
            m_Parts = null;
            m_StyleSelectors = null;
            m_Relationship = StyleSelectorRelationship.None;
            m_Matchers = new List<RuleMatcher>();
            pseudoStatesMask = negatedPseudoStatesMask = 0;
        }

        public UQueryBuilder<T> Class(string classname)
        {
            AddClass(classname);
            return this;
        }

        public UQueryBuilder<T> Name(string id)
        {
            AddName(id);
            return this;
        }

        public UQueryBuilder<T2> Descendents<T2>(string name = null, params string[] classNames) where T2 : VisualElement
        {
            FinishCurrentSelector();
            AddType<T2>();
            AddName(name);
            AddClasses(classNames);
            return AddRelationship<T2>(StyleSelectorRelationship.Descendent);
        }

        public UQueryBuilder<T2> Descendents<T2>(string name = null, string classname = null) where T2 : VisualElement
        {
            FinishCurrentSelector();
            AddType<T2>();
            AddName(name);
            AddClass(classname);
            return AddRelationship<T2>(StyleSelectorRelationship.Descendent);
        }

        public UQueryBuilder<T2> Children<T2>(string name = null, params string[] classes) where T2 : VisualElement
        {
            FinishCurrentSelector();
            AddType<T2>();
            AddName(name);
            AddClasses(classes);
            return AddRelationship<T2>(StyleSelectorRelationship.Child);
        }

        public UQueryBuilder<T2> Children<T2>(string name = null, string className = null) where T2 : VisualElement
        {
            FinishCurrentSelector();
            AddType<T2>();
            AddName(name);
            AddClass(className);
            return AddRelationship<T2>(StyleSelectorRelationship.Child);
        }

        public UQueryBuilder<T2> OfType<T2>(string name = null, params string[] classes) where T2 : VisualElement
        {
            AddType<T2>();
            AddName(name);
            AddClasses(classes);
            return AddRelationship<T2>(StyleSelectorRelationship.None);
        }

        public UQueryBuilder<T2> OfType<T2>(string name = null, string className = null) where T2 : VisualElement
        {
            AddType<T2>();
            AddName(name);
            AddClass(className);
            return AddRelationship<T2>(StyleSelectorRelationship.None);
        }

        //Only used to avoid allocations in Q<>() Don't use this unless you know what you're doing
        internal UQueryBuilder<T> SingleBaseType()
        {
            parts.Add(StyleSelectorPart.CreatePredicate(UQuery.IsOfType<T>.s_Instance));
            return this;
        }

        public UQueryBuilder<T> Where(Func<T, bool> selectorPredicate)
        {
            //we can't use a static instance as in the QueryState<T>.ForEach below since the query might be long lived
            parts.Add(StyleSelectorPart.CreatePredicate(new UQuery.PredicateWrapper<T>(selectorPredicate)));
            return this;
        }

        private void AddClass(string c)
        {
            if (c != null)
                parts.Add(StyleSelectorPart.CreateClass(c));
        }

        private void AddClasses(params string[] classes)
        {
            if (classes != null)
            {
                for (int i = 0; i < classes.Length; i++)
                    AddClass(classes[i]);
            }
        }

        private void AddName(string id)
        {
            if (id != null)
                parts.Add(StyleSelectorPart.CreateId(id));
        }

        private void AddType<T2>() where T2 : VisualElement
        {
            if (typeof(T2) != typeof(VisualElement))
                parts.Add(StyleSelectorPart.CreatePredicate(UQuery.IsOfType<T2>.s_Instance));
        }

        private UQueryBuilder<T> AddPseudoState(PseudoStates s)
        {
            pseudoStatesMask = pseudoStatesMask | (int)s;
            return this;
        }

        private UQueryBuilder<T> AddNegativePseudoState(PseudoStates s)
        {
            negatedPseudoStatesMask = negatedPseudoStatesMask | (int)s;
            return this;
        }

        public UQueryBuilder<T> Active()
        {
            return AddPseudoState(PseudoStates.Active);
        }

        public UQueryBuilder<T> NotActive()
        {
            return AddNegativePseudoState(PseudoStates.Active);
        }

        public UQueryBuilder<T> Visible()
        {
            return Where(e => e.visible);
        }

        public UQueryBuilder<T> NotVisible()
        {
            return Where(e => !e.visible);
        }

        public UQueryBuilder<T> Hovered()
        {
            return AddPseudoState(PseudoStates.Hover);
        }

        public UQueryBuilder<T> NotHovered()
        {
            return AddNegativePseudoState(PseudoStates.Hover);
        }

        public UQueryBuilder<T> Checked()
        {
            return AddPseudoState(PseudoStates.Checked);
        }

        public UQueryBuilder<T> NotChecked()
        {
            return AddNegativePseudoState(PseudoStates.Checked);
        }

        [Obsolete("Use Checked() instead")]
        public UQueryBuilder<T> Selected()
        {
            return AddPseudoState(PseudoStates.Checked);
        }

        [Obsolete("Use NotChecked() instead")]
        public UQueryBuilder<T> NotSelected()
        {
            return AddNegativePseudoState(PseudoStates.Checked);
        }

        public UQueryBuilder<T> Enabled()
        {
            return AddNegativePseudoState(PseudoStates.Disabled);
        }

        public UQueryBuilder<T> NotEnabled()
        {
            return AddPseudoState(PseudoStates.Disabled);
        }

        public UQueryBuilder<T> Focused()
        {
            return AddPseudoState(PseudoStates.Focus);
        }

        public UQueryBuilder<T> NotFocused()
        {
            return AddNegativePseudoState(PseudoStates.Focus);
        }

        private UQueryBuilder<T2> AddRelationship<T2>(StyleSelectorRelationship relationship) where T2 : VisualElement
        {
            return new UQueryBuilder<T2>(m_Element)
            {
                m_Matchers = m_Matchers,
                m_Parts = m_Parts,
                m_StyleSelectors = m_StyleSelectors,
                m_Relationship = relationship == StyleSelectorRelationship.None ? m_Relationship : relationship,
                pseudoStatesMask = pseudoStatesMask,
                negatedPseudoStatesMask = negatedPseudoStatesMask
            };
        }

        void AddPseudoStatesRuleIfNecessasy()
        {
            if (pseudoStatesMask != 0 ||
                negatedPseudoStatesMask != 0)
            {
                parts.Add(new StyleSelectorPart() {type = StyleSelectorType.PseudoClass});
            }
        }

        private void FinishSelector()
        {
            FinishCurrentSelector();
            if (styleSelectors.Count > 0)
            {
                var selector = new StyleComplexSelector();
                selector.selectors = styleSelectors.ToArray();
                styleSelectors.Clear();
                m_Matchers.Add(new RuleMatcher { complexSelector = selector });
            }
        }

        private bool CurrentSelectorEmpty()
        {
            return parts.Count == 0 &&
                m_Relationship == StyleSelectorRelationship.None &&
                pseudoStatesMask == 0 &&
                negatedPseudoStatesMask == 0;
        }

        private void FinishCurrentSelector()
        {
            if (!CurrentSelectorEmpty())
            {
                StyleSelector sel = new StyleSelector();
                sel.previousRelationship = m_Relationship;

                AddPseudoStatesRuleIfNecessasy();

                sel.parts = m_Parts.ToArray();
                sel.pseudoStateMask = pseudoStatesMask;
                sel.negatedPseudoStateMask = negatedPseudoStatesMask;
                styleSelectors.Add(sel);
                m_Parts.Clear();
                pseudoStatesMask = negatedPseudoStatesMask = 0;
            }
        }

        public UQueryState<T> Build()
        {
            FinishSelector();
            return new UQueryState<T>(m_Element, m_Matchers);
        }

        // Quick One-liners accessors
        public static implicit operator T(UQueryBuilder<T> s)
        {
            return s.First();
        }

        public static bool operator==(UQueryBuilder<T> builder1, UQueryBuilder<T> builder2)
        {
            return builder1.Equals(builder2);
        }

        public static bool operator!=(UQueryBuilder<T> builder1, UQueryBuilder<T> builder2)
        {
            return !(builder1 == builder2);
        }

        public T First()
        {
            return Build().First();
        }

        public T Last()
        {
            return Build().Last();
        }

        public List<T> ToList()
        {
            return Build().ToList();
        }

        public void ToList(List<T> results)
        {
            Build().ToList(results);
        }

        public T AtIndex(int index)
        {
            return Build().AtIndex(index);
        }

        public void ForEach<T2>(List<T2> result, Func<T, T2> funcCall)
        {
            Build().ForEach(result, funcCall);
        }

        public List<T2> ForEach<T2>(Func<T, T2> funcCall)
        {
            return Build().ForEach(funcCall);
        }

        public void ForEach(Action<T> funcCall)
        {
            Build().ForEach(funcCall);
        }

        public bool Equals(UQueryBuilder<T> other)
        {
            return EqualityComparer<List<StyleSelector>>.Default.Equals(m_StyleSelectors, other.m_StyleSelectors) &&
                EqualityComparer<List<StyleSelector>>.Default.Equals(styleSelectors, other.styleSelectors) &&
                EqualityComparer<List<StyleSelectorPart>>.Default.Equals(m_Parts, other.m_Parts) &&
                EqualityComparer<List<StyleSelectorPart>>.Default.Equals(parts, other.parts) && ReferenceEquals(m_Element, other.m_Element) &&
                EqualityComparer<List<RuleMatcher>>.Default.Equals(m_Matchers, other.m_Matchers) &&
                m_Relationship == other.m_Relationship &&
                pseudoStatesMask == other.pseudoStatesMask &&
                negatedPseudoStatesMask == other.negatedPseudoStatesMask;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is UQueryBuilder<T>))
            {
                return false;
            }

            return Equals((UQueryBuilder<T>)obj);
        }

        public override int GetHashCode()
        {
            var hashCode = -949812380;
            hashCode = hashCode * -1521134295 + EqualityComparer<List<StyleSelector>>.Default.GetHashCode(m_StyleSelectors);
            hashCode = hashCode * -1521134295 + EqualityComparer<List<StyleSelector>>.Default.GetHashCode(styleSelectors);
            hashCode = hashCode * -1521134295 + EqualityComparer<List<StyleSelectorPart>>.Default.GetHashCode(m_Parts);
            hashCode = hashCode * -1521134295 + EqualityComparer<List<StyleSelectorPart>>.Default.GetHashCode(parts);
            hashCode = hashCode * -1521134295 + EqualityComparer<VisualElement>.Default.GetHashCode(m_Element);
            hashCode = hashCode * -1521134295 + EqualityComparer<List<RuleMatcher>>.Default.GetHashCode(m_Matchers);
            hashCode = hashCode * -1521134295 + m_Relationship.GetHashCode();
            hashCode = hashCode * -1521134295 + pseudoStatesMask.GetHashCode();
            hashCode = hashCode * -1521134295 + negatedPseudoStatesMask.GetHashCode();
            return hashCode;
        }
    }

    public static class UQueryExtensions
    {
        private static UQueryState<VisualElement> SingleElementEmptyQuery = new UQueryBuilder<VisualElement>(null).Build();

        private static UQueryState<VisualElement> SingleElementNameQuery = new UQueryBuilder<VisualElement>(null).Name(String.Empty).Build();
        private static UQueryState<VisualElement> SingleElementClassQuery = new UQueryBuilder<VisualElement>(null).Class(String.Empty).Build();
        private static UQueryState<VisualElement> SingleElementNameAndClassQuery = new UQueryBuilder<VisualElement>(null).Name(String.Empty).Class(String.Empty).Build();

        private static UQueryState<VisualElement> SingleElementTypeQuery = new UQueryBuilder<VisualElement>(null).SingleBaseType().Build();
        private static UQueryState<VisualElement> SingleElementTypeAndNameQuery = new UQueryBuilder<VisualElement>(null).SingleBaseType().Name(String.Empty).Build();
        private static UQueryState<VisualElement> SingleElementTypeAndClassQuery = new UQueryBuilder<VisualElement>(null).SingleBaseType().Class(String.Empty).Build();
        private static UQueryState<VisualElement> SingleElementTypeAndNameAndClassQuery = new UQueryBuilder<VisualElement>(null).SingleBaseType().Name(String.Empty).Class(String.Empty).Build();

        public static T Q<T>(this VisualElement e, string name = null, params string[] classes) where T : VisualElement
        {
            return e.Query<T>(name, classes).Build().First();
        }

        public static VisualElement Q(this VisualElement e, string name = null, params string[] classes)
        {
            return e.Query<VisualElement>(name, classes).Build().First();
        }

        public static T Q<T>(this VisualElement e, string name = null, string className = null) where T : VisualElement
        {
            if (typeof(T) == typeof(VisualElement))
            {
                return e.Q(name, className) as T;
            }

            UQueryState<VisualElement> query;

            if (name == null)
            {
                if (className == null)
                {
                    query = SingleElementTypeQuery.RebuildOn(e);
                    query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreatePredicate(UQuery.IsOfType<T>.s_Instance);
                    return query.First() as T;
                }

                query = SingleElementTypeAndClassQuery.RebuildOn(e);
                query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreatePredicate(UQuery.IsOfType<T>.s_Instance);
                query.m_Matchers[0].complexSelector.selectors[0].parts[1] = StyleSelectorPart.CreateClass(className);
                return query.First() as T;
            }

            if (className == null)
            {
                query = SingleElementTypeAndNameQuery.RebuildOn(e);
                query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreatePredicate(UQuery.IsOfType<T>.s_Instance);
                query.m_Matchers[0].complexSelector.selectors[0].parts[1] = StyleSelectorPart.CreateId(name);
                return query.First() as T;
            }


            query = SingleElementTypeAndNameAndClassQuery.RebuildOn(e);
            query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreatePredicate(UQuery.IsOfType<T>.s_Instance);
            query.m_Matchers[0].complexSelector.selectors[0].parts[1] = StyleSelectorPart.CreateId(name);
            query.m_Matchers[0].complexSelector.selectors[0].parts[2] = StyleSelectorPart.CreateClass(className);
            return query.First() as T;
        }

        internal static T MandatoryQ<T>(this VisualElement e, string name, string className = null) where T : VisualElement
        {
            var element = e.Q<T>(name, className);
            if (element == null)
                throw new MissingVisualElementException("Element not found: " + name);
            return element;
        }

        public static VisualElement Q(this VisualElement e, string name = null, string className = null)
        {
            UQueryState<VisualElement> query;

            if (name == null)
            {
                if (className == null)
                {
                    return SingleElementEmptyQuery.RebuildOn(e).First();
                }

                query = SingleElementClassQuery.RebuildOn(e);
                query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreateClass(className);
                return query.First();
            }

            if (className == null)
            {
                query = SingleElementNameQuery.RebuildOn(e);
                query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreateId(name);
                return query.First();
            }

            query = SingleElementNameAndClassQuery.RebuildOn(e);
            query.m_Matchers[0].complexSelector.selectors[0].parts[0] = StyleSelectorPart.CreateId(name);
            query.m_Matchers[0].complexSelector.selectors[0].parts[1] = StyleSelectorPart.CreateClass(className);
            return query.First();
        }

        internal static VisualElement MandatoryQ(this VisualElement e, string name, string className = null)
        {
            var element = e.Q<VisualElement>(name, className);
            if (element == null)
                throw new MissingVisualElementException("Element not found: " + name);
            return element;
        }

        public static UQueryBuilder<VisualElement> Query(this VisualElement e, string name = null, params string[] classes)
        {
            return e.Query<VisualElement>(name, classes);
        }

        public static UQueryBuilder<VisualElement> Query(this VisualElement e, string name = null, string className = null)
        {
            return e.Query<VisualElement>(name, className);
        }

        public static UQueryBuilder<T> Query<T>(this VisualElement e, string name = null, params string[] classes) where T : VisualElement
        {
            var queryBuilder = new UQueryBuilder<VisualElement>(e).OfType<T>(name, classes);
            return queryBuilder;
        }

        public static UQueryBuilder<T> Query<T>(this VisualElement e, string name = null, string className = null) where T : VisualElement
        {
            var queryBuilder = new UQueryBuilder<VisualElement>(e).OfType<T>(name, className);
            return queryBuilder;
        }

        public static UQueryBuilder<VisualElement> Query(this VisualElement e)
        {
            return new UQueryBuilder<VisualElement>(e);
        }

        class MissingVisualElementException : Exception
        {
            public MissingVisualElementException()
            {
            }

            public MissingVisualElementException(string message)
                : base(message)
            {
            }
        }
    }
}
