#pragma once

#include <type_traits>
#include <typeindex>
#include <algorithm>

#include "TypeTraits/FunctionTraits.h"
#include "Container/Str.h"
#include "Container/StringHash.h"
#include "Container/Swap.h"
#include "Container/HashMap.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"
#include "Math/Rect.h"
#include "Math/BoundingBox.h"
#include "Math/Ray.h"
#include "Math/Color.h"

#include "Log.h"

namespace FlagGG
{

template < class T, class ... Args >
struct MaxSize : std::integral_constant<int, (sizeof(T) > MaxSize<Args...>::value ? sizeof(T) : MaxSize<Args...>::value)>
{};

template < class T >
struct MaxSize<T> : std::integral_constant<int, sizeof(T)>
{};

template < class T, class ... Args >
struct MaxAlign : std::integral_constant<int, (std::alignment_of<T>::value > MaxAlign<Args...>::value ? std::alignment_of<T>::value : MaxAlign<Args...>::value)>
{};

template < class T >
struct MaxAlign<T> : std::integral_constant<int, std::alignment_of<T>::value>
{};

template < class T, class ... Args >
struct Contains : std::true_type
{};

template < class T, class First, class ... Rest >
struct Contains<T, First, Rest...> : std::conditional<std::is_same<T, First>::value, std::true_type, Contains<T, Rest...>>::type
{};

template < class T >
struct Contains<T> : std::false_type
{};

template < class ... Args >
struct VariantHelper
{};

template < class T, class ... Args >
struct VariantHelper<T, Args...>
{
	inline static void Destroy(std::type_index id, void* data)
	{
		if (id == std::type_index(typeid(T)))
			((T*)(data))->~T();
		else
			VariantHelper<Args...>::Destroy(id, data);
	}

	inline static  void Move(std::type_index id, void* srcData, void* dstData)
	{
		if (id == std::type_index(typeid(T)))
			new (dstData) T(std::move(*(T*)srcData));
		else
			VariantHelper<Args...>::Move(id, srcData, dstData);
	}

	inline static void Copy(std::type_index id, const void* srcData, void* dstData)
	{
		if (id == std::type_index(typeid(T)))
			new (dstData) T(*(const T*)srcData);
		else
			VariantHelper<Args...>::Copy(id, srcData, dstData);
	}
};

template <>
struct VariantHelper<>
{
	inline static void Destroy(std::type_index id, void* data) {}
	inline static  void Move(std::type_index id, void* srcData, void* dstData) {}
	inline static void Copy(std::type_index id, const void* srcData, void* dstData) {}
};

template < class ... Args >
class Variant
{
public:
	Variant() :
		id_(typeid(int)),
		data_{ 0 }
	{}

	Variant(Variant<Args...>&& rhs) :
		id_(rhs.id_)
	{
		VariantHelper<Args...>::Move(rhs.id_, &rhs.data_, &data_);
	}

	Variant(const Variant<Args...>& rhs) :
		id_(rhs.id_)
	{
		VariantHelper<Args...>::Copy(rhs.id_, &rhs.data_, &data_);
	}

	template < class T, class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Args...>::value>::type >
	Variant(T&& value) :
		id_(typeid(int)),
		data_{ 0 }
	{
		Set(std::forward<T>(value));
	}

	virtual ~Variant()
	{
		VariantHelper<Args...>::Destroy(id_, &data_);
	}

	template < class T >
	inline bool Is() const
	{
		return id_ == std::type_index(typeid(T));
	}

	template < class T >
	typename std::decay<T>::type& Get()
	{
		using U = typename std::decay<T>::type;
		if (!Is<U>())
		{
			FLAGGG_LOG_ERROR("Type[{}] is not same to current type", typeid(U).name());
			throw std::bad_cast();
		}
		return *(U*)&data_;
	}

	template < class T >
	const typename std::decay<T>::type& Get() const
	{
		using U = typename std::decay<T>::type;
		if (!Is<U>())
		{
			FLAGGG_LOG_ERROR("Type[{}] is not same to current type", typeid(U).name());
			throw std::bad_cast();
		}
		return *(U*)&data_;
	}

	template < class T >
	typename std::decay<T>::type& TryGet()
	{
		using U = typename std::decay<T>::type;
		if (!Is<U>())
		{
			return typename std::decay<T>::type();
		}
		return *(U*)&data_;
	}

	template < class T >
	const typename std::decay<T>::type& TryGet() const
	{
		using U = typename std::decay<T>::type;
		if (!Is<U>())
		{
			return typename std::decay<T>::type();
		}
		return *(U*)&data_;
	}

	template < class T, class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Args...>::value>::type >
	void Set(T&& value)
	{
		using U = typename std::decay<T>::type;

		if (!Is<U>())
		{
			VariantHelper<Args...>::Destroy(id_, &data_);
			id_ = std::type_index(typeid(U));
		}

		new (&data_) U(value);
	}

	Variant<Args...>& operator=(Variant<Args...>&& rhs)
	{
		id_ = rhs.id_;
		VariantHelper<Args...>::Move(rhs.id_, &rhs.data_, &data_);
		return *this;
	}

	Variant<Args...>& operator=(const Variant<Args...>& rhs)
	{
		id_ = rhs.id_;
		VariantHelper<Args...>::Copy(rhs.id_, &rhs.data_, &data_);
		return *this;
	}

	template < class T, class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Args...>::value>::type >
	Variant<Args...>& operator=(T&& value)
	{
		Set(std::forward<T>(value));
		return *this;
	}

	template < class F >
	void Visit(F&& f)
	{
		using T = typename FunctionTraits<typename std::remove_reference<F>::type>::template Argument<0>::Type;
		if (Is<T>())
			f(Get<T>());
	}

	template < class F, class ... Rest >
	void Visit(F&& f, Rest&& ... rest)
	{
		using T = typename FunctionTraits<typename std::remove_reference<F>::type>::template Argument<0>::Type;
		if (Is<T>())
			f(Get<T>());
		else
			Visit(std::forward<Rest>(rest)...);
	}

private:
	using DataType = typename std::aligned_storage<MaxSize<Args...>::value, MaxAlign<Args...>::value>::type;
	std::type_index id_;
	DataType data_;
};

typedef Variant<
	char,
	unsigned char,
	short,
	unsigned short,
	int,
	unsigned,
	long,
	unsigned long,
	long long,
	unsigned long long,
	float,
	double,
	void*,
	String,
	Vector2,
	Vector3,
	Vector4,
	Quaternion,
	Matrix3,
	Matrix3x4,
	Matrix4,
	Rect,
	BoundingBox,
	Ray,
	Color
> FVariant;

using FVariantMap = HashMap<StringHash, FVariant>;

}
