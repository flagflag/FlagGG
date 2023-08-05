using System;
using UnityEngine.Bindings;

namespace UnityEngine.Internal
{
    /// <summary>
    /// Tags that it is not possible (or undefined) to duplicate/instantiate an asset of this type.
    /// </summary>
    [VisibleToOtherModules]
    interface ISubAssetNotDuplicatable
    {
    }
}
