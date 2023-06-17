#pragma once

#include "Container/Allocator.h"
#include "Container/Swap.h"
#include "Core/BaseMacro.h"

#include "Export.h"

namespace FlagGG
{

/// Doubly-linked list node base class.
struct ListNodeBase
{
	/// Construct.
	ListNodeBase() :
	prev_(nullptr),
	next_(nullptr)
	{
	}

	FORCEINLINE bool IsInList() const
	{
		return prev_ || next_;
	}

	FORCEINLINE bool RemoveFromList()
	{
		if (!IsInList())
			return false;

		prev_->next_ = next_;
		next_->prev_ = prev_;
		prev_ = nullptr;
		next_ = nullptr;

		return true;
	}

	FORCEINLINE void InsertInList(ListNodeBase* pos)
	{
		if (this == pos)
			return;

		if (IsInList())
			RemoveFromList();

		prev_ = pos->prev_;
		next_ = pos;
		prev_->next_ = this;
		next_->prev_ = this;
	}

	ListNodeBase* GetPrev() const { return prev_; }
	ListNodeBase* GetNext() const { return next_; }

	/// Previous node.
	ListNodeBase* prev_;
	/// Next node.
	ListNodeBase* next_;
};

/// Doubly-linked list iterator base class.
struct ListIteratorBase
{
	/// Construct.
	ListIteratorBase() :
	ptr_(nullptr)
	{
	}

	/// Construct with a node pointer.
	explicit ListIteratorBase(ListNodeBase* ptr) :
		ptr_(ptr)
	{
	}

	/// Test for equality with another iterator.
	bool operator ==(const ListIteratorBase& rhs) const { return ptr_ == rhs.ptr_; }

	/// Test for inequality with another iterator.
	bool operator !=(const ListIteratorBase& rhs) const { return ptr_ != rhs.ptr_; }

	/// Go to the next node.
	void GotoNext()
	{
		if (ptr_)
			ptr_ = ptr_->next_;
	}

	/// Go to the previous node.
	void GotoPrev()
	{
		if (ptr_)
			ptr_ = ptr_->prev_;
	}

	/// Node pointer.
	ListNodeBase* ptr_;
};

/// Doubly-linked list base class.
class FlagGG_API ListBase
{
public:
	/// Construct.
	ListBase() :
		head_(nullptr),
		tail_(nullptr),
		allocator_(nullptr),
		size_(0)
	{
	}

	/// Swap with another linked list.
	void Swap(ListBase& rhs)
	{
		FlagGG::Swap(head_, rhs.head_);
		FlagGG::Swap(tail_, rhs.tail_);
		FlagGG::Swap(allocator_, rhs.allocator_);
		FlagGG::Swap(size_, rhs.size_);
	}

protected:
	/// Head node pointer.
	ListNodeBase* head_;
	/// Tail node pointer.
	ListNodeBase* tail_;
	/// Node allocator.
	AllocatorBlock* allocator_;
	/// Number of nodes.
	unsigned size_;
};

}
