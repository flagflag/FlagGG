using System;
using System.Runtime.InteropServices;

namespace FlagGG
{ 
    public static unsafe class UIElementsInterface
    {
        [UnmanagedCallersOnly]
        public static void Init()
        {
            Console.WriteLine("----- UIElementsInterface Method Init Called -----");
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
    }
}
