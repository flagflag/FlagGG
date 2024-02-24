using System;
using System.Xml.Linq;

namespace UnityEngine
{
    public class AssetBundle
    {
        private Object LoadAsset_Internal(string name, Type type)
        {
            return null;
        }

        public Object LoadAsset(string name, Type type)
        {
            if (name == null)
            {
                throw new System.NullReferenceException("The input asset name cannot be null.");
            }
            if (name.Length == 0)
            {
                throw new System.ArgumentException("The input asset name cannot be empty.");
            }
            if (type == null)
            {
                throw new System.NullReferenceException("The input type cannot be null.");
            }

            return LoadAsset_Internal(name, type);
        }

        public T LoadAsset<T>(string name) where T : Object
        {
            return (T)LoadAsset(name, typeof(T));
        }

        public void Unload(bool unloadAllLoadedObjects)
        {

        }
    }
}
