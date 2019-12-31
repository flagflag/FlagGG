#pragma once

#include <type_traits>
#include <typeindex>
#include <algorithm>

#include "Container/Str.h"
#include "Container/StringHash.h"
#include "Container/Swap.h"
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
	namespace Container
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

			inline static void Set(std::type_index id, const void* srcData, void* dstData)
			{
				if (id == std::type_index(typeid(T)))
					new (dstData) T(*(const T*)srcData);
				else
					VariantHelper<Args...>::Set(id, srcData, dstData);
			}
		};

		template <>
		struct VariantHelper<>
		{
			inline static void Destroy(std::type_index id, void* data) {}
			inline static void Set(std::type_index id, const void* srcData, void* dstData) {}
		};

		template < class ... Args >
		class Variant
		{
		public:
			Variant() :
				id_(typeid(int)),
				data_{ 0 }
			{}

			template < class T >
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
			inline bool Is()
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
			void Set(T&& value)
			{
				using U = typename std::decay<T>::type;

				static_assert(Contains<U, Args...>::value || std::is_same<U, Variant<Args...>>::value, "Type is not expired.");

				if (std::is_same<T, Variant<Args...>>::value)
				{
					Swap(std::forward<Variant<Args...>&&>(value));
					return;
				}

				if (std::is_same<T, Variant<Args...>&>::value)
				{
					SetEntry(std::forward<T>(value));
					return;
				}

				if (!Is<U>())
				{
					VariantHelper<Args...>::Destroy(id_, &data_);
					id_ = std::type_index(typeid(U));
				}

				new (&data_) U(value);
			}

			void Swap(Variant<Args...>&& rhs)
			{
				FlagGG::Container::Swap(id_, rhs.id_);
				FlagGG::Container::Swap(data_, rhs.data_);
			}

			void SetEntry(const Variant<Args...>& rhs)
			{
				id_ = rhs.id_;
				VariantHelper<Args...>::Set(rhs.id_, &rhs.data_, &data_);
			}

			template < class T >
			Variant<Args...>& operator=(T&& value)
			{
				Set(std::forward<T>(value));
				return *this;
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
			String,
			Math::Vector2,
			Math::Vector3,
			Math::Vector4,
			Math::Quaternion,
			Math::Matrix3,
			Math::Matrix3x4,
			Math::Matrix4,
			Math::Rect,
			Math::BoundingBox,
			Math::Ray,
			Math::Color
		> DefaultVariant;
	}
}
