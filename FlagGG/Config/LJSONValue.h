#pragma once

#include "Export.h"

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

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
		using LJSONString = std::string;
		using LJSONArray = std::vector<LJSONValue>;
		using LJSONObject = std::map<std::string, LJSONValue>;

		// Like JSON Value
		class FlagGG_API LJSONValue
		{
		public:
			LJSONValue(LJSONValueType type = LJSON_NULL);
			LJSONValue(bool value);
			LJSONValue(int32_t value);
			LJSONValue(uint32_t value);
			LJSONValue(float value);
			LJSONValue(double value);
			LJSONValue(const char* value);
			LJSONValue(const LJSONString& value);
			LJSONValue(const LJSONArray& value);
			LJSONValue(const LJSONObject& value);
			LJSONValue(const LJSONValue& value);

			virtual ~LJSONValue();

			// 取值
			LJSONValue& operator[](uint32_t index);
			const LJSONValue& operator[](uint32_t index) const;

			LJSONValue& operator[](const std::string& key);
			const LJSONValue& operator[](const std::string& key) const;

			// 赋值
			LJSONValue& operator=(bool value);
			LJSONValue& operator=(int32_t value);
			LJSONValue& operator=(uint32_t value);
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
			const int32_t GetInt() const;
			const uint32_t GetUInt() const;
			double GetDouble() const;
			const LJSONString& GetString() const;
			const LJSONArray& GetArray() const;
			const LJSONObject& GetObject() const;

			// 数组操作
			LJSONValue& Append();

			// 其他
			bool Contains(const std::string& key) const;
			uint32_t Size() const;

			static const LJSONValue EMPTY;

		protected:
			void CheckType(LJSONValueType type);

			void SetType(LJSONValueType type);

			void Reset();

			static const LJSONString EMPTY_STRING;
			static const LJSONArray EMPTY_ARRAY;
			static const LJSONObject EMPTY_OBJECT;

		private:
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


