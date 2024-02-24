using System;
using System.Runtime.InteropServices;
using UnityEngine.Bindings;
using UnityEngine.Scripting;
using UnityEngineInternal;
using uei = UnityEngine.Internal;

namespace UnityEngine
{
    // Bit mask that controls object destruction and visibility in inspectors
    [Flags]
    public enum HideFlags
    {
        // A normal, visible object. This is the default.
        None = 0,

        // The object will not appear in the hierarchy and will not show up in the project view if it is stored in an asset.
        HideInHierarchy = 1,

        // It is not possible to view it in the inspector
        HideInInspector = 2,

        // The object will not be saved to the scene.
        DontSaveInEditor = 4,

        // The object is not be editable in the inspector
        NotEditable = 8,

        // The object will not be saved when building a player
        DontSaveInBuild = 16,

        // The object will not be unloaded by UnloadUnusedAssets
        DontUnloadUnusedAsset = 32,

        DontSave = 4 + 16 + 32,

        // A combination of not shown in the hierarchy and not saved to to scenes.
        HideAndDontSave = 1 + 4 + 8 + 16 + 32
    }

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

        public static void DontDestroyOnLoad(Object target)
        {

        }

        public static Object FindObjectOfType(System.Type type)
        {
            return null;
        }

        public static T FindObjectOfType<T>() where T : Object
        {
            return (T)FindObjectOfType(typeof(T));
        }
    }
}
