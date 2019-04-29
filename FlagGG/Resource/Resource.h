#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>
#include "Export.h"
#include "Container/Str.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"

namespace FlagGG
{
	namespace Resource
	{
		class FlagGG_API Resource : public Container::RefCounted
		{
		public:
			virtual ~Resource() = default;

			bool LoadFile(const Container::String& fileName);

			bool LoadFile(IOFrame::Buffer::IOBuffer* stream);

		protected:
			// ����BeginLoadΪʲô����ɷǹ���ָ���ԭ��LoadFile����ջ�ϴ���IOBuffer������
			virtual bool BeginLoad(IOFrame::Buffer::IOBuffer* stream);

			virtual bool EndLoad();
		};
	}
}

#endif
