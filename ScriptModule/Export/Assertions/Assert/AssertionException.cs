using System;
using UnityEngine;

namespace UnityEngine.Assertions
{
    public class AssertionException : Exception
    {
        string m_UserMessage;

        public AssertionException(string message, string userMessage)
            : base(message)
        {
            m_UserMessage = userMessage;
        }

        public override string Message
        {
            get
            {
                var message = base.Message;
                if (m_UserMessage != null)
                    message += '\n' + m_UserMessage;
                return message;
            }
        }
    }
}
