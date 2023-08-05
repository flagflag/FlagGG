//#define ENABLE_CAPTURE_DEBUG
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using UnityEngine.Yoga;
using UnityEngine.UIElements.StyleSheets;
using UnityEngine.UIElements.UIR;
using PropertyBagValue = System.Collections.Generic.KeyValuePair<UnityEngine.PropertyName, object>;

namespace UnityEngine.UIElements
{
    // pseudo states are used for common states of a widget
    // they are addressable from CSS via the pseudo state syntax ":selected" for example
    // while css class list can solve the same problem, pseudo states are a fast commonly agreed upon path for common cases.
    [Flags]
    internal enum PseudoStates
    {
        Active    = 1 << 0,     // control is currently pressed in the case of a button
        Hover     = 1 << 1,     // mouse is over control, set and removed from dispatcher automatically
        Checked   = 1 << 3,     // usually associated with toggles of some kind to change visible style
        Disabled  = 1 << 5,     // control will not respond to user input
        Focus     = 1 << 6,     // control has the keyboard focus. This is activated deactivated by the dispatcher automatically
        Root      = 1 << 7,     // set on the root visual element
    }

    public enum PickingMode
    {
        Position, // todo better name
        Ignore
    }

    internal static class VisualElementListPool
    {
        static ObjectPool<List<VisualElement>> pool = new ObjectPool<List<VisualElement>>(20);

        public static List<VisualElement> Copy(List<VisualElement> elements)
        {
            var result = pool.Get();

            result.AddRange(elements);

            return result;
        }

        public static List<VisualElement> Get(int initialCapacity = 0)
        {
            List<VisualElement> result = pool.Get();

            if (initialCapacity > 0 && result.Capacity < initialCapacity)
            {
                result.Capacity = initialCapacity;
            }
            return result;
        }

        public static void Release(List<VisualElement> elements)
        {
            elements.Clear();
            pool.Release(elements);
        }
    }

    public partial class VisualElement : Focusable, ITransform
    {
        public class UxmlFactory : UxmlFactory<VisualElement, UxmlTraits> {}

        public class UxmlTraits : UIElements.UxmlTraits
        {
            UxmlStringAttributeDescription m_Name = new UxmlStringAttributeDescription { name = "name" };
            UxmlStringAttributeDescription m_ViewDataKey = new UxmlStringAttributeDescription { name = "view-data-key" };
            protected UxmlEnumAttributeDescription<PickingMode> m_PickingMode = new UxmlEnumAttributeDescription<PickingMode> { name = "picking-mode", obsoleteNames = new[] { "pickingMode" }};
            UxmlStringAttributeDescription m_Tooltip = new UxmlStringAttributeDescription { name = "tooltip" };
            UxmlEnumAttributeDescription<UsageHints> m_UsageHints = new UxmlEnumAttributeDescription<UsageHints> { name = "usage-hints" };

            // focusIndex is obsolete. It has been replaced by tabIndex and focusable.
            protected UxmlIntAttributeDescription focusIndex { get; set; } = new UxmlIntAttributeDescription { name = null, obsoleteNames = new[] { "focus-index", "focusIndex" }, defaultValue = -1 };
            UxmlIntAttributeDescription m_TabIndex = new UxmlIntAttributeDescription { name = "tabindex", defaultValue = 0 };
            protected UxmlBoolAttributeDescription focusable { get; set; } = new UxmlBoolAttributeDescription { name = "focusable", defaultValue = false };

#pragma warning disable 414
            // These variables are used by reflection.
            UxmlStringAttributeDescription m_Class = new UxmlStringAttributeDescription { name = "class" };
            UxmlStringAttributeDescription m_ContentContainer = new UxmlStringAttributeDescription { name = "content-container", obsoleteNames = new[] { "contentContainer" } };
            UxmlStringAttributeDescription m_Style = new UxmlStringAttributeDescription { name = "style" };
#pragma warning restore

            public override IEnumerable<UxmlChildElementDescription> uxmlChildElementsDescription
            {
                get { yield return new UxmlChildElementDescription(typeof(VisualElement)); }
            }

            public override void Init(VisualElement ve, IUxmlAttributes bag, CreationContext cc)
            {
                base.Init(ve, bag, cc);

                if (ve == null)
                {
                    throw new ArgumentNullException(nameof(ve));
                }

                ve.name = m_Name.GetValueFromBag(bag, cc);
                ve.viewDataKey = m_ViewDataKey.GetValueFromBag(bag, cc);
                ve.pickingMode = m_PickingMode.GetValueFromBag(bag, cc);
                // usageHints can't be changed if the element is attached to a panel. Here we ignore usageHints if the VE is attached to a panel already.
                // This happens in the UIBuilder, and the hints don't cause visual or behavioral differences, so it is safe from an editing experience stand point.
                if (ve.panel == null)
                    ve.usageHints = m_UsageHints.GetValueFromBag(bag, cc);

                int index = 0;
                if (focusIndex.TryGetValueFromBag(bag, cc, ref index))
                {
                    ve.tabIndex = index >= 0 ? index : 0;
                    ve.focusable = index >= 0;
                }

                // tabIndex and focusable overrides obsolete focusIndex.
                if (m_TabIndex.TryGetValueFromBag(bag, cc, ref index))
                {
                    ve.tabIndex = index;
                }

                bool focus = false;

                if (focusable.TryGetValueFromBag(bag, cc, ref focus))
                {
                    ve.focusable = focus;
                }

                ve.tooltip = m_Tooltip.GetValueFromBag(bag, cc);

                // We ignore m_Class, it was processed in UIElementsViewImporter.
                // We ignore m_ContentContainer, it was processed in UIElementsViewImporter.
                // We ignore m_Style, it was processed in UIElementsViewImporter.
            }
        }

        internal bool isCompositeRoot { get; /*protected*/ set; }

        private static uint s_NextId;

        private static List<string> s_EmptyClassList = new List<string>(0);

        internal static readonly PropertyName userDataPropertyKey = new PropertyName("--unity-user-data");
        public static readonly string disabledUssClassName = "unity-disabled";

        string m_Name;
        List<string> m_ClassList;
        string m_TypeName;
        string m_FullTypeName;
        private List<PropertyBagValue> m_PropertyBag;

        // Used for view data persistence (ie. scroll position or tree view expanded states)
        private string m_ViewDataKey;
        public string viewDataKey
        {
            get { return m_ViewDataKey; }
            set
            {
                if (m_ViewDataKey != value)
                {
                    m_ViewDataKey = value;

                    if (!string.IsNullOrEmpty(value))
                        IncrementVersion(VersionChangeType.ViewData);
                }
            }
        }

