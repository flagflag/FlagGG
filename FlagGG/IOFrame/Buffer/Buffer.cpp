#include "Buffer.h"
#include "Define.h"
#include <algorithm>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Buffer
		{
			NetBuffer::NetBuffer()
			{
				ClearIndex();
			}

			bool NetBuffer::CheckBuffer(int mode)
			{
				if (!m_current_buffer.buffer || m_count >= (int)m_current_buffer.buffer_size)
				{
					if (m_index + 1 >= (int)m_buffers.size())
					{
						if (mode == mode_read) //读模式，不会开辟内存
						{
							return false;
						}

						m_current_buffer.buffer = new char[ONE_KB];
						m_current_buffer.buffer_size = ONE_KB;

						m_index = m_buffers.size();
						m_count = 0;

						m_buffers.emplace_back(m_current_buffer);
					}
					else
					{
						++m_index;
						m_count = 0;

						m_current_buffer = m_buffers[m_index];
					}
				}

				return true;
			}

			void NetBuffer::ClearIndex()
			{
				m_index = -1;
				m_count = 0;
				m_current_buffer.buffer = nullptr;
				m_current_buffer.buffer_size = 0;
			}

			bool NetBuffer::ReadByte(uint8_t& byte)
			{
				if (!CheckBuffer(mode_read))
				{
					return false;
				}

				byte = (uint8_t)m_current_buffer.buffer[m_count];
				++m_count;

				return true;
			}

			bool NetBuffer::WriteByte(uint8_t byte)
			{
				if (!CheckBuffer(mode_write))
				{
					return false;
				}

				m_current_buffer.buffer[m_count] = (char)byte;
				++m_count;

				return true;
			}

			#define __READ__(value) \
			{ \
				value = 0; \
				size_t _size = sizeof(value); \
				uint8_t byte; \
				while (_size--) \
				{ \
					if (!ReadByte(byte)) \
					{ \
						break; \
					} \
					value <<= 8; \
					value |= byte; \
				} \
			}

			#define __WRITE__(value) \
			{ \
				size_t _size = sizeof(value); \
				uint8_t byte; \
				uint8_t offset = (_size - 1) * 8; \
				while (_size--) \
				{ \
					byte = (value >> offset) & 0xff; \
					offset -= 8; \
					if (!WriteByte(byte)) \
					{ \
						break; \
					} \
				} \
			}

			void NetBuffer::ReadInt8(int8_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt8(int8_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt8(uint8_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt8(uint8_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadInt16(int16_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt16(int16_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt16(uint16_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt16(uint16_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadInt32(int32_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt32(int32_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt32(uint32_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt32(uint32_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadInt64(int64_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteInt64(int64_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::ReadUInt64(uint64_t& value)
			{
				__READ__(value);
			}

			void NetBuffer::WriteUInt64(uint64_t value)
			{
				__WRITE__(value);
			}

			void NetBuffer::WriteStream(const char* data, size_t data_size)
			{
				if (!data || data_size == 0) return;

				const char* index = data;
				size_t left_size = data_size;
				while (left_size)
				{
					CheckBuffer(mode_write);

					size_t write_size = std::min < size_t >(left_size, 
						m_current_buffer.buffer_size - m_count);

					memcpy(m_current_buffer.buffer, index, write_size);
					index += write_size;
					m_count += write_size;

					left_size -= write_size;
				}
			}

			void NetBuffer::ToString(char*& data, size_t& data_size)
			{
				if (m_buffers.size() <= 0)
				{
					data = nullptr;
					data_size = 0;
					return;
				}

				data_size = 0;
				for (size_t i = 0; i < m_buffers.size(); ++i)
				{
					data_size += (i + 1 == m_buffers.size() ? m_count : m_buffers[i].buffer_size);
				}

				data = new char[data_size];
				char* index = data;
				for (size_t i = 0; i < m_buffers.size(); ++i)
				{
					size_t real_size = (i + 1 == m_buffers.size() ? m_count : m_buffers[i].buffer_size);
					memcpy(index, m_buffers[i].buffer, real_size);
					index += real_size;
				}
			}
		}
	}
}