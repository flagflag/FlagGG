
namespace UnityEngine
{
    public abstract class ScriptableObject : Object
    {
        public static T CreateInstance<T>() where T : ScriptableObject
        {
            return null;
        }

        public int GetInstanceID()
        {
            return 0;
        }
    }
}
