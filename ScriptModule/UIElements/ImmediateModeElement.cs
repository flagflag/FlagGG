using System;

namespace UnityEngine.UIElements
{
    public abstract class ImmediateModeElement : VisualElement
    {
        public ImmediateModeElement()
        {
            generateVisualContent += OnGenerateVisualContent;
        }

        private void OnGenerateVisualContent(MeshGenerationContext mgc)
        {
            mgc.painter.DrawImmediate(ImmediateRepaint);
        }

        protected abstract void ImmediateRepaint();
    }

    // Used to wrap the exception thrown by the immediate callback
    class ImmediateModeException : Exception
    {
        public ImmediateModeException(Exception inner)
            : base("", inner)
        {
        }
    }
}
