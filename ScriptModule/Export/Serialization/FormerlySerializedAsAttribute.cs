using System;
using UnityEngine.Scripting;

namespace UnityEngine.Serialization
{
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = true, Inherited = false)]
    [RequiredByNativeCode]
    public class FormerlySerializedAsAttribute : Attribute
    {
        private string m_oldName;
        public FormerlySerializedAsAttribute(string oldName)
        {
            m_oldName = oldName;
        }

        public string oldName { get { return m_oldName; } }
    }
}
