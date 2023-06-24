using System;
using System.Collections.Generic;

namespace UnityEngine.UIElements
{
    public abstract class Focusable : CallbackEventHandler
    {
        protected Focusable()
        {
            focusable = true;
            tabIndex = 0;
        }

        public abstract FocusController focusController { get; }

        public bool focusable { get; set; }

        // See http://w3c.github.io/html/editing.html#the-tabindex-attribute
        public int tabIndex { get; set; }

        bool m_DelegatesFocus;
        public bool delegatesFocus
        {
            get { return m_DelegatesFocus; }
            set
            {
                if (!((VisualElement)this).isCompositeRoot)
                {
                    throw new InvalidOperationException("delegatesFocus should only be set on composite roots.");
                }
                m_DelegatesFocus = value;
            }
        }

        // Used when we want then children of a composite to appear at
        // composite root tabIndex position in the focus ring, but
        // we do not want the root itself to be part of the ring.
        bool m_ExcludeFromFocusRing;
        internal bool excludeFromFocusRing
        {
            get { return m_ExcludeFromFocusRing; }
            set
            {
                if (!((VisualElement)this).isCompositeRoot)
                {
                    throw new InvalidOperationException("excludeFromFocusRing should only be set on composite roots.");
                }
                m_ExcludeFromFocusRing = value;
            }
        }

        // IMGUIContainers are special snowflakes that need custom treatment regarding events.
        // This enables early outs in some dispatching strategies.
        internal bool isIMGUIContainer = false;

        public virtual bool canGrabFocus => focusable;

        public virtual void Focus()
        {
            if (focusController != null)
            {
                if (canGrabFocus)
                {
                    var elementGettingFocused = GetFocusDelegate();
                    focusController.SwitchFocus(elementGettingFocused);
                }
                else
                {
                    focusController.SwitchFocus(null);
                }
            }
        }

        public virtual void Blur()
        {
            if (focusController != null)
            {
                if (focusController.IsFocused(this))
                {
                    focusController.SwitchFocus(null);
                }
            }
        }

        // Use the tree to find the first focusable child.
        // FIXME: we should use the focus ring; however, it may happens that the
        // children are focusable but not part of the ring.
        Focusable GetFocusDelegate()
        {
            var f = this;

            while (f != null && f.delegatesFocus)
            {
                f = GetFirstFocusableChild(f as VisualElement);
            }

            return f;
        }

        static Focusable GetFirstFocusableChild(VisualElement ve)
        {
            foreach (var child in ve.hierarchy.Children())
            {
                if (child.canGrabFocus)
                {
                    return child;
                }

                bool isSlot = child.hierarchy.parent != null && child == child.hierarchy.parent.contentContainer;
                if (!child.isCompositeRoot && !isSlot)
                {
                    var f = GetFirstFocusableChild(child);
                    if (f != null)
                    {
                        return f;
                    }
                }
            }

            return null;
        }

        protected override void ExecuteDefaultAction(EventBase evt)
        {
            base.ExecuteDefaultAction(evt);

            if (evt != null && evt.target == evt.leafTarget)
            {
                if (evt.eventTypeId == MouseDownEvent.TypeId())
                {
                    Focus();

                    // If we click on a focusable element, that means the event was handled and we should not send it to the rootIMGUIContainer
                    if (this.canGrabFocus)
                        evt.doNotSendToRootIMGUIContainer = true;
                }

                focusController?.SwitchFocusOnEvent(evt);
            }
        }
    }

    public class FocusChangeDirection
    {
        public static FocusChangeDirection unspecified { get; } = new FocusChangeDirection(-1);

        public static FocusChangeDirection none { get; } = new FocusChangeDirection(0);

        protected static FocusChangeDirection lastValue { get; } = none;

        readonly int m_Value;

        protected FocusChangeDirection(int value)
        {
            m_Value = value;
        }

        public static implicit operator int(FocusChangeDirection fcd)
        {
            return fcd?.m_Value ?? 0;
        }
    }

    public interface IFocusRing
    {
        FocusChangeDirection GetFocusChangeDirection(Focusable currentFocusable, EventBase e);

        Focusable GetNextFocusable(Focusable currentFocusable, FocusChangeDirection direction);
    }

    public class FocusController
    {
        // https://w3c.github.io/uievents/#interface-focusevent

        public FocusController(IFocusRing focusRing)
        {
            this.focusRing = focusRing;
            imguiKeyboardControl = 0;
        }

        IFocusRing focusRing { get; }

        struct FocusedElement
        {
            public VisualElement m_SubTreeRoot;
            public Focusable m_FocusedElement;
        }

        List<FocusedElement> m_FocusedElements = new List<FocusedElement>();

        public Focusable focusedElement => GetRetargetedFocusedElement(null);

        internal bool IsFocused(Focusable f)
        {
            foreach (var fe in m_FocusedElements)
            {
                if (fe.m_FocusedElement == f)
                {
                    return true;
                }
            }

            return false;
        }

