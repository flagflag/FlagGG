using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using UnityEngineInternal;
// using UnityEngine.SceneManagement;
using UnityEngine.Bindings;
using UnityEngine.Scripting;
using uei = UnityEngine.Internal;
using System.ComponentModel;


namespace UnityEngine
{
    public sealed class GameObject : Object
    {
        public extern Transform transform
        {
            [FreeFunction("GameObjectBindings::GetTransform", HasExplicitThis = true)]
            get;
        }

        public GameObject(string name)
        {
        }

        public GameObject()
        {
        }

        public GameObject(string name, params Type[] components)
        {
        }

        public T GetComponent<T>() where T : Component
        {
            return null;
        }

        public Component AddComponent(Type componentType)
        {
            return null;
        }

        public T AddComponent<T>() where T : Component
        {
            return AddComponent(typeof(T)) as T;
        }

        public void SetActive(bool value)
        {

        }

        public int layer { get; set; }
    }
}
