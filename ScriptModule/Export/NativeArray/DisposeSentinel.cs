using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
using UnityEngine;

namespace Unity.Collections
{
    public enum NativeLeakDetectionMode
    {
        EnabledWithStackTrace = 3,
        Enabled = 2,
        Disabled = 1
    }

    public static class NativeLeakDetection
    {
        // For performance reasons no assignment operator (static initializer cost in il2cpp)
        // and flipped enabled / disabled enum value
        static int s_NativeLeakDetectionMode;
        const string kNativeLeakDetectionModePrefsString = "Unity.Colletions.NativeLeakDetection.Mode";

        // Initialize leak detection on startup/domain reload to avoid NativeLeakDetection.Mode
        // access on a job to trigger the initialization.
        [RuntimeInitializeOnLoadMethod]
        static void Initialize()
        {
            #if UNITY_EDITOR
            s_NativeLeakDetectionMode = UnityEngine.PlayerPrefs.EditorPrefsGetInt(kNativeLeakDetectionModePrefsString, (int)NativeLeakDetectionMode.Enabled);
            if (s_NativeLeakDetectionMode < (int)NativeLeakDetectionMode.Disabled || s_NativeLeakDetectionMode > (int)NativeLeakDetectionMode.EnabledWithStackTrace)
                s_NativeLeakDetectionMode = (int)NativeLeakDetectionMode.Enabled;
            #else
            s_NativeLeakDetectionMode = (int)NativeLeakDetectionMode.Disabled;
            #endif
        }

        public static NativeLeakDetectionMode Mode
        {
            get
            {
                if (s_NativeLeakDetectionMode == 0)
                    Initialize();
                return (NativeLeakDetectionMode)s_NativeLeakDetectionMode;
            }
            set
            {
                var intValue = (int)value;
                if (s_NativeLeakDetectionMode != intValue)
                {
                    s_NativeLeakDetectionMode = intValue;
                    #if UNITY_EDITOR
                    UnityEngine.PlayerPrefs.EditorPrefsSetInt(kNativeLeakDetectionModePrefsString, intValue);
                    #endif
                }
            }
        }
    }
}


#if ENABLE_UNITY_COLLECTIONS_CHECKS
namespace Unity.Collections.LowLevel.Unsafe
{
    [StructLayout(LayoutKind.Sequential)]
    public sealed class DisposeSentinel
    {
        int                m_IsCreated;
        StackTrace         m_StackTrace;

        private DisposeSentinel()
        {
        }

        public static void Dispose(ref AtomicSafetyHandle safety, ref DisposeSentinel sentinel)
        {
            AtomicSafetyHandle.CheckDeallocateAndThrow(safety);
            // If the safety handle is for a temp allocation, create a new safety handle for this instance which can be marked as invalid
            // Setting it to new AtomicSafetyHandle is not enough since the handle needs a valid node pointer in order to give the correct errors
            if (AtomicSafetyHandle.IsTempMemoryHandle(safety))
                safety = AtomicSafetyHandle.Create();
            AtomicSafetyHandle.Release(safety);
            Clear(ref sentinel);
        }

        public static void Create(out AtomicSafetyHandle safety, out DisposeSentinel sentinel, int callSiteStackDepth, Allocator allocator)
        {
            safety = (allocator == Allocator.Temp) ? AtomicSafetyHandle.GetTempMemoryHandle() : AtomicSafetyHandle.Create();
            sentinel = null;
            if (allocator == Allocator.Temp || allocator == Allocator.AudioKernel)
                return;

            if (Unity.Jobs.LowLevel.Unsafe.JobsUtility.IsExecutingJob)
                throw new InvalidOperationException("Jobs can only create Temp memory");

            CreateInternal(ref sentinel, callSiteStackDepth);
        }

        [Unity.Burst.BurstDiscard]
        private static void CreateInternal(ref DisposeSentinel sentinel, int callSiteStackDepth)
        {
            var mode = NativeLeakDetection.Mode;
            if (mode == NativeLeakDetectionMode.Disabled)
                return;

            StackTrace stackTrace = null;
            if (mode == NativeLeakDetectionMode.EnabledWithStackTrace)
                stackTrace = new StackTrace(callSiteStackDepth + 2, true);

            sentinel = new DisposeSentinel
            {
                m_StackTrace = stackTrace,
                m_IsCreated = 1
            };
        }

        ~DisposeSentinel()
        {
            if (m_IsCreated != 0)
            {
                var fileName = "";
                var lineNb = 0;

                if (m_StackTrace != null)
                {
                    var stackTrace = UnityEngine.StackTraceUtility.ExtractFormattedStackTrace(m_StackTrace);
                    var err = "A Native Collection has not been disposed, resulting in a memory leak. Allocated from:\n" + stackTrace;

                    if (m_StackTrace.FrameCount != 0)
                    {
                        fileName = m_StackTrace.GetFrame(0).GetFileName();
                        lineNb = m_StackTrace.GetFrame(0).GetFileLineNumber();
                    }

                    UnsafeUtility.LogError(err, fileName, lineNb);
                }
                else
                {
                    var err = "A Native Collection has not been disposed, resulting in a memory leak. Enable Full StackTraces to get more details.";
                    UnsafeUtility.LogError(err, fileName, lineNb);
                }
            }
        }

        [Unity.Burst.BurstDiscard]
        public static void Clear(ref DisposeSentinel sentinel)
        {
            if (sentinel != null)
            {
                sentinel.m_IsCreated = 0;
                sentinel = null;
            }
        }
    }
}
#endif // ENABLE_UNITY_COLLECTIONS_CHECKS
