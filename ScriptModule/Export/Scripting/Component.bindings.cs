using UnityEngine.Internal;
using UnityEngineInternal;
using UnityEngine.Bindings;
using UnityEngine.Scripting;

using System;
using System.Collections.Generic;

namespace UnityEngine
{
    public partial class Component : UnityEngine.Object
    {
        public extern Transform transform
        {
            [FreeFunction("GetTransform", HasExplicitThis = true, ThrowsException = true)]
            get;
        }

        public extern GameObject gameObject
        {
            [FreeFunction("GetGameObject", HasExplicitThis = true)]
            get;
        }
    }
}
