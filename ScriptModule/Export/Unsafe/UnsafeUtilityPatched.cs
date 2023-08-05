using System;
using System.Reflection;
using System.Runtime.InteropServices;
using UnityEngine.Bindings;
using System.Runtime.CompilerServices;

namespace Unity.Collections.LowLevel.Unsafe
{
    public static partial class UnsafeUtility
    {
        // Copies sizeof(T) bytes from ptr to output
        [MethodImpl(256)] // AggressiveInlining
        unsafe public static void CopyPtrToStructure<T>(void* ptr, out T output) where T : struct
        {
#if ENABLE_UNITY_COLLECTIONS_CHECKS
            if (ptr == null)
                throw new ArgumentNullException();
#endif
            InternalCopyPtrToStructure(ptr, out output);
        }

        unsafe static void InternalCopyPtrToStructure<T>(void* ptr, out T output) where T : struct
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        // Copies sizeof(T) bytes from output to ptr
        [MethodImpl(256)] // AggressiveInlining
        unsafe public static void CopyStructureToPtr<T>(ref T input, void* ptr) where T : struct
        {
#if ENABLE_UNITY_COLLECTIONS_CHECKS
            if (ptr == null)
                throw new ArgumentNullException();
#endif
            InternalCopyStructureToPtr(ref input, ptr);
        }

        unsafe static void InternalCopyStructureToPtr<T>(ref T input, void* ptr) where T : struct
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        unsafe public static T ReadArrayElement<T>(void* source, int index)
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        unsafe public static T ReadArrayElementWithStride<T>(void* source, int index, int stride)
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        unsafe public static void WriteArrayElement<T>(void* destination, int index, T value)
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        unsafe public static void WriteArrayElementWithStride<T>(void* destination, int index, int stride, T value)
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        // The address of the memory where the struct resides in memory
        unsafe public static void* AddressOf<T>(ref T output) where T : struct
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        // The size of a struct
        public static int SizeOf<T>() where T : struct
        {
            // @patched at compile time
            throw new NotImplementedException("Patching this method failed");
        }

        // converts generic enum to int without boxing
        public static int EnumToInt<T>(T enumValue) where T : struct, IConvertible
        {
            var value = 0;
            InternalEnumToInt(ref enumValue, ref value);
            return value;
        }

        static void InternalEnumToInt<T>(ref T enumValue, ref int intValue)
        {
            throw new NotImplementedException("Patching this method failed");
        }
    }
}
