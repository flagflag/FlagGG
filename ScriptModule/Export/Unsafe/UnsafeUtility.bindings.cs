using System;
using System.Reflection;
using System.Runtime.InteropServices;
using UnityEngine.Bindings;
using System.Runtime.CompilerServices;

namespace Unity.Collections.LowLevel.Unsafe
{
    [NativeHeader("Runtime/Export/Unsafe/UnsafeUtility.bindings.h")]
    [StaticAccessor("UnsafeUtility", StaticAccessorType.DoubleColon)]
    public static partial class UnsafeUtility
    {
        [ThreadSafe]
        extern static int GetFieldOffsetInStruct(FieldInfo field);

        [ThreadSafe]
        extern static int GetFieldOffsetInClass(FieldInfo field);

        public static int GetFieldOffset(FieldInfo field)
        {
#if ENABLE_UNITY_COLLECTIONS_CHECKS
            if (field == null)
                throw new ArgumentNullException(nameof(field));
#endif
            if (field.DeclaringType.IsValueType)
                return GetFieldOffsetInStruct(field);
            else if (field.DeclaringType.IsClass)
                return GetFieldOffsetInClass(field);
            else
            {
#if ENABLE_UNITY_COLLECTIONS_CHECKS
                throw new ArgumentException("field.DeclaringType must be a struct or class");
#else
                return -1;
#endif
            }
        }

        unsafe public static void* PinGCObjectAndGetAddress(System.Object target, out ulong gcHandle)
        {
#if ENABLE_UNITY_COLLECTIONS_CHECKS
            if (target == null)
                throw new ArgumentNullException(nameof(target));
#endif

            return PinSystemObjectAndGetAddress(target, out gcHandle);
        }

        unsafe public static void* PinGCArrayAndGetDataAddress(System.Array target, out ulong gcHandle)
        {
#if ENABLE_UNITY_COLLECTIONS_CHECKS
            if (target == null)
                throw new ArgumentNullException(nameof(target));
#endif

            return PinSystemArrayAndGetAddress(target, out gcHandle);
        }

        [ThreadSafe]
        unsafe private static extern void* PinSystemArrayAndGetAddress(System.Object target, out ulong gcHandle);

        [ThreadSafe]
        unsafe private static extern void* PinSystemObjectAndGetAddress(System.Object target, out ulong gcHandle);

        [ThreadSafe]
        unsafe public static extern void ReleaseGCObject(ulong gcHandle);

        [ThreadSafe]
        unsafe public static extern void CopyObjectAddressToPtr(object target, void* dstPtr);

        public static unsafe bool IsBlittable<T>() where T : struct
        {
            return IsBlittable(typeof(T));
        }

        [ThreadSafe]
        unsafe public static extern void* Malloc(long size, int alignment, Allocator allocator);

        [ThreadSafe]
        unsafe public static extern void Free(void* memory, Allocator allocator);

        public static bool IsValidAllocator(Allocator allocator) { return allocator > Allocator.None; }


        [ThreadSafe]
        unsafe public static extern void MemCpy(void* destination, void* source, long size);

        [ThreadSafe]
        unsafe public static extern void MemCpyReplicate(void* destination, void* source, int size, int count);

        [ThreadSafe]
        unsafe public static extern void MemCpyStride(void* destination, int destinationStride, void* source, int sourceStride, int elementSize, int count);

        [ThreadSafe]
        unsafe public static extern void MemMove(void* destination, void* source, long size);

        [ThreadSafe]
        unsafe public static extern void MemSet(void* destination, byte value, long size);

        unsafe public static void MemClear(void* destination, long size)
        {
            MemSet(destination, 0, size);
        }

        [ThreadSafe]
        unsafe public static extern int MemCmp(void* ptr1, void* ptr2, long size);

        [ThreadSafe]
        public static extern int SizeOf(Type type);

        [ThreadSafe]
        public static extern bool IsBlittable(Type type);

        [ThreadSafe]
        public static extern bool IsUnmanaged(Type type);

        [ThreadSafe]
        public static extern bool IsValidNativeContainerElementType(Type type);

        // @TODO : This is probably not the ideal place to have this?
        [ThreadSafe]
        internal static extern void LogError(string msg, string filename, int linenumber);
    }
}
