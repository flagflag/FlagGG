#pragma once

#include "Export.h"
#include "IOFrame/Buffer/IOBufferAdaptor.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class FlagGG_API StringBuffer : public IOBufferAdaptor
			{
			public:
				StringBuffer();

				StringBuffer(void* data, UInt32 length);

				StringBuffer(const void* data, UInt32 length);

				UInt32 GetIndex() const override;
				UInt32 GetSize() const override;

				void ClearIndex() override;

				void Seek(UInt32 pos) override;

				void Clear() override;

				UInt32 ReadStream(void* data, UInt32 dataSize) override;
				UInt32 WriteStream(const void* data, UInt32 dataSize) override;

				bool IsEof() override;
			
			private:
				bool sizeFixed_; // buffer¥Û–°πÃ∂®
				bool readOnly_;

				char* cBuffer_;
				const char* ccBuffer_;

				UInt32 index_;
				UInt32 bufferSize_;
				UInt32 capacity_;
			};
		}
	}
}