        // Persistence of view data is almost always enabled as long as an element has
        // a valid viewDataKey. The only exception is when an element is in its parent's
        // shadow tree, that is, not a physical child of its logical parent's contentContainer.
        // In this exception case, persistence is disabled on the element even if the element
        // does have a viewDataKey, if its logical parent does not have a viewDataKey.
        // This check internally controls whether or not view data persistence is enabled as
        // the VisualTreeViewDataUpdater traverses the visual tree.
        internal bool enableViewDataPersistence { get; private set; }

        public object userData
        {
            get { return GetPropertyInternal(userDataPropertyKey); }
            set { SetPropertyInternal(userDataPropertyKey, value); }
        }

        public override bool canGrabFocus
        {
            get
            {
                bool focusDisabledOnComposite = false;
                VisualElement p = hierarchy.parent;
                while (p != null)
                {
                    if (p.isCompositeRoot)
                    {
                        focusDisabledOnComposite |= !p.canGrabFocus;
                        break;
                    }
                    p = p.parent;
                }

                return !focusDisabledOnComposite && visible && (resolvedStyle.display != DisplayStyle.None) && enabledInHierarchy && base.canGrabFocus;
            }
        }

        public override FocusController focusController
        {
            get { return panel?.focusController; }
        }

        public UsageHints usageHints
        {
            get
            {
                return
                    (((m_RenderHints & RenderHints.GroupTransform) != 0) ? UsageHints.GroupTransform : 0) |
                    (((m_RenderHints & RenderHints.BoneTransform) != 0) ? UsageHints.DynamicTransform : 0);
            }
            set
            {
                if (panel != null)
                    throw new InvalidOperationException("usageHints cannot be changed once the VisualElement is part of an active visual tree");

                // Preserve hints not exposed through UsageHints
                if ((value & UsageHints.GroupTransform) != 0)
                    m_RenderHints |= RenderHints.GroupTransform;
                else m_RenderHints &= ~RenderHints.GroupTransform;

                if ((value & UsageHints.DynamicTransform) != 0)
                    m_RenderHints |= RenderHints.BoneTransform;
                else m_RenderHints &= ~RenderHints.BoneTransform;
            }
        }

        private RenderHints m_RenderHints;
        internal RenderHints renderHints
        {
            get { return m_RenderHints; }
            set
            {
                if (panel != null)
                    throw new InvalidOperationException("renderHints cannot be changed once the VisualElement is part of an active visual tree");
                m_RenderHints = value;
            }
        }

        internal Rect lastLayout;
        internal RenderChainVEData renderChainData;

        Vector3 m_Position = Vector3.zero;
        Quaternion m_Rotation = Quaternion.identity;
        Vector3 m_Scale = Vector3.one;

        public ITransform transform
        {
            get { return this; }
        }

        Vector3 ITransform.position
        {
            get
            {
                return m_Position;
            }
            set
            {
                if (m_Position == value)
                    return;
                m_Position = value;
                IncrementVersion(VersionChangeType.Transform);
            }
        }

        Quaternion ITransform.rotation
        {
            get
            {
                return m_Rotation;
            }
            set
            {
                if (m_Rotation == value)
                    return;
                m_Rotation = value;

                IncrementVersion(VersionChangeType.Transform);
            }
        }

        Vector3 ITransform.scale
        {
            get
            {
                return m_Scale;
            }
            set
            {
                if (m_Scale == value)
                    return;
                m_Scale = value;
                IncrementVersion(VersionChangeType.Transform | VersionChangeType.Layout /*This will change how we measure text*/);
            }
        }

        internal Vector3 ComputeGlobalScale()
        {
            Vector3 result = m_Scale;

            var ve = this.hierarchy.parent;

            while (ve != null)
            {
                result.Scale(ve.m_Scale);
                ve = ve.hierarchy.parent;
            }
            return result;
        }

        Matrix4x4 ITransform.matrix
        {
            get { return Matrix4x4.TRS(m_Position, m_Rotation, m_Scale); }
        }

        internal bool isLayoutManual { get; private set; }


        internal float scaledPixelsPerPoint
        {
            get { return panel == null ? GUIUtility.pixelsPerPoint : (panel as BaseVisualElementPanel).scaledPixelsPerPoint; }
        }

        Rect m_Layout;

        // This will replace the Rect position
        // origin and size relative to parent
        public Rect layout
        {
            get
            {
                var result = m_Layout;
                if (yogaNode != null && !isLayoutManual)
                {
                    result.x = yogaNode.LayoutX;
                    result.y = yogaNode.LayoutY;
                    result.width = yogaNode.LayoutWidth;
                    result.height = yogaNode.LayoutHeight;
                }
                return result;
            }
            internal set
            {
                if (yogaNode == null)
                {
                    yogaNode = new YogaNode();
                }

                // Same position value while type is already manual should not trigger any layout change, return early
                if (isLayoutManual && m_Layout == value)
                    return;

                Rect lastLayout = layout;
                VersionChangeType changeType = 0;
                if (!Mathf.Approximately(lastLayout.x, value.x) || !Mathf.Approximately(lastLayout.y, value.y))
                    changeType |= VersionChangeType.Transform;
                if (!Mathf.Approximately(lastLayout.width, value.width) || !Mathf.Approximately(lastLayout.height, value.height))
                    changeType |= VersionChangeType.Size;

                // set results so we can read straight back in get without waiting for a pass
                m_Layout = value;
                isLayoutManual = true;

                // mark as inline so that they do not get overridden if needed.
                IStyle styleAccess = style;
                styleAccess.position = Position.Absolute;
                styleAccess.marginLeft = 0.0f;
                styleAccess.marginRight = 0.0f;
                styleAccess.marginBottom = 0.0f;
                styleAccess.marginTop = 0.0f;
                styleAccess.left = value.x;
                styleAccess.top = value.y;
                styleAccess.right = float.NaN;
                styleAccess.bottom = float.NaN;
                styleAccess.width = value.width;
                styleAccess.height = value.height;

                if (changeType != 0)
                    IncrementVersion(changeType);
            }
        }

        public Rect contentRect
        {
            get
            {
                var spacing = new Spacing(resolvedStyle.paddingLeft,
                    resolvedStyle.paddingTop,
                    resolvedStyle.paddingRight,
                    resolvedStyle.paddingBottom);

                return paddingRect - spacing;
            }
        }

