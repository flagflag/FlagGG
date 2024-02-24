using System;
using System.Runtime.InteropServices;

namespace UnityEngine
{
    public partial class Application
    {
        // Returns true when in any kind of player (RO).
        public extern static bool isPlaying
        {
            [DllImport(Import.DLLName, EntryPoint = "Application_IsPlaying")]
            get;
        }

        public static RuntimePlatform platform { get { return RuntimePlatform.WindowsPlayer; } }
    }
}
