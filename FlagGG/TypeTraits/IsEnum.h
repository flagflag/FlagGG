#pragma once

namespace FlagGG
{

template <typename T>
struct TIsEnum
{
	enum { value = __is_enum(T) };
};

}