        protected Rect paddingRect
        {
            get
            {
                var spacing = new Spacing(resolvedStyle.borderLeftWidth,
                    resolvedStyle.borderTopWidth,
                    resolvedStyle.borderRightWidth,
                    resolvedStyle.borderBottomWidth);

                return rect - spacing;
            }
        }

        internal static Rect TransformAlignedRect(Matrix4x4 lhc, Rect rect)
        {
            var min = MultiplyMatrix44Point2(lhc, rect.min);
            var max = MultiplyMatrix44Point2(lhc, rect.max);

            // We assume that the transform performs translation/scaling without rotation.
            return Rect.MinMaxRect(Math.Min(min.x, max.x), Math.Min(min.y, max.y), Math.Max(min.x, max.x), Math.Max(min.y, max.y));
        }

        internal static Vector2 MultiplyMatrix44Point2(Matrix4x4 lhs, Vector2 point)
        {
            Vector2 res;
            res.x = lhs.m00 * point.x + lhs.m01 * point.y + lhs.m03;
            res.y = lhs.m10 * point.x + lhs.m11 * point.y + lhs.m13;
            return res;
        }

        internal bool isBoundingBoxDirty = true;
        private Rect m_BoundingBox;
        internal bool isWorldBoundingBoxDirty = true;
        private Rect m_WorldBoundingBox;

        internal Rect boundingBox
        {
            get
            {
                if (isBoundingBoxDirty)
                {
                    UpdateBoundingBox();
                    isBoundingBoxDirty = false;
                }

                return m_BoundingBox;
            }
        }

        internal Rect worldBoundingBox
        {
            get
            {
                if (isWorldBoundingBoxDirty || isBoundingBoxDirty)
                {
                    UpdateWorldBoundingBox();
                    isWorldBoundingBoxDirty = false;
                }

                return m_WorldBoundingBox;
            }
        }

        internal void UpdateBoundingBox()
        {
            if (float.IsNaN(rect.x) || float.IsNaN(rect.y) || float.IsNaN(rect.width) || float.IsNaN(rect.height))
            {
                // Ignored unlayouted VisualElements.
                m_BoundingBox = Rect.zero;
            }
            else
            {
                m_BoundingBox = rect;
                var childCount = m_Children.Count;
                for (int i = 0; i < childCount; i++)
                {
                    var childBB = m_Children[i].boundingBox;

                    // Use localtransform instead
                    childBB = m_Children[i].ChangeCoordinatesTo(this, childBB);
                    m_BoundingBox.xMin = Math.Min(m_BoundingBox.xMin, childBB.xMin);
                    m_BoundingBox.xMax = Math.Max(m_BoundingBox.xMax, childBB.xMax);
                    m_BoundingBox.yMin = Math.Min(m_BoundingBox.yMin, childBB.yMin);
                    m_BoundingBox.yMax = Math.Max(m_BoundingBox.yMax, childBB.yMax);
                }
            }

            isWorldBoundingBoxDirty = true;
        }

        internal void UpdateWorldBoundingBox()
        {
            m_WorldBoundingBox = TransformAlignedRect(worldTransform, boundingBox);
        }

        /// <summary>
        /// AABB after applying the world transform to <c>rect</c>.
        /// </summary>
        public Rect worldBound
        {
            get
            {
                var g = worldTransform;

                return TransformAlignedRect(g, rect);
            }
        }

        /// <summary>
        /// AABB after applying the transform to the rect, but before applying the layout translation.
        /// </summary>
        public Rect localBound
        {
            get
            {
                var g = transform.matrix;

                var l = layout;

                return TransformAlignedRect(g, l);
            }
        }

        internal Rect rect
        {
            get
            {
                return new Rect(0.0f, 0.0f, layout.width, layout.height);
            }
        }

        internal bool isWorldTransformDirty { get; set; } = true;
        internal bool isWorldTransformInverseDirty { get; set; } = true;

        private Matrix4x4 m_WorldTransformCache = Matrix4x4.identity;
        private Matrix4x4 m_WorldTransformInverseCache = Matrix4x4.identity;

        /// <summary>
        /// Returns a matrix that cumulates the following operations (in order):
        /// -Local Scaling
        /// -Local Rotation
        /// -Local Translation
        /// -Layout Translation
        /// -Parent <c>worldTransform</c> (recursive definition - consider identity when there is no parent)
        /// </summary>
        /// <remarks>
        /// Multiplying the <c>layout</c> rect by this matrix is incorrect because it already contains the translation.
        /// </remarks>
        public Matrix4x4 worldTransform
        {
            get
            {
                if (isWorldTransformDirty)
                {
                    UpdateWorldTransform();
                }
                return m_WorldTransformCache;
            }
        }

        internal Matrix4x4 worldTransformInverse
        {
            get
            {
                if (isWorldTransformDirty || isWorldTransformInverseDirty)
                {
                    m_WorldTransformInverseCache = worldTransform.inverse;
                    isWorldTransformInverseDirty = false;
                }
                return m_WorldTransformInverseCache;
            }
        }

        private void UpdateWorldTransform()
        {
            // If we are during a layout we don't want to remove the dirty transform flag
            // since this could lead to invalid computed transform (see ScopeContentainer.DoMeasure)
            if (elementPanel != null && !elementPanel.duringLayoutPhase)
            {
                isWorldTransformDirty = false;
            }

            var offset = Matrix4x4.Translate(new Vector3(layout.x, layout.y, 0));
            if (hierarchy.parent != null)
            {
                m_WorldTransformCache = hierarchy.parent.worldTransform * offset * transform.matrix;
            }
            else
            {
                m_WorldTransformCache = offset * transform.matrix;
            }

            isWorldTransformInverseDirty = true;
            isWorldBoundingBoxDirty = true;
        }

        internal bool isWorldClipDirty { get; set; } = true;

        private Rect m_WorldClip = Rect.zero;
        private Rect m_WorldClipMinusGroup = Rect.zero;
        internal Rect worldClip
        {
            get
            {
                if (isWorldClipDirty)
                {
                    UpdateWorldClip();
                    isWorldClipDirty = false;
                }

                return m_WorldClip;
            }
        }
        internal Rect worldClipMinusGroup
        {
            get
            {
                if (isWorldClipDirty)
                {
                    UpdateWorldClip();
                    isWorldClipDirty = false;
                }
                return m_WorldClipMinusGroup;
            }
        }

