using System;
using System.Diagnostics;
using UnityEngine;

namespace UnityEngine.Assertions
{
    public static partial class Assert
    {
        [Conditional(UNITY_ASSERTIONS)]
        public static void IsTrue(bool condition)
        {
            if (!condition)
                IsTrue(condition, null);
        }

        [Conditional(UNITY_ASSERTIONS)]
        public static void IsTrue(bool condition, string message)
        {
            if (!condition)
                Fail(AssertionMessageUtil.BooleanFailureMessage(true), message);
        }

        [Conditional(UNITY_ASSERTIONS)]
        public static void IsFalse(bool condition)
        {
            if (condition)
                IsFalse(condition, null);
        }

        [Conditional(UNITY_ASSERTIONS)]
        public static void IsFalse(bool condition, string message)
        {
            if (condition)
                Fail(AssertionMessageUtil.BooleanFailureMessage(false), message);
        }
    }
}
