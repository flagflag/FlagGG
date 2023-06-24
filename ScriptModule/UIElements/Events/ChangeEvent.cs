namespace UnityEngine.UIElements
{
    public interface IChangeEvent
    {
    }

    public class ChangeEvent<T> : EventBase<ChangeEvent<T>>, IChangeEvent
    {
        public T previousValue { get; protected set; }
        public T newValue { get; protected set; }

        protected override void Init()
        {
            base.Init();
            LocalInit();
        }

        void LocalInit()
        {
            propagation = EventPropagation.Bubbles | EventPropagation.TricklesDown;
            previousValue = default(T);
            newValue = default(T);
        }

        public static ChangeEvent<T> GetPooled(T previousValue, T newValue)
        {
            ChangeEvent<T> e = GetPooled();
            e.previousValue = previousValue;
            e.newValue = newValue;
            return e;
        }

        public ChangeEvent()
        {
            LocalInit();
        }
    }
}