        private static readonly Rect s_InfiniteRect = new Rect(-10000, -10000, 40000, 40000);

        private void UpdateWorldClip()
        {
            if (hierarchy.parent != null)
            {
                m_WorldClip = hierarchy.parent.worldClip;
                if (hierarchy.parent != renderChainData.groupTransformAncestor) // Accessing render data here?
                    m_WorldClipMinusGroup = hierarchy.parent.worldClipMinusGroup;
                else m_WorldClipMinusGroup = panel?.contextType == ContextType.Player ? s_InfiniteRect : GUIClip.topmostRect;

                if (ShouldClip())
                {
                    // Case 1222517: We must substract before intersection. Otherwise, if the parent world clip
                    // boundary happens to be overlapping the element, we may be over-substracting. Also clamping must
                    // be the last operation that's performed.
                    Rect wb = SubstractBorderPadding(worldBound);

                    float x1 = Mathf.Max(wb.xMin, m_WorldClip.xMin);
                    float x2 = Mathf.Min(wb.xMax, m_WorldClip.xMax);
                    float y1 = Mathf.Max(wb.yMin, m_WorldClip.yMin);
                    float y2 = Mathf.Min(wb.yMax, m_WorldClip.yMax);
                    float width = Mathf.Max(x2 - x1, 0);
                    float height = Mathf.Max(y2 - y1, 0);
                    m_WorldClip = new Rect(x1, y1, width, height);

                    x1 = Mathf.Max(wb.xMin, m_WorldClipMinusGroup.xMin);
                    x2 = Mathf.Min(wb.xMax, m_WorldClipMinusGroup.xMax);
                    y1 = Mathf.Max(wb.yMin, m_WorldClipMinusGroup.yMin);
                    y2 = Mathf.Min(wb.yMax, m_WorldClipMinusGroup.yMax);
                    width = Mathf.Max(x2 - x1, 0);
                    height = Mathf.Max(y2 - y1, 0);
                    m_WorldClipMinusGroup = new Rect(x1, y1, width, height);
                }
            }
            else
            {
                m_WorldClipMinusGroup = m_WorldClip = (panel != null) ? panel.visualTree.rect : s_InfiniteRect;
            }
        }

        private Rect SubstractBorderPadding(Rect worldRect)
        {
            // Case 1222517: We must take the scaling into consideration when applying local changes to the world rect.
            float xScale = worldTransform.m00;
            float yScale = worldTransform.m11;

            worldRect.x += resolvedStyle.borderLeftWidth * xScale;
            worldRect.y += resolvedStyle.borderTopWidth * yScale;
            worldRect.width -= (resolvedStyle.borderLeftWidth + resolvedStyle.borderRightWidth) * xScale;
            worldRect.height -= (resolvedStyle.borderTopWidth + resolvedStyle.borderBottomWidth) * yScale;

            if (computedStyle.unityOverflowClipBox == OverflowClipBox.ContentBox)
            {
                worldRect.x += resolvedStyle.paddingLeft * xScale;
                worldRect.y += resolvedStyle.paddingTop * yScale;
                worldRect.width -= (resolvedStyle.paddingLeft + resolvedStyle.paddingRight) * xScale;
                worldRect.height -= (resolvedStyle.paddingTop + resolvedStyle.paddingBottom) * yScale;
            }

            return worldRect;
        }

        // get the AA aligned bound
        internal static Rect ComputeAAAlignedBound(Rect position, Matrix4x4 mat)
        {
            Rect p = position;
            Vector3 v0 = mat.MultiplyPoint3x4(new Vector3(p.x, p.y, 0.0f));
            Vector3 v1 = mat.MultiplyPoint3x4(new Vector3(p.x + p.width, p.y, 0.0f));
            Vector3 v2 = mat.MultiplyPoint3x4(new Vector3(p.x, p.y + p.height, 0.0f));
            Vector3 v3 = mat.MultiplyPoint3x4(new Vector3(p.x + p.width, p.y + p.height, 0.0f));
            return Rect.MinMaxRect(
                Mathf.Min(v0.x, Mathf.Min(v1.x, Mathf.Min(v2.x, v3.x))),
                Mathf.Min(v0.y, Mathf.Min(v1.y, Mathf.Min(v2.y, v3.y))),
                Mathf.Max(v0.x, Mathf.Max(v1.x, Mathf.Max(v2.x, v3.x))),
                Mathf.Max(v0.y, Mathf.Max(v1.y, Mathf.Max(v2.y, v3.y))));
        }

        // which pseudo states would change the current VE styles if added
        internal PseudoStates triggerPseudoMask;
        // which pseudo states would change the current VE styles if removed
        internal PseudoStates dependencyPseudoMask;

        private PseudoStates m_PseudoStates;
        internal PseudoStates pseudoStates
        {
            get { return m_PseudoStates; }
            set
            {
                if (m_PseudoStates != value)
                {
                    m_PseudoStates = value;

                    if ((triggerPseudoMask & m_PseudoStates) != 0
                        || (dependencyPseudoMask & ~m_PseudoStates) != 0)
                    {
                        IncrementVersion(VersionChangeType.StyleSheet);
                    }
                }
            }
        }

        public PickingMode pickingMode { get; set; }

        // does not guarantee uniqueness
        public string name
        {
            get { return m_Name; }
            set
            {
                if (m_Name == value)
                    return;
                m_Name = value;
                IncrementVersion(VersionChangeType.StyleSheet);
            }
        }

        internal List<string> classList
        {
            get { return m_ClassList; }
        }

        internal string fullTypeName
        {
            get
            {
                if (string.IsNullOrEmpty(m_FullTypeName))
                    m_FullTypeName = GetType().FullName;
                return m_FullTypeName;
            }
        }

        internal string typeName
        {
            get
            {
                if (string.IsNullOrEmpty(m_TypeName))
                {
                    var type = GetType();
                    bool isGeneric = type.IsGenericType;
                    m_TypeName = type.Name;

                    if (isGeneric)
                    {
                        int genericTypeIndex = m_TypeName.IndexOf('`');
                        if (genericTypeIndex >= 0)
                        {
                            m_TypeName = m_TypeName.Remove(genericTypeIndex);
                        }
                    }
                }

                return m_TypeName;
            }
        }

        // Set and pass in values to be used for layout
        internal YogaNode yogaNode { get; private set; }

        // shared style object, cannot be changed by the user
        internal VisualElementStylesData m_SharedStyle = VisualElementStylesData.none;
        // user-defined style object, if not set, is the same reference as m_SharedStyles
        internal VisualElementStylesData m_Style = VisualElementStylesData.none;

