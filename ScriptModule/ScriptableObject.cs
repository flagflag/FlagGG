
namespace UnityEngine
{
    public class Object
    {
        public string name;

        public HideFlags hideFlags;

        public static implicit operator bool(Object exists)
        {
            return exists != null;
        }

        public static void Destroy(Object obj)
        {
            
        }

        public static void DestroyImmediate(Object obj)
        {

        }
    }

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
