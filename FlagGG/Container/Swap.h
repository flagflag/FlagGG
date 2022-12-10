#pragma once

#include "Export.h"

namespace FlagGG
{
class HashBase;
class ListBase;
class String;
class VectorBase;

/// Swap two values.
template <class T> inline void Swap(T& first, T& second)
{
	T temp = first;
	first = second;
	second = temp;
}

template <> FlagGG_API void Swap<String>(String& first, String& second);
template <> FlagGG_API void Swap<VectorBase>(VectorBase& first, VectorBase& second);
template <> FlagGG_API void Swap<ListBase>(ListBase& first, ListBase& second);
template <> FlagGG_API void Swap<HashBase>(HashBase& first, HashBase& second);

}
