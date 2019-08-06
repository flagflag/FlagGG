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

				StringBuffer(void* data, uint32_t length);

				StringBuffer(const void* data, uint32_t length);

				uint32_t GetIndex() const override;
				uint32_t GetSize() const override;

				void ClearIndex() override;

				void Seek(uint32_t pos) override;

				uint32_t ReadStream(void* data, uint32_t dataSize) override;
				uint32_t WriteStream(const void* data, uint32_t dataSize) override;

				bool IsEof() override;
			
			private:
				bool sizeFixed_; // buffer¥Û–°πÃ∂®
				bool readOnly_;

				char* cBuffer_;
				const char* ccBuffer_;

				uint32_t index_;
				uint32_t bufferSize_;
				uint32_t capacity_;
			};
		}
	}
}