        internal Focusable GetRetargetedFocusedElement(VisualElement retargetAgainst)
        {
            var retargetRoot = retargetAgainst?.hierarchy.parent;
            if (retargetRoot == null)
            {
                if (m_FocusedElements.Count > 0)
                {
                    return m_FocusedElements[m_FocusedElements.Count - 1].m_FocusedElement;
                }
            }
            else
            {
                while (!retargetRoot.isCompositeRoot && retargetRoot.hierarchy.parent != null)
                {
                    retargetRoot = retargetRoot.hierarchy.parent;
                }

                foreach (var fe in m_FocusedElements)
                {
                    if (fe.m_SubTreeRoot == retargetRoot)
                    {
                        return fe.m_FocusedElement;
                    }
                }
            }

            return null;
        }

        internal Focusable GetLeafFocusedElement()
        {
            if (m_FocusedElements.Count > 0)
            {
                return m_FocusedElements[0].m_FocusedElement;
            }
            return null;
        }

        internal void DoFocusChange(Focusable f)
        {
            m_FocusedElements.Clear();

            VisualElement ve = f as VisualElement;
            while (ve != null)
            {
                if (ve.hierarchy.parent == null || ve.isCompositeRoot)
                {
                    m_FocusedElements.Add(new FocusedElement { m_SubTreeRoot = ve, m_FocusedElement = f });
                    f = ve;
                }
                ve = ve.hierarchy.parent;
            }
        }

        void AboutToReleaseFocus(Focusable focusable, Focusable willGiveFocusTo, FocusChangeDirection direction)
        {
            using (FocusOutEvent e = FocusOutEvent.GetPooled(focusable, willGiveFocusTo, direction, this))
            {
                focusable.SendEvent(e);
            }
        }

        void ReleaseFocus(Focusable focusable, Focusable willGiveFocusTo, FocusChangeDirection direction)
        {
            using (BlurEvent e = BlurEvent.GetPooled(focusable, willGiveFocusTo, direction, this))
            {
                focusable.SendEvent(e);
            }
        }

        void AboutToGrabFocus(Focusable focusable, Focusable willTakeFocusFrom, FocusChangeDirection direction)
        {
            using (FocusInEvent e = FocusInEvent.GetPooled(focusable, willTakeFocusFrom, direction, this))
            {
                focusable.SendEvent(e);
            }
        }

        void GrabFocus(Focusable focusable, Focusable willTakeFocusFrom, FocusChangeDirection direction)
        {
            using (FocusEvent e = FocusEvent.GetPooled(focusable, willTakeFocusFrom, direction, this))
            {
                focusable.SendEvent(e);
            }
        }

        internal void SwitchFocus(Focusable newFocusedElement)
        {
            SwitchFocus(newFocusedElement, FocusChangeDirection.unspecified);
        }

        void SwitchFocus(Focusable newFocusedElement, FocusChangeDirection direction)
        {
            if (GetLeafFocusedElement() == newFocusedElement)
            {
                return;
            }

            var oldFocusedElement = GetLeafFocusedElement();

            if (newFocusedElement == null || !newFocusedElement.canGrabFocus)
            {
                if (oldFocusedElement != null)
                {
                    AboutToReleaseFocus(oldFocusedElement, null, direction);
                    ReleaseFocus(oldFocusedElement, null, direction);
                }
            }
            else if (newFocusedElement != oldFocusedElement)
            {
                // Retarget event.relatedTarget so it is in the same tree as event.target.
                var retargetedNewFocusedElement = (newFocusedElement as VisualElement)?.RetargetElement(oldFocusedElement as VisualElement);
                var retargetedOldFocusedElement = (oldFocusedElement as VisualElement)?.RetargetElement(newFocusedElement as VisualElement);

                if (oldFocusedElement != null)
                {
                    AboutToReleaseFocus(oldFocusedElement, retargetedNewFocusedElement, direction);
                }

                AboutToGrabFocus(newFocusedElement, retargetedOldFocusedElement, direction);

                if (oldFocusedElement != null)
                {
                    ReleaseFocus(oldFocusedElement, retargetedNewFocusedElement, direction);
                }

                GrabFocus(newFocusedElement, retargetedOldFocusedElement, direction);
            }
        }

        internal Focusable SwitchFocusOnEvent(EventBase e)
        {
            FocusChangeDirection direction = focusRing.GetFocusChangeDirection(GetLeafFocusedElement(), e);
            if (direction != FocusChangeDirection.none)
            {
                Focusable f = focusRing.GetNextFocusable(GetLeafFocusedElement(), direction);
                SwitchFocus(f, direction);
                // f does not have the focus yet. It will when the series of focus events will have been handled.
                return f;
            }

            return GetLeafFocusedElement();
        }

        /// <summary>
        /// This property contains the actual keyboard id of the element being focused in the case of an IMGUIContainer
        /// </summary>
        internal int imguiKeyboardControl { get; set; }

        internal void SyncIMGUIFocus(int imguiKeyboardControlID, Focusable imguiContainerHavingKeyboardControl, bool forceSwitch)
        {
            imguiKeyboardControl = imguiKeyboardControlID;

            if (forceSwitch || imguiKeyboardControl != 0)
            {
                SwitchFocus(imguiContainerHavingKeyboardControl, FocusChangeDirection.unspecified);
            }
            else
            {
                SwitchFocus(null, FocusChangeDirection.unspecified);
            }
        }
    }
}
