using System;
using System.Runtime.InteropServices;
using UnityEngine.Scripting;
using UnityEngine.Bindings;
using System.Globalization;

namespace UnityEngine
{
    // This function exists because UnityEngine.dll is compiled against .NET 3.5, but .NET Core removes all the overloads
    // except this one. So to prevent our code compiling against the (string, object, object) version and use the params
    // version instead, we reroute through this.
    // TODO: remove this when the dependency goes away.
    [VisibleToOtherModules]
    internal sealed partial class UnityString
    {
        public static string Format(string fmt, params object[] args)
        {
            return String.Format(CultureInfo.InvariantCulture.NumberFormat, fmt, args);
        }
    }
}
