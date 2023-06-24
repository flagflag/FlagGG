using System;

namespace Unity.Burst
{
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Property)]
    public class BurstDiscardAttribute : Attribute
    {
        // Attribute used to discard entirely a method/property from being compiled by the burst compiler.
    }
}
