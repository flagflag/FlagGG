#ifndef __BUFFER__
#define __BUFFER__

#include <vector>
#include <memory>
#include <stdint.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			class IOBuffer
			{
			public:
				virtual void toString(char*& data, size_t& data_size) = 0;
			};

			typedef std::shared_ptr < IOBuffer > IOBufferPtr;

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

				virtual ~NetBuffer();

				void clearIndex();

				void readInt8(int8_t& value);
				void writeInt8(int8_t value);

				void readUInt8(uint8_t& value);
				void writeUInt8(uint8_t value);

				void readInt16(int16_t& value);
				void writeInt16(int16_t value);

				void readUInt16(uint16_t& value);
				void writeUInt16(uint16_t value);

				void readInt32(int32_t& value);
				void writeInt32(int32_t value);

				void readUInt32(uint32_t& value);
				void writeUInt32(uint32_t value);

				void readInt64(int64_t& value);
				void writeInt64(int64_t value);

				void readUInt64(uint64_t& value);
				void writeUInt64(uint64_t value);

				void writeStream(const char* data, size_t data_size);

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