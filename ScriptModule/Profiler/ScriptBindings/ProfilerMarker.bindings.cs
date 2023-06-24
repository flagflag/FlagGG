using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Unity.Collections.LowLevel.Unsafe;
using Unity.Profiling.LowLevel;
using UnityEngine.Bindings;
using UnityEngine.Scripting;
using Object = UnityEngine.Object;

namespace Unity.Profiling
{
    [UsedByNativeCode]
    [StructLayout(LayoutKind.Sequential)]
    [NativeHeader("Runtime/Profiler/ScriptBindings/ProfilerMarker.bindings.h")]
    public struct ProfilerMarker
    {
        [NativeDisableUnsafePtrRestriction]
        internal readonly IntPtr m_Ptr;

        // 256 : Aggressive inlining
        [MethodImpl(256)]
        public ProfilerMarker(string name)
        {
            m_Ptr = Internal_Create(name, (ushort)MarkerFlags.Default);
        }

        [MethodImpl(256)]
        [Conditional("ENABLE_PROFILER")]
        public void Begin()
        {
            Internal_Begin(m_Ptr);
        }

        [MethodImpl(256)]
        [Conditional("ENABLE_PROFILER")]
        public void Begin(Object contextUnityObject)
        {
            Internal_BeginWithObject(m_Ptr, contextUnityObject);
        }

        [MethodImpl(256)]
        [Conditional("ENABLE_PROFILER")]
        public void End()
        {
            Internal_End(m_Ptr);
        }

        [Conditional("ENABLE_PROFILER")]
        internal void GetName(ref string name)
        {
            name = Internal_GetName(m_Ptr);
        }

        [UsedByNativeCode]
        public struct AutoScope : IDisposable
        {
            [NativeDisableUnsafePtrRestriction]
            internal readonly IntPtr m_Ptr;

            [MethodImpl(256)]
            internal AutoScope(IntPtr markerPtr)
            {
                m_Ptr = markerPtr;
                Internal_Begin(markerPtr);
            }

            [MethodImpl(256)]
            public void Dispose()
            {
                Internal_End(m_Ptr);
            }
        }

        [MethodImpl(256)]
        public AutoScope Auto()
        {
            return new AutoScope(m_Ptr);
        }

        [ThreadSafe]
        [NativeConditional("ENABLE_PROFILER")]
        internal static extern IntPtr Internal_Create(string name, ushort flags);

        [ThreadSafe]
        [NativeConditional("ENABLE_PROFILER")]
        internal static extern void Internal_Begin(IntPtr markerPtr);

        [ThreadSafe]
        [NativeConditional("ENABLE_PROFILER")]
        internal static extern void Internal_BeginWithObject(IntPtr markerPtr, UnityEngine.Object contextUnityObject);

        [ThreadSafe]
        [NativeConditional("ENABLE_PROFILER")]
        internal static extern void Internal_End(IntPtr markerPtr);

        [ThreadSafe]
        [NativeConditional("ENABLE_PROFILER")]
        internal static extern unsafe void Internal_Emit(IntPtr markerPtr, ushort eventType, int metadataCount, void* metadata);

        [ThreadSafe]
        [NativeConditional("ENABLE_PROFILER")]
        static extern string Internal_GetName(IntPtr markerPtr);
    }
}
