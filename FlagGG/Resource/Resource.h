#ifndef __RESOURCE__
#define __RESOURCE__

#include "Export.h"
#include "Container/Str.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Core/Contex.h"

namespace FlagGG
{
	namespace Resource
	{
		class FlagGG_API Resource : public Container::RefCounted
		{
		public:
			Resource(Core::Context* context);

			~Resource() override = default;

			bool LoadFile(const Container::String& fileName);

			bool LoadFile(IOFrame::Buffer::IOBuffer* stream);

		protected:
			// ����BeginLoadΪʲô����ɷǹ���ָ���ԭ��LoadFile����ջ�ϴ���IOBuffer������
			virtual bool BeginLoad(IOFrame::Buffer::IOBuffer* stream);

			virtual bool EndLoad();

			Core::Context* context_;
		};
	}
}

#endif
