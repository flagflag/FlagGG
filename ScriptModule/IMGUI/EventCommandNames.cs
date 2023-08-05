using System;
using System.Runtime.InteropServices;
using UnityEngine.Scripting;

namespace UnityEngine
{
    internal static class EventCommandNames
    {
        //Some of these strings are also hardcoded on the native side. Change them at your own risk!
        public const string Cut = "Cut";
        public const string Copy = "Copy";
        public const string Paste = "Paste";
        public const string SelectAll = "SelectAll";
        public const string DeselectAll = "DeselectAll";
        public const string InvertSelection = "InvertSelection";
        public const string Duplicate = "Duplicate";
        public const string Rename = "Rename";
        public const string Delete = "Delete";
        public const string SoftDelete = "SoftDelete";
        public const string Find = "Find";
        public const string SelectChildren = "SelectChildren";
        public const string SelectPrefabRoot = "SelectPrefabRoot";

        public const string UndoRedoPerformed = "UndoRedoPerformed";
        public const string OnLostFocus = "OnLostFocus";

        //Used by IMGUIContainer to force editing textfield when focus is changed with tab
        public const string NewKeyboardFocus = "NewKeyboardFocus";
        public const string ModifierKeysChanged = "ModifierKeysChanged";

        //Used by ColorPicker
        public const string EyeDropperUpdate = "EyeDropperUpdate";
        public const string EyeDropperClicked = "EyeDropperClicked";
        public const string EyeDropperCancelled = "EyeDropperCancelled";
        public const string ColorPickerChanged = "ColorPickerChanged";


        public const string FrameSelected = "FrameSelected";
        public const string FrameSelectedWithLock = "FrameSelectedWithLock";


        /*
                 public const string  = "";
                 public const string  = "";
                 public const string  = "";
                 public const string  = "";
*/
    }
}
