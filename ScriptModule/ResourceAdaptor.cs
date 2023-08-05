using System;

namespace UnityEngine
{
    public class Resources : ScriptableObject
    {
        static public T Load<T>(params object[] list) where T : ScriptableObject
        {
            return null;
        }

        static public Object Load(string path, Type type)
        {
            return null;
        }
    }
}
