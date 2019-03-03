#pragma once

#include "../Container/Vector.h"
#include "../Container/List.h"
#include "../Container/HashSet.h"
#include "../Container/HashMap.h"

#include <algorithm>

// VS2010+ and other compilers: use std::begin(), std::end() & range based for
// C++11 features need to be enabled
#if !defined(_MSC_VER) || _MSC_VER > 1600
#define foreach(VAL, VALS) for (VAL : VALS)
#else

namespace FlagGG
{
	namespace Container
	{
		template<typename T>
		struct false_wrapper {
			false_wrapper(const T& value) : value(value) { }
			operator bool() const { return false; }
			T value;
		};

		template<typename T>
		false_wrapper<T> make_false_wrapper(const T& value) {
			return false_wrapper<T>(value);
		}

		// vector support functions

		template <class T>
		Urho3D::RandomAccessIterator<T> Begin(Urho3D::Vector<T> &v) {
			return v.Begin();
		}
		template <class T>
		Urho3D::RandomAccessIterator<T> Begin(Urho3D::Vector<T> *v) {
			return v->Begin();
		}

		template <class T>
		Urho3D::RandomAccessConstIterator<T> Begin(const Urho3D::Vector<T> &v) {
			return v.Begin();
		}
		template <class T>
		Urho3D::RandomAccessConstIterator<T> Begin(const Urho3D::Vector<T> *v) {
			return v->Begin();
		}

		template <class T>
		Urho3D::RandomAccessIterator<T> End(Urho3D::Vector<T> &v) {
			return v.End();
		}
		template <class T>
		Urho3D::RandomAccessIterator<T> End(Urho3D::Vector<T> *v) {
			return v->End();
		}

		template <class T>
		Urho3D::RandomAccessConstIterator<T> End(const Urho3D::Vector<T> &v) {
			return v.End();
		}
		template <class T>
		Urho3D::RandomAccessConstIterator<T> End(const Urho3D::Vector<T> *v) {
			return v->End();
		}

		// podvector support functions

		template <class T>
		Urho3D::RandomAccessIterator<T> Begin(Urho3D::PODVector<T> &v) {
			return v.Begin();
		}
		template <class T>
		Urho3D::RandomAccessIterator<T> Begin(Urho3D::PODVector<T> *v) {
			return v->Begin();
		}

		template <class T>
		Urho3D::RandomAccessConstIterator<T> Begin(const Urho3D::PODVector<T> &v) {
			return v.Begin();
		}
		template <class T>
		Urho3D::RandomAccessConstIterator<T> Begin(const Urho3D::PODVector<T> *v) {
			return v->Begin();
		}

		template <class T>
		Urho3D::RandomAccessIterator<T> End(Urho3D::PODVector<T> &v) {
			return v.End();
		}
		template <class T>
		Urho3D::RandomAccessIterator<T> End(Urho3D::PODVector<T> *v) {
			return v->End();
		}

		template <class T>
		Urho3D::RandomAccessConstIterator<T> End(const Urho3D::PODVector<T> &v) {
			return v.End();
		}
		template <class T>
		Urho3D::RandomAccessConstIterator<T> End(const Urho3D::PODVector<T> *v) {
			return v->End();
		}
	}
}

#define foreach(VAL, VALS) \
    if (const auto& _foreach_begin = FlagGG::Container::make_false_wrapper(FlagGG::Container::Begin(VALS))) { } else \
    if (const auto& _foreach_end = FlagGG::Container::make_false_wrapper(FlagGG::Container::End(VALS))) { } else \
    for (auto it = _foreach_begin.value; it != _foreach_end.value; ++it) \
    if (bool _foreach_flag = false) { } else \
    for (VAL = *it; !_foreach_flag; _foreach_flag = true)

#endif