        internal VisualElementStylesData sharedStyle
        {
            get
            {
                return m_SharedStyle;
            }
        }

        internal VisualElementStylesData specifiedStyle
        {
            get
            {
                return m_Style;
            }
        }

        // Variables that children inherit
        internal StyleVariableContext variableContext = StyleVariableContext.none;

        // Styles that children inherit
        internal InheritedStylesData propagatedStyle = InheritedStylesData.none;

        private InheritedStylesData m_InheritedStylesData = InheritedStylesData.none;
        // Styles inherited from the parent
        internal InheritedStylesData inheritedStyle
        {
            get { return m_InheritedStylesData; }
            set
            {
                if (!ReferenceEquals(m_InheritedStylesData, value))
                {
                    m_InheritedStylesData = value;
                    IncrementVersion(VersionChangeType.Repaint | VersionChangeType.Styles | VersionChangeType.Layout);
                }
            }
        }

        internal bool hasInlineStyle
        {
            get { return m_Style != m_SharedStyle; }
        }

        internal ComputedStyle computedStyle { get; private set; }

        // Opacity is not fully supported so it's hidden from public API for now
        internal float opacity
        {
            get { return resolvedStyle.opacity; }
            set
            {
                style.opacity = value;
            }
        }

        internal readonly uint controlid;

        // IMGUIContainers are special snowflakes that need custom treatment regarding events.
        // This enables early outs in some dispatching strategies.
        // see focusable.isIMGUIContainer;
        internal int imguiContainerDescendantCount = 0;

        private void ChangeIMGUIContainerCount(int delta)
        {
            VisualElement ve = this;

            while (ve != null)
            {
                ve.imguiContainerDescendantCount += delta;
                ve = ve.hierarchy.parent;
            }
        }

        public VisualElement()
        {
            m_Children = s_EmptyList;
            controlid = ++s_NextId;

            hierarchy = new Hierarchy(this);
            computedStyle = new ComputedStyle(this);

            m_ClassList = s_EmptyClassList;
            m_FullTypeName = string.Empty;
            m_TypeName = string.Empty;
            SetEnabled(true);

            focusable = false;

            name = string.Empty;
            yogaNode = new YogaNode();

            renderHints = RenderHints.None;
        }

        protected override void ExecuteDefaultAction(EventBase evt)
        {
            base.ExecuteDefaultAction(evt);
            if (evt == null)
            {
                return;
            }

            if (evt.eventTypeId == MouseOverEvent.TypeId() || evt.eventTypeId == MouseOutEvent.TypeId())
            {
                // Updating cursor has to happen on MouseOver/Out because exiting a children do not send a mouse enter to the parent.
                UpdateCursorStyle(evt.eventTypeId);
            }
            else if (evt.eventTypeId == MouseEnterEvent.TypeId())
            {
                pseudoStates |= PseudoStates.Hover;
            }
            else if (evt.eventTypeId == MouseLeaveEvent.TypeId())
            {
                pseudoStates &= ~PseudoStates.Hover;
            }
            else if (evt.eventTypeId == BlurEvent.TypeId())
            {
                pseudoStates = pseudoStates & ~PseudoStates.Focus;
            }
            else if (evt.eventTypeId == FocusEvent.TypeId())
            {
                pseudoStates = pseudoStates | PseudoStates.Focus;
            }
        }

        public sealed override void Focus()
        {
            if (!canGrabFocus && hierarchy.parent != null)
            {
                hierarchy.parent.Focus();
            }
            else
            {
                base.Focus();
            }
        }

        internal void SetPanel(BaseVisualElementPanel p)
        {
            if (panel == p)
                return;

            //We now gather all Elements in order to dispatch events in an efficient manner
            List<VisualElement> elements = VisualElementListPool.Get();
            try
            {
                elements.Add(this);
                GatherAllChildren(elements);

                EventDispatcherGate? pDispatcherGate = null;
                if (p?.dispatcher != null)
                {
                    pDispatcherGate = new EventDispatcherGate(p.dispatcher);
                }

                EventDispatcherGate? panelDispatcherGate = null;
                if (panel?.dispatcher != null && panel.dispatcher != p?.dispatcher)
                {
                    panelDispatcherGate = new EventDispatcherGate(panel.dispatcher);
                }

                using (pDispatcherGate)
                using (panelDispatcherGate)
                {
                    foreach (var e in elements)
                    {
                        e.ChangePanel(p);
                    }
                }
            }
            finally
            {
                VisualElementListPool.Release(elements);
            }
        }

        void ChangePanel(BaseVisualElementPanel p)
        {
            if (panel == p)
            {
                return;
            }

            if (panel != null)
            {
                using (var e = DetachFromPanelEvent.GetPooled(panel, p))
                {
                    e.target = this;
                    elementPanel.SendEvent(e, DispatchMode.Immediate);
                }
                UnregisterRunningAnimations();
            }

            IPanel prevPanel = panel;
            elementPanel = p;

            if (panel != null)
            {
                RegisterRunningAnimations();
                using (var e = AttachToPanelEvent.GetPooled(prevPanel, p))
                {
                    e.target = this;
                    elementPanel.SendEvent(e, DispatchMode.Default);
                }
            }

            // styles are dependent on topology
            IncrementVersion(VersionChangeType.StyleSheet | VersionChangeType.Layout | VersionChangeType.Transform);

            // persistent data key may have changed or needs initialization
            if (!string.IsNullOrEmpty(viewDataKey))
                IncrementVersion(VersionChangeType.ViewData);
        }

        public sealed override void SendEvent(EventBase e)
        {
            elementPanel?.SendEvent(e);
        }

        internal void IncrementVersion(VersionChangeType changeType)
        {
            elementPanel?.OnVersionChanged(this, changeType);
        }

        internal void InvokeHierarchyChanged(HierarchyChangeType changeType) { elementPanel?.InvokeHierarchyChanged(this, changeType); }

        //TODO: Make private once VisualContainer is merged with VisualElement
        protected internal bool SetEnabledFromHierarchy(bool state)
        {
            //returns false if state hasn't changed
            if (state == ((pseudoStates & PseudoStates.Disabled) != PseudoStates.Disabled))
                return false;

            if (state && enabledSelf && (parent == null || parent.enabledInHierarchy))
                pseudoStates &= ~PseudoStates.Disabled;
            else
                pseudoStates |= PseudoStates.Disabled;

            return true;
        }

