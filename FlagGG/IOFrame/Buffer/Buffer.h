#ifndef __BUFFER__
#define __BUFFER__

#include "IOBuffer.h"

#include <vector>
#include <memory>
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
					size_t buffer_size;
				};

				NetBuffer();

				~NetBuffer() override = default;

				void clearIndex() override;

				void readInt8(int8_t& value) override;
				void writeInt8(int8_t value) override;

				void readUInt8(uint8_t& value) override;
				void writeUInt8(uint8_t value) override;

				void readInt16(int16_t& value) override;
				void writeInt16(int16_t value) override;

				void readUInt16(uint16_t& value) override;
				void writeUInt16(uint16_t value) override;

				void readInt32(int32_t& value) override;
				void writeInt32(int32_t value) override;

				void readUInt32(uint32_t& value) override;
				void writeUInt32(uint32_t value) override;

				void readInt64(int64_t& value) override;
				void writeInt64(int64_t value) override;

				void readUInt64(uint64_t& value) override;
				void writeUInt64(uint64_t value) override;

				void writeStream(const char* data, size_t data_size) override;

				void toString(char*& data, size_t& data_size) override;

			protected:
				enum CheckMode
				{
					mode_read = 1,
					mode_write = 2,
				};

				bool checkBuffer(int mode);

				bool readByte(uint8_t& byte);

				bool writeByte(uint8_t byte);

			private:
				std::vector < SimpleBuffer > m_buffers;

				int m_index;
				
				int m_count;
			
				SimpleBuffer m_current_buffer;
			};

			typedef std::shared_ptr < NetBuffer > NetBufferPtr;
		}
	}
}

#endif