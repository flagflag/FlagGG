#pragma once

#include "Container/Allocator.h"
#include "Container/Swap.h"

#include "Export.h"

namespace FlagGG
{
	namespace Container
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
				FlagGG::Container::Swap(head_, rhs.head_);
				FlagGG::Container::Swap(tail_, rhs.tail_);
				FlagGG::Container::Swap(allocator_, rhs.allocator_);
				FlagGG::Container::Swap(size_, rhs.size_);
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
}
