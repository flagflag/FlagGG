#include "HandleObject.h"

#include <windows.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace KernelObject
		{
			HandleObject::HandleObject()
				: m_handle(nullptr)
			{ }

			HandleObject::~HandleObject()
			{
				if (m_handle)
				{
					CloseHandle((HANDLE)m_handle);
					m_handle = nullptr;
				}
			}

			KernelHandle HandleObject::getHandle()
			{
				return m_handle;
			}
		}
	}
}