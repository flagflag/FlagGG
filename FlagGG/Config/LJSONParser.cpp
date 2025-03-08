#include "LJSONParser.h"
#include "Utility/Format.h"
#include "Log.h"

#include <ctype.h>

namespace FlagGG
{

void LJSONError::DumpError(const Byte* bufferStart, const Byte* bufferEnd, const Byte* bufferIndex, const String& content)
{
	Int32 lineCount = 1;
	for (const Byte* index = bufferStart; index < bufferEnd && index <= bufferIndex; ++index)
	{
		if ((*index) == '\n') ++lineCount;
	}

	errors_.Push(filePath_ + "(" + ToString("%d", lineCount) + "):" + content);
}

void LJSONError::SetFilePath(const String& filePath)
{
	filePath_ = filePath;
}

void LJSONError::Dump()
{
	for (UInt32 i = 0; i < errors_.Size(); ++i)
	{
		FLAGGG_LOG_ERROR(errors_[i].CString());
	}
}

void LJSONError::Clear()
{
	errors_.Clear();
}

bool LJSONError::HasError()
{
	return errors_.Size() > 0;
}

#define ERROR_STREAM(__content__) error_.DumpError(bufferStart_, bufferEnd_, index_, __content__)

LJSONParser::LJSONParser() :
	emptyState_(new EmptyAutomaticMachine()),
	commentState_(new CommentAutomationMachine()),
	keywordState_(new TypeKeywordAutomaticMachine()),
	variableState_(new VariableAutomaticMachine()),
	contentState_(new ContentAutomaticMachine())
{ }

LJSONParser::~LJSONParser()
{
	if (emptyState_)	{ delete emptyState_;	 emptyState_ = nullptr; }
	if (commentState_)	{ delete commentState_;  commentState_ = nullptr; }
	if (keywordState_)	{ delete keywordState_;  keywordState_ = nullptr; }
	if (variableState_) { delete variableState_; variableState_ = nullptr; }
	if (contentState_)	{ delete contentState_;  contentState_ = nullptr; }
}

bool LJSONParser::Load(const char* buffer, UInt32 bufferSize, LJSONValue& value)
{
	bufferStart_ = (const Byte*)buffer;
	bufferEnd_ = (const Byte*)buffer + bufferSize;
	index_ = bufferStart_;

	bool result = StartAccept(LJSONValue::EMPTY, value, "var");

	if (!result)
	{
		error_.Dump();
	}

	return result;
}

#define READ_COMMENT() while (AcceptComment())

bool LJSONParser::StartAccept(const LJSONValue& parent, LJSONValue& node, const String& rootType)
{
	// 注释
	READ_COMMENT();

	char ch;
	if (!AcceptValidChar(ch))
	{
		ERROR_STREAM("content is none.");

		return false;
	}

	if (ch == '{')
	{
		String key;
		for (bool symbol = false; HasNext(); symbol = true)
		{
			READ_COMMENT();

			if (AcceptValidChar(ch) && ch == '}')
			{
				break;
			}

			Back();

			if (symbol && (!AcceptValidChar(ch) || ch != ','))
			{
				ERROR_STREAM("lack separation symbol.");

				return false;
			}

			READ_COMMENT();

			if (!AcceptKey(key))
			{
				ERROR_STREAM("cannot accept key.");

				return false;
			}

			READ_COMMENT();

			if (!AcceptValidChar(ch) || !IsMidSign(ch))
			{
				ERROR_STREAM("lack middle sign.");

				return false;
			}

			LJSONValue& value = node[key];
			if (!StartAccept(node, value, "var"))
			{
				return false;
			}
		}
	}
	else if (ch == '[')
	{
		for (bool symbol = false; HasNext(); symbol = true)
		{
			READ_COMMENT();

			if (AcceptValidChar(ch) && ch == ']')
			{
				break;
			}

			Back();

			if (symbol && (!AcceptValidChar(ch) || ch != ','))
			{
				ERROR_STREAM("lack separation symbol.");

				return false;
			}

			READ_COMMENT();

			LJSONValue& value = node.Append();
			if (!StartAccept(node, value, "var"))
			{
				return false;
			}
		}
	}
	else
	{
		Back();

		String content;
		if (!AcceptContent(content))
		{
			ERROR_STREAM("cannot accept content.");

			return false;
		}

		if (!ToValue(parent, rootType, content, node))
		{
			ERROR_STREAM("content is illegal.");

			return false;
		}
	}

	READ_COMMENT();

	return true;
}

bool LJSONParser::HasNext()
{
	static String none;
	emptyState_->Accapt(index_, bufferEnd_, index_, none);
	return index_ < bufferEnd_;
}

bool LJSONParser::AcceptComment()
{
	String comment;
	const Byte* cacheIndex = index_;
	bool result = commentState_->Accapt(index_, bufferEnd_, index_, comment) && !comment.Empty();
	if (!result) index_ = cacheIndex;
	return result;
}

String LJSONParser::AcceptType()
{
	String type;
	const Byte* cacheIndex = index_;
	bool result = keywordState_->Accapt(index_, bufferEnd_, index_, type);
	if (!result)
	{
		index_ = cacheIndex;
		type = "var";
	}
	return std::move(type);
}

bool LJSONParser::AcceptKey(String& key)
{
	const Byte* cacheIndex = index_;
	bool result = variableState_->Accapt(index_, bufferEnd_, index_, key);
	if (!result) index_ = cacheIndex;
	return result;
}

bool LJSONParser::AcceptContent(String& content)
{
	const Byte* cacheIndex = index_;
	bool result = contentState_->Accapt(index_, bufferEnd_, index_, content);
	if (!result) index_ = cacheIndex;
	return result;
}

bool LJSONParser::AcceptValidChar(char& c)
{
	static String none;
	bool result = emptyState_->Accapt(index_, bufferEnd_, index_, none);
	if (result)
	{
		c = (*index_++);
	}
	return result;
}

bool LJSONParser::IsMidSign(char ch)
{
	return ch == '=' || ch == ':';
}

void LJSONParser::Back()
{
	--index_;
}

bool LJSONParser::ToValue(const LJSONValue& parent, const String& type, const String& content, LJSONValue& value)
{
	if (content.Length() >= 2 && content[0] == '(' && content.Back() == ')')
	{
		value = content.Substring(2, content.Length() - 4);

		return true;
	}

	if (content.Length() >= 2 && content[0] == '\"' && content.Back() == '\"')
	{
		if (content.Length() > 2 && content[1] == '@')
		{
			String temp = content.Substring(2, content.Length() - 3);
			Vector<String> path = temp.Split('.', false);

			const LJSONValue* index = &parent;
			for (size_t i = 0; i < path.Size(); ++i)
			{
				if (index && index->IsObject() && index->Contains(path[i]))
				{
					index = &(*index)[path[i]];
				}
				else
				{
					index = nullptr;
					break;
				}
			}

			if (index)
			{
				value = *index;
			}
		}
		else
		{
			value = content.Substring(1, content.Length() - 2);
		}

		return true;
	}

	if (content == "true")
	{
		value = true;

		return true;
	}

	if (content == "false")
	{
		value = false;

		return true;
	}

	bool hasNonDig = false;
	bool appearDot = false;
	Int32 indexE = -1;

	for (UInt32 i = 0; i < content.Length(); ++i)
	{
		if (!isdigit(content[i]))
		{
			if (content[i] == '-' && indexE + 1 == i)
			{
				continue;
			}

			if (!appearDot && content[i] == '.')
			{
				appearDot = true;
			}
			else if (indexE == -1 && content[i] == 'e')
			{
				indexE = i;
			}
			else
			{
				hasNonDig = true;
				break;
			}
		}
	}

	if (!hasNonDig)
	{
		value = ToDouble(content);

		return true;
	}

	ERROR_STREAM("content is illegal.");

	return false;
}

}

