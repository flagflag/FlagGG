#include "LJSONValue.h"
#include "Utility/Format.h"

namespace FlagGG
{
	namespace Config
	{
		const LJSONValue LJSONValue::EMPTY;
		const LJSONString LJSONValue::EMPTY_STRING;
		const LJSONArray LJSONValue::EMPTY_ARRAY;
		const LJSONObject LJSONValue::EMPTY_OBJECT;

		LJSONValue::LJSONValue(LJSONValueType type) :
			type_(LJSON_NULL)
		{
			CheckType(type_);
		}

		LJSONValue::LJSONValue(bool value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(Int32 value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(UInt32 value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(float value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(double value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(const char* value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(const LJSONString& value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(const LJSONArray& value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(const LJSONObject& value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::LJSONValue(const LJSONValue& value) :
			type_(LJSON_NULL)
		{
			(*this) = value;
		}

		LJSONValue::~LJSONValue()
		{
			Reset();
		}

		LJSONValue & LJSONValue::operator[](UInt32 index)
		{
			CheckType(LJSON_ARRAY);
			return (*arrayValue_)[index];
		}

		const LJSONValue& LJSONValue::operator[](UInt32 index) const
		{
			if (type_ != LJSON_ARRAY)
			{
				return EMPTY_ARRAY;
			}

			return (*arrayValue_)[index];
		}

		LJSONValue& LJSONValue::operator[](const LJSONString& key)
		{
			CheckType(LJSON_OBJECT);
			return (*objectValue_)[key];
		}

		const LJSONValue& LJSONValue::operator[](const LJSONString& key) const
		{
			if (type_ != LJSON_OBJECT)
			{
				return EMPTY_OBJECT;
			}

			return (*objectValue_)[key];
		}

		LJSONValue& LJSONValue::operator=(bool value)
		{
			CheckType(LJSON_BOOL);
			boolValue_ = value;

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(Int32 value)
		{
			CheckType(LJSON_NUMBER);
			numberValue_ = static_cast<double>(value);

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(UInt32 value)
		{
			CheckType(LJSON_NUMBER);
			numberValue_ = static_cast<double>(value);

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(float value)
		{
			CheckType(LJSON_NUMBER);
			numberValue_ = static_cast<double>(value);

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(double value)
		{
			CheckType(LJSON_NUMBER);
			numberValue_ = value;

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(const char* value)
		{
			CheckType(LJSON_STRING);
			(*stringValue_) = value;

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(const LJSONString& value)
		{
			CheckType(LJSON_STRING);
			(*stringValue_) = value;

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(const LJSONArray& value)
		{
			CheckType(LJSON_ARRAY);
			(*arrayValue_) = value;

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(const LJSONObject& value)
		{
			CheckType(LJSON_OBJECT);
			(*objectValue_) = value;

			return (*this);
		}

		LJSONValue& LJSONValue::operator=(const LJSONValue& value)
		{
			CheckType(value.type_);

			switch (value.type_)
			{
			case LJSON_BOOL:
				boolValue_ = value.boolValue_;
				break;

			case LJSON_NUMBER:
				numberValue_ = value.numberValue_;
				break;

			case LJSON_STRING:
				(*stringValue_) = (*value.stringValue_);
				break;

			case LJSON_ARRAY:
				(*arrayValue_) = (*value.arrayValue_);
				break;

			case LJSON_OBJECT:
				(*objectValue_) = (*value.objectValue_);
				break;
			}

			return (*this);
		}

		bool LJSONValue::IsBool() const
		{
			return type_ == LJSON_BOOL;
		}

		bool LJSONValue::IsNumber() const
		{
			return type_ == LJSON_NUMBER;
		}

		bool LJSONValue::IsString() const
		{
			return type_ == LJSON_STRING;
		}

		bool LJSONValue::IsArray() const
		{
			return type_ == LJSON_ARRAY;
		}

		bool LJSONValue::IsObject() const
		{
			return type_ == LJSON_OBJECT;
		}

		const bool LJSONValue::GetBool() const
		{
			return IsBool() ? boolValue_ : false;
		}

		const Int32 LJSONValue::GetInt() const
		{
			return IsNumber() ? static_cast<Int32>(numberValue_) : 0;
		}

		const UInt32 LJSONValue::GetUInt() const
		{
			return IsNumber() ? static_cast<UInt32>(numberValue_) : 0u;
		}

		double LJSONValue::GetDouble() const
		{
			return IsNumber() ? numberValue_ : 0.0f;
		}

		const LJSONString& LJSONValue::GetString() const
		{
			return IsString() ? (*stringValue_) : EMPTY_STRING;
		}

		const LJSONArray& LJSONValue::GetArray() const
		{
			return IsArray() ? (*arrayValue_) : EMPTY_ARRAY;
		}

		const LJSONObject& LJSONValue::GetObject() const
		{
			return IsObject() ? (*objectValue_) : EMPTY_OBJECT;
		}

		const bool LJSONValue::ToBool() const
		{
			return IsBool() ? boolValue_ : (IsString() && (*stringValue_) == "true" ? true : false);
		}

		const Int32 LJSONValue::ToInt() const
		{
			if (IsNumber())
			{
				return static_cast<Int32>(numberValue_);
			}

			return Utility::Format::ToInt(*stringValue_);
		}

		const UInt32 LJSONValue::ToUInt() const
		{
			if (IsNumber())
			{
				static_cast<UInt32>(numberValue_);
			}

			return Utility::Format::ToUInt(*stringValue_);
		}

		const double LJSONValue::ToDouble() const
		{
			if (IsNumber())
			{
				return numberValue_;
			}

			return Utility::Format::ToDouble(*stringValue_);
		}

		LJSONValue& LJSONValue::Append()
		{
			CheckType(LJSON_ARRAY);
			LJSONValue temp;
			arrayValue_->Push(temp);
			return arrayValue_->Back();
		}

		bool LJSONValue::Contains(const LJSONString& key) const
		{
			return type_ == LJSON_OBJECT && objectValue_->Contains(key);
		}

		UInt32 LJSONValue::Size() const
		{
			if (type_ == LJSON_ARRAY) return arrayValue_->Size();
			if (type_ == LJSON_OBJECT) return objectValue_->Size();
			return 0u;
		}

		LJSONConstIterator LJSONValue::Begin() const
		{
			return type_ == LJSON_OBJECT ? objectValue_->Begin() : EMPTY_OBJECT.Begin();
		}

		LJSONConstIterator LJSONValue::End() const
		{
			return type_ == LJSON_OBJECT ? objectValue_->End() : EMPTY_OBJECT.End();
		}

		void LJSONValue::CheckType(LJSONValueType type)
		{
			if (type_ != type)
			{
				Reset();

				SetType(type);
			}
		}

		void LJSONValue::SetType(LJSONValueType type)
		{
			switch (type)
			{
			case LJSON_STRING:
				stringValue_ = new LJSONString();
				break;

			case LJSON_ARRAY:
				arrayValue_ = new LJSONArray();
				break;

			case LJSON_OBJECT:
				objectValue_ = new LJSONObject();
				break;
			}

			type_ = type;
		}

		void LJSONValue::Reset()
		{
			switch (type_)
			{
			case LJSON_STRING:
				delete stringValue_;
				break;

			case LJSON_ARRAY:
				delete arrayValue_;
				break;

			case LJSON_OBJECT:
				delete objectValue_;
				break;
			}

			type_ = LJSON_NULL;
		}
	}
}

