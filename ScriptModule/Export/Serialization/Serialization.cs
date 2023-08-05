using scm = System.ComponentModel;
using uei = UnityEngine.Internal;
using RequiredByNativeCodeAttribute = UnityEngine.Scripting.RequiredByNativeCodeAttribute;
using UsedByNativeCodeAttribute = UnityEngine.Scripting.UsedByNativeCodeAttribute;

using System;
namespace UnityEngine
{
    [System.Obsolete("Use SerializeField on the private variables that you want to be serialized instead")]
    [RequiredByNativeCode]
    public sealed partial class SerializePrivateVariables : Attribute
    {
    }

    [RequiredByNativeCode]
    public sealed partial class SerializeField : Attribute
    {
    }

    /// <summary>
    /// What is this : Instruct the Unity serialization backend to serialize field as a reference type, as opposed to SerializeField
    ///                 that serializes the data in place (as if it where a Value type).
    /// Motivation(s):
    ///  - Polymorphic serialization of plain old C# classes.
    ///  - Representing graph like structures without having to resort to ScriptableObjects.
    ///  - Express null fields.
    /// </summary>
    [RequiredByNativeCode]
    [AttributeUsage(AttributeTargets.Field)]
    public sealed partial class SerializeReference : Attribute
    {
        [UnityEngine.Internal.ExcludeFromDocs]
        public SerializeReference() {}
    }

    [RequiredByNativeCode]
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class PreferBinarySerialization : Attribute
    {
    }

    [RequiredByNativeCode]
    public interface ISerializationCallbackReceiver
    {
        [RequiredByNativeCode]
        void OnBeforeSerialize();

        [RequiredByNativeCode]
        void OnAfterDeserialize();
    }
}
