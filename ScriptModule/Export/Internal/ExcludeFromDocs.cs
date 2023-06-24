using System;

namespace UnityEngine.Internal
{
    /// <summary>
    /// Adds default value information for optional parameters
    /// </summary>
    [Serializable]
    public class ExcludeFromDocsAttribute : Attribute
    {
        public ExcludeFromDocsAttribute()
        {
        }
    }
}
