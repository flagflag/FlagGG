#ifndef __RESOURCE__
#define __RESOURCE__

#include "Export.h"
#include "Container/Str.h"
#include "Container/RefCounted.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Core/Context.h"

namespace FlagGG
{
	namespace Resource
	{
		class FlagGG_API Resource : public Container::RefCounted
		{
		public:
			Resource(Core::Context* context);

			~Resource() override = default;

			bool LoadFile(const Container::String& filepath);

			bool LoadFile(IOFrame::Buffer::IOBuffer* stream);

			bool SaveFile(const Container::String& filepath);

			bool SaveFile(IOFrame::Buffer::IOBuffer* stream);

			void SetMemoryUse(UInt32 memoryUse);

			UInt32 GetMemoryUse() const;

		protected:
			// ����BeginLoadΪʲô����ɷǹ���ָ���ԭ��LoadFile����ջ�ϴ���IOBuffer������
			virtual bool BeginLoad(IOFrame::Buffer::IOBuffer* stream);

			virtual bool EndLoad();

			virtual bool BeginSave(IOFrame::Buffer::IOBuffer* stream);

			virtual bool EndSave();

			Core::Context* context_;

			UInt32 memoryUse_{ 0 };
		};
	}
}

#endif
