/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

using System;
using System.Runtime.InteropServices;

namespace UnityEngine.Yoga
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct YogaValue
    {
        private float value;
        private YogaUnit unit;

        public YogaUnit Unit
        {
            get
            {
                return unit;
            }
        }

        public float Value
        {
            get
            {
                return value;
            }
        }

        public static YogaValue Point(float value)
        {
            return new YogaValue
            {
                value = value,
                unit = YogaConstants.IsUndefined(value) ? YogaUnit.Undefined : YogaUnit.Point
            };
        }

        public bool Equals(YogaValue other)
        {
            return Unit == other.Unit && (Value.Equals(other.Value) || Unit == YogaUnit.Undefined);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            return obj is YogaValue && Equals((YogaValue) obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return (Value.GetHashCode() * 397) ^ (int) Unit;
            }
        }

        public static YogaValue Undefined()
        {
            return new YogaValue
            {
                value = YogaConstants.Undefined,
                unit = YogaUnit.Undefined
            };
        }

        public static YogaValue Auto()
        {
            return new YogaValue
            {
                // BEGIN_UNITY @jonathanma For some styles Auto has to be NaN
                value = float.NaN,
                // END_UNITY
                unit = YogaUnit.Auto
            };
        }

        public static YogaValue Percent(float value)
        {
            return new YogaValue
            {
                value = value,
                unit = YogaConstants.IsUndefined(value) ? YogaUnit.Undefined : YogaUnit.Percent
            };
        }

        public static implicit operator YogaValue(float pointValue)
        {
            return Point(pointValue);
        }

#if WINDOWS_UWP_ARM
        internal static YogaValue MarshalValue(IntPtr ptr)
        {
            return Marshal.PtrToStructure<YogaValue>(ptr);
        }
#else
        internal static YogaValue MarshalValue(YogaValue value)
        {
            return value;
        }
#endif
    }
}
