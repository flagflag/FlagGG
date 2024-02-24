using System;
using System.Runtime.InteropServices;
using UnityEngine;

using UnityEngine.Scripting;
using UnityEngine.Bindings;
using scm = System.ComponentModel;
using uei = UnityEngine.Internal;
using System.Globalization;

namespace FlagGG
{
    public static unsafe class ScriptInterface
    {
        [DllImport(Import.DLLName, EntryPoint = "CallCpp3")]
        public static extern void CallCpp3(Vector3 vec3);

        [UnmanagedCallersOnly]
        public static void Init()
        {
            Console.WriteLine("----- UIElementsInterface Method Init Called -----");

            // CallCpp1(1, 1);

            Vector3 vec3 = new Vector3(1.0f, 2.0f, 3.0f);
            CallCpp3(vec3);

            // Debug.DrawLine(TTVector3.one, TTVector3.zero, Color.black, 1.0f, true);
            Console.WriteLine("----- UIElementsInterface Method Init Called 2 -----");
        }

        [UnmanagedCallersOnly]
        public static void Tick()
        {
            Console.WriteLine("----- UIElementsInterface Method Tick Called -----");
        }

        [UnmanagedCallersOnly]
        public static void Render()
        {
            Console.WriteLine("----- UIElementsInterface Method Render Called -----");
        }

        [UnmanagedCallersOnly]
        public static void MathTest()
        {
            Console.WriteLine("Math Test ==> Start");
            Vector3 A = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 B = new Vector3(3.0f, 2.0f, 1.0f);
            Vector3 C = Vector3.Lerp(A, B, 0.5f);
            Console.WriteLine("Math Test ==> " + C.ToString());

            Vector3 D = Vector3.Slerp(A, B, 0.5f);
            Console.WriteLine("Math Test ==> " + D.ToString());
            D = Vector3.SlerpUnclamped(A, B, 0.5f);
            Console.WriteLine("Math Test ==> " + D.ToString());

            Vector3.OrthoNormalize(ref A, ref B);
            Vector3.OrthoNormalize(ref A, ref B, ref C);

            Vector3 E = Vector3.RotateTowards(A, B, 1.0f, 1.0f);
            Console.WriteLine("Math Test ==> " + E.ToString());

            Console.WriteLine("Math Test ==> " + Mathf.ClosestPowerOfTwo(5));
            Console.WriteLine("Math Test ==> " + Mathf.IsPowerOfTwo(233));
            Console.WriteLine("Math Test ==> " + Mathf.NextPowerOfTwo(666));

            Console.WriteLine("Math Test ==> " + Mathf.GammaToLinearSpace(0.5f));
            Console.WriteLine("Math Test ==> " + Mathf.LinearToGammaSpace(0.2f));
            Console.WriteLine("Math Test ==> " + Mathf.CorrelatedColorTemperatureToRGB(66.0f));
            Console.WriteLine("Math Test ==> " + Mathf.FloatToHalf(666));
            Console.WriteLine("Math Test ==> " + Mathf.HalfToFloat(124));
            Console.WriteLine("Math Test ==> " + Mathf.PerlinNoise(3, 5));

            Console.WriteLine("Math Test ==> End");
        }
    }
}