        //Returns true if 'this' can be enabled relative to the enabled state of its panel
        public bool enabledInHierarchy
        {
            get { return (pseudoStates & PseudoStates.Disabled) != PseudoStates.Disabled; }
        }

        //Returns the local enabled state
        public bool enabledSelf { get; private set;}

        public void SetEnabled(bool value)
        {
            if (enabledSelf != value)
            {
                enabledSelf = value;

                if (value)
                    RemoveFromClassList(disabledUssClassName);
                else AddToClassList(disabledUssClassName);

                PropagateEnabledToChildren(value);
            }
        }

        void PropagateEnabledToChildren(bool value)
        {
            if (SetEnabledFromHierarchy(value))
            {
                var count = m_Children.Count;
                for (int i = 0; i < count; ++i)
                {
                    m_Children[i].PropagateEnabledToChildren(value);
                }
            }
        }

        public bool visible
        {
            get
            {
                return resolvedStyle.visibility == Visibility.Visible;
            }
            set
            {
                // Note: this could causes an allocation because styles are copy-on-write
                // we might want to remove this setter altogether
                // so everything goes through style.visibility (and then it's documented in a single place)
                style.visibility = value ? Visibility.Visible : Visibility.Hidden;
            }
        }

        public void MarkDirtyRepaint()
        {
            IncrementVersion(VersionChangeType.Repaint);
        }

        public Action<MeshGenerationContext> generateVisualContent { get; set; }

        internal void InvokeGenerateVisualContent(MeshGenerationContext mgc)
        {
            if (generateVisualContent != null)
            {
                try
                {
                    generateVisualContent(mgc);
                }
                catch (Exception e)
                {
                    Debug.LogException(e);
                }
            }
        }

        internal void GetFullHierarchicalViewDataKey(StringBuilder key)
        {
            const string keySeparator = "__";

            if (parent != null)
                parent.GetFullHierarchicalViewDataKey(key);

            if (!string.IsNullOrEmpty(viewDataKey))
            {
                key.Append(keySeparator);
                key.Append(viewDataKey);
            }
        }

        internal string GetFullHierarchicalViewDataKey()
        {
            StringBuilder key = new StringBuilder();

            GetFullHierarchicalViewDataKey(key);

            return key.ToString();
        }

        internal T GetOrCreateViewData<T>(object existing, string key) where T : class, new()
        {
            Debug.Assert(elementPanel != null, "VisualElement.elementPanel is null! Cannot load persistent data.");

            var viewData = elementPanel == null || elementPanel.getViewDataDictionary == null ? null : elementPanel.getViewDataDictionary();

            // If persistency is disable (no data, no key, no key one of the parents), just return the
            // existing data or create a local one if none exists.
            if (viewData == null || string.IsNullOrEmpty(viewDataKey) || enableViewDataPersistence == false)
            {
                if (existing != null)
                    return existing as T;

                return new T();
            }

            string keyWithType = key + "__" + typeof(T).ToString();

            if (!viewData.ContainsKey(keyWithType))
                viewData.Set(keyWithType, new T());

            return viewData.Get<T>(keyWithType);
        }

        internal T GetOrCreateViewData<T>(ScriptableObject existing, string key) where T : ScriptableObject
        {
            Debug.Assert(elementPanel != null, "VisualElement.elementPanel is null! Cannot load view data.");

            var viewData = elementPanel == null || elementPanel.getViewDataDictionary == null ? null : elementPanel.getViewDataDictionary();

            // If persistency is disable (no data, no key, no key one of the parents), just return the
            // existing data or create a local one if none exists.
            if (viewData == null || string.IsNullOrEmpty(viewDataKey) || enableViewDataPersistence == false)
            {
                if (existing != null)
                    return existing as T;

                return ScriptableObject.CreateInstance<T>();
            }

            string keyWithType = key + "__" + typeof(T).ToString();

            if (!viewData.ContainsKey(keyWithType))
                viewData.Set(keyWithType, ScriptableObject.CreateInstance<T>());

            return viewData.GetScriptable<T>(keyWithType);
        }

        internal void OverwriteFromViewData(object obj, string key)
        {
            if (obj == null)
            {
                throw new ArgumentNullException(nameof(obj));
            }

            Debug.Assert(elementPanel != null, "VisualElement.elementPanel is null! Cannot load view data.");

            var viewDataPersistentData = elementPanel == null || elementPanel.getViewDataDictionary == null ? null : elementPanel.getViewDataDictionary();

            // If persistency is disable (no data, no key, no key one of the parents), just return the
            // existing data or create a local one if none exists.
            if (viewDataPersistentData == null || string.IsNullOrEmpty(viewDataKey) || enableViewDataPersistence == false)
            {
                return;
            }

            string keyWithType = key + "__" + obj.GetType();

            if (!viewDataPersistentData.ContainsKey(keyWithType))
            {
                viewDataPersistentData.Set(keyWithType, obj);
                return;
            }

            viewDataPersistentData.Overwrite(obj, keyWithType);
        }

        internal void SaveViewData()
        {
            if (elementPanel != null
                && elementPanel.saveViewData != null
                && !string.IsNullOrEmpty(viewDataKey)
                && enableViewDataPersistence)
            {
                elementPanel.saveViewData();
            }
        }

        internal bool IsViewDataPersitenceSupportedOnChildren(bool existingState)
        {
            bool newState = existingState;

            // If this element has no key AND this element has a custom contentContainer,
            // turn off view data persistence. This essentially turns off persistence
            // on shadow elements if the parent has no key.
            if (string.IsNullOrEmpty(viewDataKey) && this != contentContainer)
                newState = false;

            // However, once we enter the light tree again, we need to turn persistence back on.
            if (parent != null && this == parent.contentContainer)
                newState = true;

            return newState;
        }

        internal void OnViewDataReady(bool enablePersistence)
        {
            this.enableViewDataPersistence = enablePersistence;
            OnViewDataReady();
        }

        internal virtual void OnViewDataReady() {}

        // position should be in local space
        // override to customize intersection between point and shape
        public virtual bool ContainsPoint(Vector2 localPoint)
        {
            return rect.Contains(localPoint);
        }

        public virtual bool Overlaps(Rect rectangle)
        {
            return rect.Overlaps(rectangle, true);
        }

        public enum MeasureMode
        {
            Undefined = YogaMeasureMode.Undefined,
            Exactly = YogaMeasureMode.Exactly,
            AtMost = YogaMeasureMode.AtMost
        }

