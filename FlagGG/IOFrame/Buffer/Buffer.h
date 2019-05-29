#ifndef __BUFFER__
#define __BUFFER__

#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Vector.h"

#include <stdint.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			//不区分TCP和UDP，用同一种buffer
			class NetBuffer : public IOBuffer
			{
			public:
				struct SimpleBuffer
				{
					char* buffer;
					size_t bufferSize;
				};

				NetBuffer();

				~NetBuffer() override = default;

				// 先暂时留空，等需要用的时候在补上
				uint32_t GetIndex() const override { return 0; }
				uint32_t GetSize() const override { return 0; }

				void ClearIndex() override;

				// 先暂时留空，等需要用的时候在补上
				void Seek(uint32_t pos) override { }

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

				void ReadFloat(float& value) override;
				void WriteFloat(float value) override;

				uint32_t ReadStream(void* data, size_t dataSize) override { return 0u; }
				uint32_t WriteStream(const void* data, size_t dataSize) override;

				void ToString(char*& data, size_t& dataSize) override;

				// 先暂时留空，等需要用的时候在补上
				bool IsEof() override { return false; }

			protected:
				enum CheckMode
				{
					mode_read = 1,
					mode_write = 2,
				};

				bool CheckBuffer(int mode);

				bool ReadByte(uint8_t& byte);

				bool WriteByte(uint8_t byte);

			private:
				Container::PODVector < SimpleBuffer > buffers_;

				int index_;
				
				int count_;
			
				SimpleBuffer currentBuffer_;
			};

			typedef Container::SharedPtr < NetBuffer > NetBufferPtr;
		}
	}
}

#endif