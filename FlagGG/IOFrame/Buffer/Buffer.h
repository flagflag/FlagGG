#ifndef __BUFFER__
#define __BUFFER__

#include "IOBuffer.h"

#include <vector>
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

				void ClearIndex() override;

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

				void WriteStream(const char* data, size_t data_size) override;

				void ToString(char*& data, size_t& data_size) override;

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
				std::vector < SimpleBuffer > buffers_;

				int index_;
				
				int count_;
			
				SimpleBuffer currentBuffer_;
			};

			typedef Container::SharedPtr < NetBuffer > NetBufferPtr;
		}
	}
}

#endif