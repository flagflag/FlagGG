#pragma once

#include <initializer_list>

#include "Export.h"
#include "Container/ListBase.h"

namespace FlagGG
{

template <class T>
class LinkedListNode : public ListNodeBase
{
public:
	LinkedListNode(T* data = nullptr)
		: data_(data)
	{}

	T& operator*() const { return *data_; }
	T* operator->() const { return data_; }
	T* GetData() const { return data_; }
	void SetData(T* data) { data_ = data; }

	T* data_;
};

/// Singly-linked list template class. Elements must inherit from LinkedListNode.
template <class T> class LinkedList
{
public:
	/// %List iterator.
	struct Iterator : public ListIteratorBase
	{
		/// Construct.
		Iterator() = default;

		/// Construct with a node pointer.
		explicit Iterator(T* ptr) :
			ListIteratorBase(ptr)
		{
		}

		/// Construct with a base node pointer.
		explicit Iterator(ListNodeBase* ptr)
			: ListIteratorBase(ptr)
		{
		}

		/// Preincrement the pointer.
		Iterator& operator ++()
		{
			GotoNext();
			return *this;
		}

		/// Postincrement the pointer.
		Iterator operator ++(int)
		{
			Iterator it = *this;
			GotoNext();
			return it;
		}

		/// Predecrement the pointer.
		Iterator& operator --()
		{
			GotoPrev();
			return *this;
		}

		/// Postdecrement the pointer.
		Iterator operator --(int)
		{
			Iterator it = *this;
			GotoPrev();
			return it;
		}

		/// Point to the node value.
		T* operator ->() const { return static_cast<T*>(ptr_); }

		/// Dereference the node value.
		T& operator *() const { return *static_cast<T*>(ptr_); }
	};

	/// Construct empty.
	LinkedList()
	{
		root_.prev_ = &root_;
		root_.next_ = &root_;
	}

	/// Non-copyable.
	LinkedList(const LinkedList<T>& list) = delete;

	/// Aggregate initialization constructor.
	LinkedList(const std::initializer_list<T>& list) : LinkedList()
	{
		for (auto it = list.begin(); it != list.end(); it++)
		{
			Insert(*it);
		}
	}

	/// Non-assignable.
	LinkedList<T>& operator =(const LinkedList<T>& list) = delete;

	/// Destruct.
	~LinkedList()
	{
		Clear();
	}

	/// Remove all elements.
	void Clear()
	{
		ListNodeBase* node = root_.next_;
		while (node != &root_)
		{
			ListNodeBase* next = node->next_;
			node->prev_ = nullptr;
			node->next_ = nullptr;
			node = next;
		}
		root_.prev_ = &root_;
		root_.next_ = &root_;
	}

	/// Insert an element to the end.
	void Push(T& value) { InsertNode(Tail(), value); }

	/// Insert an element to the beginning.
	void PushFront(T& value) { InsertNode(Head(), value); }

	/// Insert an element at position.
	void Insert(const Iterator& dest, T& value) { InsertNode(dest.ptr_, value); }

	/// Erase the last element.
	void Pop()
	{
		if (!Empty())
			Erase(--End());
	}

	/// Erase the first element.
	void PopFront()
	{
		if (!Empty())
			Erase(Begin());
	}

	/// Erase an element by iterator. Return iterator to the next element.
	Iterator Erase(Iterator it)
	{
		return Iterator(EraseNode(it.ptr_));
	}

	/// Erase a range by iterators. Return an iterator to the next element.
	Iterator Erase(const Iterator& start, const Iterator& end)
	{
		Iterator it = start;
		while (it != end)
			it = Erase(it);

		return it;
	}

	/// Return iterator to value, or to the end if not found.
	Iterator Find(const T& value)
	{
		Iterator it = Begin();
		while (it != End() && *it != value)
			++it;
		return it;
	}

	/// Return iterator to the first element.
	Iterator Begin() { return Iterator(Head()); }

	/// Return iterator to the end.
	Iterator End() { return Iterator(Tail()); }

	/// Return first element.
	T& Front() { return *Begin(); }

	/// Return const first element.
	const T& Front() const { return *Begin(); }

	/// Return last element.
	T& Back() { return *(--End()); }

	/// Return const last element.
	const T& Back() const { return *(--End()); }

	/// Return whether list is empty.
	bool Empty() const { return root_.next_ == &root_; }

private:
	/// Return the head node.
	T* Head() { return static_cast<T*>(root_.next_); }

	/// Return the tail node.
	ListNodeBase* Tail() { return &root_; }

	/// Allocate and insert a node into the list.
	void InsertNode(ListNodeBase* dest, T& value)
	{
		value.InsertInList(dest);
	}

	/// Erase and free a node. Return pointer to the next node, or to the end if could not erase.
	ListNodeBase* EraseNode(ListNodeBase* node)
	{
		ListNodeBase* next = node->next_;
		node->RemoveFromList();
		return next;
	}

	/// Last element
	ListNodeBase root_;
};

template <class T> typename LinkedList<T>::ConstIterator begin(const LinkedList<T>& v) { return v.Begin(); }

template <class T> typename LinkedList<T>::ConstIterator end(const LinkedList<T>& v) { return v.End(); }

template <class T> typename LinkedList<T>::Iterator begin(LinkedList<T>& v) { return v.Begin(); }

template <class T> typename LinkedList<T>::Iterator end(LinkedList<T>& v) { return v.End(); }

}
