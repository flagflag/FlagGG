#include "Ultralight/Buffer.h"
#include "Ultralight/private/util/RefCountedImpl.h"

#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

class BufferImpl : public Buffer, public RefCountedImpl<BufferImpl>
{
public:
	REF_COUNTED_IMPL(BufferImpl);

	BufferImpl(void* data, size_t size, void* userData, DestroyBufferCallback destroyCallback)
		: data_(data)
		, dataSize_(size)
		, userData_(userData)
		, destroyCallback_(destroyCallback)
	{

	}

	BufferImpl(const void* data, size_t size)
		: dataSize_(size)
		, userData_(nullptr)
		, destroyCallback_(nullptr)
	{
		data_ = new char[size];
		Memory::Memcpy(data_, data, size);
	}

	~BufferImpl() override
	{
		if (destroyCallback_)
		{
			destroyCallback_(userData_, data_);
			destroyCallback_ = nullptr;
		}
		else
		{
			delete[] data_;
		}
	}

	///
	/// Get a pointer to the raw byte data.
	///
	virtual void* data() override
	{
		return data_;
	}

	///
	/// Get the size in bytes.
	///
	virtual size_t size() const override
	{
		return dataSize_;
	}

	///
	/// Get the user data associated with this Buffer, if any.
	///
	virtual void* user_data() override
	{
		return userData_;
	}

	///
	/// Check whether this Buffer owns its own data (Buffer was created via CreateFromCopy).
	/// If this is false, Buffer will call the user-supplied destruction callback to deallocate data
	/// when this Buffer instance is destroyed.
	///
	virtual bool owns_data() const override
	{
		return !destroyCallback_;
	}

private:
	void* data_;

	size_t dataSize_;

	void* userData_;

	DestroyBufferCallback destroyCallback_;
};

Buffer::Buffer() = default;

Buffer::~Buffer() = default;

Buffer::Buffer(const Buffer&)
{

}

void Buffer::operator=(const Buffer&)
{

}

RefPtr<Buffer> Buffer::Create(void* data, size_t size, void* user_data, DestroyBufferCallback destruction_callback)
{
	return AdoptRef<Buffer>(*new BufferImpl(data, size, user_data, destruction_callback));
}

RefPtr<Buffer> Buffer::CreateFromCopy(const void* data, size_t size)
{
	return AdoptRef<Buffer>(*new BufferImpl(data, size));
}

}
