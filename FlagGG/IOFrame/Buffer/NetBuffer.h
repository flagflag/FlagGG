#ifndef __BUFFER__
#define __BUFFER__

#include "IOFrame/Buffer/IOBuffer.h"
#include "Container/Vector.h"
#include "Core/BaseTypes.h"

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
					Size bufferSize;
				};

				NetBuffer();

				~NetBuffer() override = default;

				// 先暂时留空，等需要用的时候在补上
				UInt32 GetIndex() const override { return 0; }
				UInt32 GetSize() const override;

				void ClearIndex() override;

				// 先暂时留空，等需要用的时候在补上
				void Seek(UInt32 pos) override { }

				// 先暂时留空，等需要用的时候在补上
				void Clear() override {}

				void ReadInt8(Int8& value) override;
				void WriteInt8(Int8 value) override;

				void ReadUInt8(UInt8& value) override;
				void WriteUInt8(UInt8 value) override;

				void ReadInt16(Int16& value) override;
				void WriteInt16(Int16 value) override;

				void ReadUInt16(UInt16& value) override;
				void WriteUInt16(UInt16 value) override;

				void ReadInt32(Int32& value) override;
				void WriteInt32(Int32 value) override;

				void ReadUInt32(UInt32& value) override;
				void WriteUInt32(UInt32 value) override;

				void ReadInt64(Int64& value) override;
				void WriteInt64(Int64 value) override;

				void ReadUInt64(UInt64& value) override;
				void WriteUInt64(UInt64 value) override;

				void ReadFloat(float& value) override;
				void WriteFloat(float value) override;

				UInt32 ReadStream(void* data, UInt32 dataSize) override;
				UInt32 WriteStream(const void* data, UInt32 dataSize) override;

				void ToString(Container::String& result) override;
				void ToBuffer(Container::SharedArrayPtr<char>& buffer, UInt32& bufferSize) override;

				// 先暂时留空，等需要用的时候在补上
				bool IsEof() override { return false; }

			protected:
				enum CheckMode
				{
					mode_read = 1,
					mode_write = 2,
				};

				bool CheckBuffer(int mode);

				bool ReadByte(UInt8& byte);

				bool WriteByte(UInt8 byte);

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