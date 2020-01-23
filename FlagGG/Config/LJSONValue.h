#pragma once

#include "Export.h"
#include "Container/Str.h"
#include "Container/Vector.h"
#include "Container/HashMap.h"
#include "Core/BaseTypes.h"

namespace FlagGG
{
	namespace Config
	{
		enum LJSONValueType
		{
			LJSON_NULL = 0,
			LJSON_BOOL,
			LJSON_NUMBER,
			LJSON_STRING,
			LJSON_ARRAY,
			LJSON_OBJECT,
			LJSON_REFERENCE,
		};

		class LJSONValue;

		using LJSONBool = bool;
		using LJSONNumber = double;
		using LJSONString = Container::String;
		using LJSONArray = Container::Vector<LJSONValue>;
		using LJSONObject = Container::HashMap<LJSONString, LJSONValue>;
		using LJSONIterator = LJSONObject::Iterator;
		using LJSONConstIterator = LJSONObject::ConstIterator;

		// Like JSON Value
		class FlagGG_API LJSONValue
		{
		public:
			LJSONValue(LJSONValueType type = LJSON_NULL);
			LJSONValue(bool value);
			LJSONValue(Int32 value);
			LJSONValue(UInt32 value);
			LJSONValue(float value);
			LJSONValue(double value);
			LJSONValue(const char* value);
			LJSONValue(const LJSONString& value);
			LJSONValue(const LJSONArray& value);
			LJSONValue(const LJSONObject& value);
			LJSONValue(const LJSONValue& value);

			virtual ~LJSONValue();

			// 取值
			LJSONValue& operator[](UInt32 index);
			const LJSONValue& operator[](UInt32 index) const;

			LJSONValue& operator[](const LJSONString& key);
			const LJSONValue& operator[](const LJSONString& key) const;

			// 赋值
			LJSONValue& operator=(bool value);
			LJSONValue& operator=(Int32 value);
			LJSONValue& operator=(UInt32 value);
			LJSONValue& operator=(float value);
			LJSONValue& operator=(double value);
			LJSONValue& operator=(const char* value);
			LJSONValue& operator=(const LJSONString& value);
			LJSONValue& operator=(const LJSONArray& value);
			LJSONValue& operator=(const LJSONObject& value);
			LJSONValue& operator=(const LJSONValue& value);

			// 判断值类型
			bool IsBool() const;
			bool IsNumber() const;
			bool IsString() const;
			bool IsArray() const;
			bool IsObject() const;

			// 获得具体值
			const bool GetBool() const;
			const Int32 GetInt() const;
			const UInt32 GetUInt() const;
			double GetDouble() const;
			const LJSONString& GetString() const;
			const LJSONArray& GetArray() const;
			const LJSONObject& GetObject() const;

			// 弱判断的Get方法
			const bool ToBool() const;
			const Int32 ToInt() const;
			const UInt32 ToUInt() const;
			const double ToDouble() const;

			// 数组操作
			LJSONValue& Append();

			// 其他
			bool Contains(const LJSONString& key) const;
			UInt32 Size() const;

			LJSONConstIterator Begin() const;
			LJSONConstIterator End() const;

			static const LJSONValue EMPTY;

		protected:
			void CheckType(LJSONValueType type);

			void SetType(LJSONValueType type);

			void Reset();

			static const LJSONString EMPTY_STRING;
			static const LJSONArray EMPTY_ARRAY;
			static const LJSONObject EMPTY_OBJECT;

			LJSONValueType type_;
			union
			{
				LJSONBool			boolValue_;
				LJSONNumber			numberValue_;
				LJSONString*		stringValue_;
				LJSONArray*			arrayValue_;
				LJSONObject*		objectValue_;
			};
		};
	}
}


