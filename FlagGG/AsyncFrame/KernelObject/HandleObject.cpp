#include "HandleObject.h"

#if WIN32 || WIN64
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			HandleObject::HandleObject()
				: m_handle(NULL_HANDLE)
			{ }

			HandleObject::~HandleObject()
			{
				if (m_handle)
				{
#if WIN32 || WIN64
					CloseHandle((HANDLE)m_handle);
#endif
					m_handle = NULL_HANDLE;
				}
			}

			KernelHandle HandleObject::getHandle()
			{
				return m_handle;
			}
		}
	}
}