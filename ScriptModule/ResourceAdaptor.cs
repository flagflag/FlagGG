using System;

namespace UnityEngine
{
    public class Resources : ScriptableObject
    {
        public static T Load<T>(params object[] list) where T : ScriptableObject
        {
            return null;
        }

        public static Object Load(string path, Type type)
        {
            return null;
        }

        public static void UnloadAsset(Object assetToUnload)
        {

        }
    }
}
