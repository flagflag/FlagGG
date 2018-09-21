#include "HandleObject.h"

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
					CloseHandle(m_handle);
					m_handle = nullptr;
				}
			}

			HANDLE HandleObject::getHandle()
			{
				return m_handle;
			}
		}
	}
}