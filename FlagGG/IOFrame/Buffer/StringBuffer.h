#pragma once

#include "Export.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Str.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class FlagGG_API StringBuffer : public IOBuffer
			{
			public:
				StringBuffer();

				StringBuffer(void* data, uint32_t length);

				StringBuffer(const void* data, uint32_t length);

				uint32_t GetIndex() const override;
				uint32_t GetSize() const override;

				void ClearIndex() override;

				void Seek(uint32_t pos) override;

				void ReadInt8(int8_t& value) override;
				void WriteInt8(int8_t value) override;

				void ReadUInt8(uint8_t& value) override;
				void WriteUInt8(uint8_t value) override;

				void ReadInt16(int16_t& value) override;
				void WriteInt16(int16_t value) override;

				void ReadUInt16(uint16_t& value) override;
				void WriteUInt16(uint16_t value) override;

				void ReadInt32(int32_t& value) override;
				void WriteInt32(int32_t value) override;

				void ReadUInt32(uint32_t& value) override;
				void WriteUInt32(uint32_t value) override;

				void ReadInt64(int64_t& value) override;
				void WriteInt64(int64_t value) override;

				void ReadUInt64(uint64_t& value) override;
				void WriteUInt64(uint64_t value) override;

				uint32_t ReadStream(void* data, size_t dataSize) override;
				uint32_t WriteStream(const void* data, size_t dataSize) override;

				void ReadFloat(float& value) override;
				void WriteFloat(float value) override;

				void ToString(char*& data, size_t& dataSize) override;

				bool IsEof() override;
			
			private:
				bool sizeFixed_; // buffer¥Û–°πÃ∂®
				bool readOnly_;

				char* cBuffer_;
				const char* ccBuffer_;

				uint32_t index_;
				uint32_t bufferSize_;
			};
		}
	}
}
