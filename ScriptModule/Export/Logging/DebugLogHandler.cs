using System;

namespace UnityEngine
{
    internal partial class DebugLogHandler : ILogHandler
    {
        public void LogFormat(LogType logType, Object context, string format, params object[] args)
        {
            Internal_Log(logType, LogOption.None, string.Format(format, args), context);
        }

        public void LogFormat(LogType logType, LogOption logOptions, Object context, string format, params object[] args)
        {
            Internal_Log(logType, logOptions, string.Format(format, args), context);
        }

        public void LogException(Exception exception, Object context)
        {
            if (exception == null)
                throw new ArgumentNullException("exception");

            Internal_LogException(exception, context);
        }
    }
}