        private bool m_RequireMeasureFunction = false;
        internal bool requireMeasureFunction
        {
            get { return m_RequireMeasureFunction; }
            set
            {
                m_RequireMeasureFunction = value;
                if (m_RequireMeasureFunction && !yogaNode.IsMeasureDefined)
                {
                    AssignMeasureFunction();
                }
                else if (!m_RequireMeasureFunction && yogaNode.IsMeasureDefined)
                {
                    RemoveMeasureFunction();
                }
            }
        }

        private void AssignMeasureFunction()
        {
            yogaNode.SetMeasureFunction((node, f, mode, f1, heightMode) => Measure(node, f, mode, f1, heightMode));
        }

        private void RemoveMeasureFunction()
        {
            yogaNode.SetMeasureFunction(null);
        }

        protected internal virtual Vector2 DoMeasure(float desiredWidth, MeasureMode widthMode, float desiredHeight, MeasureMode heightMode)
        {
            return new Vector2(float.NaN, float.NaN);
        }

        internal YogaSize Measure(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            Debug.Assert(node == yogaNode, "YogaNode instance mismatch");
            Vector2 size = DoMeasure(width, (MeasureMode)widthMode, height, (MeasureMode)heightMode);
            return MeasureOutput.Make(Mathf.RoundToInt(size.x), Mathf.RoundToInt(size.y));
        }

        internal void SetSize(Vector2 size)
        {
            var pos = layout;
            pos.width = size.x;
            pos.height = size.y;
            layout = pos;
        }

        void FinalizeLayout()
        {
            if (hasInlineStyle)
            {
                specifiedStyle.SyncWithLayout(yogaNode);
            }
            else
            {
                yogaNode.CopyStyle(specifiedStyle.yogaNode);
            }
        }

        // for internal use only, used by asset instantiation to push local styles
        // likely can be replaced by merging VisualContainer and VisualElement
        // and then storing the inline sheet in the list held by VisualContainer
        internal void SetInlineStyles(VisualElementStylesData inlineStyleData)
        {
            Debug.Assert(!inlineStyleData.isShared);
            inlineStyleData.Apply(m_Style, StylePropertyApplyMode.CopyIfEqualOrGreaterSpecificity);
            m_Style = inlineStyleData;
        }

        internal void SetSharedStyles(VisualElementStylesData sharedStyle)
        {
            Debug.Assert(sharedStyle.isShared);

            if (sharedStyle == m_SharedStyle)
            {
                return;
            }

            var previousOverflow = m_Style.overflow;
            var previousBorderBottomLeftRadius = m_Style.borderBottomLeftRadius;
            var previousBorderBottomRightRadius = m_Style.borderBottomRightRadius;
            var previousBorderTopLeftRadius = m_Style.borderTopLeftRadius;
            var previousBorderTopRightRadius = m_Style.borderTopRightRadius;
            var previousBorderLeftWidth = m_Style.borderLeftWidth;
            var previousBorderTopWidth = m_Style.borderTopWidth;
            var previousBorderRightWidth = m_Style.borderRightWidth;
            var previousBorderBottomWidth = m_Style.borderBottomWidth;
            var previousOpacity = m_Style.opacity;

            if (hasInlineStyle)
            {
                m_Style.Apply(sharedStyle, StylePropertyApplyMode.CopyIfNotInline);
            }
            else
            {
                m_Style = sharedStyle;
            }

            m_SharedStyle = sharedStyle;

            FinalizeLayout();

            VersionChangeType changes = VersionChangeType.Styles | VersionChangeType.Layout | VersionChangeType.Repaint;

            if (m_Style.overflow != previousOverflow)
                changes |= VersionChangeType.Overflow;

            if (previousBorderBottomLeftRadius != m_Style.borderBottomLeftRadius ||
                previousBorderBottomRightRadius != m_Style.borderBottomRightRadius ||
                previousBorderTopLeftRadius != m_Style.borderTopLeftRadius ||
                previousBorderTopRightRadius != m_Style.borderTopRightRadius)
            {
                changes |= VersionChangeType.BorderRadius;
            }

            if (previousBorderLeftWidth != m_Style.borderLeftWidth ||
                previousBorderTopWidth != m_Style.borderTopWidth ||
                previousBorderRightWidth != m_Style.borderRightWidth ||
                previousBorderBottomWidth != m_Style.borderBottomWidth)
            {
                changes |= VersionChangeType.BorderWidth;
            }

            if (m_Style.opacity != previousOpacity)
                changes |= VersionChangeType.Opacity;

            // This is a pre-emptive since we do not know if style changes actually cause a repaint or a layout
            // But those should be the only possible type of changes needed
            IncrementVersion(changes);
        }

        internal void ResetPositionProperties()
        {
            if (!hasInlineStyle)
            {
                return;
            }

            style.position = StyleKeyword.Null;
            style.marginLeft = StyleKeyword.Null;
            style.marginRight = StyleKeyword.Null;
            style.marginBottom = StyleKeyword.Null;
            style.marginTop = StyleKeyword.Null;
            style.left = StyleKeyword.Null;
            style.top = StyleKeyword.Null;
            style.right = StyleKeyword.Null;
            style.bottom = StyleKeyword.Null;
            style.width = StyleKeyword.Null;
            style.height = StyleKeyword.Null;

            FinalizeLayout();

            IncrementVersion(VersionChangeType.Layout);
        }

        public override string ToString()
        {
            return GetType().Name + " " + name + " " + layout + " world rect: " + worldBound;
        }

        public IEnumerable<string> GetClasses()
        {
            return m_ClassList;
        }

        public void ClearClassList()
        {
            if (m_ClassList.Count > 0)
            {
                m_ClassList = s_EmptyClassList;
                IncrementVersion(VersionChangeType.StyleSheet);
            }
        }

        public void AddToClassList(string className)
        {
            if (m_ClassList == s_EmptyClassList)
            {
                m_ClassList = new List<string>() { className };
            }
            else
            {
                if (m_ClassList.Contains(className))
                {
                    return;
                }

                // Avoid list size doubling when list is full.
                if (m_ClassList.Capacity == m_ClassList.Count)
                {
                    m_ClassList.Capacity += 1;
                }

                m_ClassList.Add(className);
            }

            IncrementVersion(VersionChangeType.StyleSheet);
        }

        public void RemoveFromClassList(string className)
        {
            if (m_ClassList.Remove(className))
            {
                IncrementVersion(VersionChangeType.StyleSheet);
            }
        }

        public void ToggleInClassList(string className)
        {
            if (ClassListContains(className))
                RemoveFromClassList(className);
            else
                AddToClassList(className);
        }

        public void EnableInClassList(string className, bool enable)
        {
            if (enable)
                AddToClassList(className);
            else
                RemoveFromClassList(className);
        }

        public bool ClassListContains(string cls)
        {
            for (int i = 0; i < m_ClassList.Count; i++)
            {
                if (m_ClassList[i] == cls)
                    return true;
            }

            return false;
        }

        public object FindAncestorUserData()
        {
            VisualElement p = parent;

            while (p != null)
            {
                if (p.userData != null)
                    return p.userData;
                p = p.parent;
            }

            return null;
        }

        internal object GetProperty(PropertyName key)
        {
            CheckUserKeyArgument(key);
            return GetPropertyInternal(key);
        }

        internal void SetProperty(PropertyName key, object value)
        {
            CheckUserKeyArgument(key);
            SetPropertyInternal(key, value);
        }

        object GetPropertyInternal(PropertyName key)
        {
            if (m_PropertyBag != null)
            {
                for (int i = 0; i < m_PropertyBag.Count; ++i)
                {
                    if (m_PropertyBag[i].Key == key)
                    {
                        return m_PropertyBag[i].Value;
                    }
                }
            }
            return null;
        }

        static void CheckUserKeyArgument(PropertyName key)
        {
            if (PropertyName.IsNullOrEmpty(key))
                throw new ArgumentNullException(nameof(key));

            if (key == userDataPropertyKey)
                throw new InvalidOperationException($"The {userDataPropertyKey} key is reserved by the system");
        }

        void SetPropertyInternal(PropertyName key, object value)
        {
            var kv = new PropertyBagValue(key, value);

            if (m_PropertyBag == null)
            {
                m_PropertyBag = new List<PropertyBagValue>(1);
                m_PropertyBag.Add(kv);
            }
            else
            {
                for (int i = 0; i < m_PropertyBag.Count; ++i)
                {
                    if (m_PropertyBag[i].Key == key)
                    {
                        m_PropertyBag[i] = kv;
                        return;
                    }
                }

                if (m_PropertyBag.Capacity == m_PropertyBag.Count)
                {
                    m_PropertyBag.Capacity += 1;
                }

                m_PropertyBag.Add(kv);
            }
        }

        private void UpdateCursorStyle(long eventType)
        {
            if (elementPanel != null)
            {
                if (eventType == MouseOverEvent.TypeId() && elementPanel.GetTopElementUnderPointer(PointerId.mousePointerId) == this)
                {
                    elementPanel.cursorManager.SetCursor(computedStyle.cursor.value);
                }
                else if (eventType == MouseOutEvent.TypeId())
                {
                    elementPanel.cursorManager.ResetCursor();
                }
            }
        }
    }

    public static class VisualElementExtensions
    {
        // transforms a point assumed in Panel space to the referential inside of the element bound (local)
        public static Vector2 WorldToLocal(this VisualElement ele, Vector2 p)
        {
            if (ele == null)
            {
                throw new ArgumentNullException(nameof(ele));
            }

            return VisualElement.MultiplyMatrix44Point2(ele.worldTransformInverse, p);
        }

        // transforms a point to Panel space referential
        public static Vector2 LocalToWorld(this VisualElement ele, Vector2 p)
        {
            if (ele == null)
            {
                throw new ArgumentNullException(nameof(ele));
            }

            return VisualElement.MultiplyMatrix44Point2(ele.worldTransform, p);
        }

        // transforms a rect assumed in Panel space to the referential inside of the element bound (local)
        public static Rect WorldToLocal(this VisualElement ele, Rect r)
        {
            if (ele == null)
            {
                throw new ArgumentNullException(nameof(ele));
            }

            Vector2 position = VisualElement.MultiplyMatrix44Point2(ele.worldTransformInverse, r.position);
            r.position = position;
            r.size = ele.worldTransformInverse.MultiplyVector(r.size);
            return r;
        }

        // transforms a rect to Panel space referential
        public static Rect LocalToWorld(this VisualElement ele, Rect r)
        {
            if (ele == null)
            {
                throw new ArgumentNullException(nameof(ele));
            }

            var toWorldMatrix = ele.worldTransform;
            r.position = VisualElement.MultiplyMatrix44Point2(toWorldMatrix, r.position);
            r.size = toWorldMatrix.MultiplyVector(r.size);
            return r;
        }

        // transform point from the local space of one element to to the local space of another
        public static Vector2 ChangeCoordinatesTo(this VisualElement src, VisualElement dest, Vector2 point)
        {
            return dest.WorldToLocal(src.LocalToWorld(point));
        }

        // transform Rect from the local space of one element to to the local space of another
        public static Rect ChangeCoordinatesTo(this VisualElement src, VisualElement dest, Rect rect)
        {
            return dest.WorldToLocal(src.LocalToWorld(rect));
        }

        public static void StretchToParentSize(this VisualElement elem)
        {
            if (elem == null)
            {
                throw new ArgumentNullException(nameof(elem));
            }

            IStyle styleAccess = elem.style;
            styleAccess.position = Position.Absolute;
            styleAccess.left = 0.0f;
            styleAccess.top = 0.0f;
            styleAccess.right = 0.0f;
            styleAccess.bottom = 0.0f;
        }

        public static void StretchToParentWidth(this VisualElement elem)
        {
            if (elem == null)
            {
                throw new ArgumentNullException(nameof(elem));
            }

            IStyle styleAccess = elem.style;
            styleAccess.position = Position.Absolute;
            styleAccess.left = 0.0f;
            styleAccess.right = 0.0f;
        }

        public static void AddManipulator(this VisualElement ele, IManipulator manipulator)
        {
            if (manipulator != null)
            {
                manipulator.target = ele;
            }
        }

        public static void RemoveManipulator(this VisualElement ele, IManipulator manipulator)
        {
            if (manipulator != null)
            {
                manipulator.target = null;
            }
        }
    }

    internal static class VisualElementDebugExtensions
    {
        public static string GetDisplayName(this VisualElement ve, bool withHashCode = true)
        {
            if (ve == null) return String.Empty;

            string objectName = ve.GetType().Name;
            if (!String.IsNullOrEmpty(ve.name))
            {
                objectName += "#" + ve.name;
            }

            if (withHashCode)
            {
                objectName += " (" + ve.GetHashCode().ToString("x8") + ")";
            }

            return objectName;
        }
    }
}
