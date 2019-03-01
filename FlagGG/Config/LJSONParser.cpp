#include "LJSONParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <ctype.h>

namespace FlagGG
{
	namespace Config
	{
		void LJSONError::DumpError(const Byte* bufferStart, const Byte* bufferEnd, const Byte* bufferIndex, const std::string& content)
		{
			int32_t lineCount = 1;
			for (const Byte* index = bufferStart; index < bufferEnd && index <= bufferIndex; ++index)
			{
				if ((*index) == '\n') ++lineCount;
			}

			errors_.emplace_back(filePath_ + "(" + std::to_string(lineCount) + "):" + content);
		}

		void LJSONError::SetFilePath(const std::string& filePath)
		{
			filePath_ = filePath;
		}

		void LJSONError::Dump()
		{
			for (uint32_t i = 0; i < errors_.size(); ++i)
			{
				puts(errors_[i].c_str());
			}
		}

		void LJSONError::Clear()
		{
			errors_.clear();
		}

		bool LJSONError::HasError()
		{
			return errors_.size() > 0;
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

		bool LJSONParser::Load(const char* buffer, uint32_t bufferSize, LJSONValue& value)
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

		bool LJSONParser::StartAccept(const LJSONValue& parent, LJSONValue& node, const std::string& rootType)
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
				std::string key;
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

					if (!AcceptValidChar(ch) || ch != AM_MID_SIGN)
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

				std::string content;
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
			static std::string none;
			emptyState_->Accapt(index_, bufferEnd_, index_, none);
			return index_ < bufferEnd_;
		}

		bool LJSONParser::AcceptComment()
		{
			std::string comment;
			const Byte* cacheIndex = index_;
			bool result = commentState_->Accapt(index_, bufferEnd_, index_, comment) && !comment.empty();
			if (!result) index_ = cacheIndex;
			return result;
		}

		std::string LJSONParser::AcceptType()
		{
			std::string type;
			const Byte* cacheIndex = index_;
			bool result = keywordState_->Accapt(index_, bufferEnd_, index_, type);
			if (!result)
			{
				index_ = cacheIndex;
				type = "var";
			}
			return std::move(type);
		}

		bool LJSONParser::AcceptKey(std::string& key)
		{
			const Byte* cacheIndex = index_;
			bool result = variableState_->Accapt(index_, bufferEnd_, index_, key);
			if (!result) index_ = cacheIndex;
			return result;
		}

		bool LJSONParser::AcceptContent(std::string& content)
		{
			const Byte* cacheIndex = index_;
			bool result = contentState_->Accapt(index_, bufferEnd_, index_, content);
			if (!result) index_ = cacheIndex;
			return result;
		}

		bool LJSONParser::AcceptValidChar(char& c)
		{
			static std::string none;
			bool result = emptyState_->Accapt(index_, bufferEnd_, index_, none);
			if (result)
			{
				c = (*index_++);
			}
			return result;
		}

		void LJSONParser::Back()
		{
			--index_;
		}

		bool LJSONParser::ToValue(const LJSONValue& parent, const std::string& type, const std::string& content, LJSONValue& value)
		{
			if (content.length() >= 2 && content[0] == '(' && content.back() == ')')
			{
				value = content.substr(2, content.length() - 4);

				return true;
			}

			if (content.length() >= 2 && content[0] == '\"' && content.back() == '\"')
			{
				if (content.length() > 2 && content[1] == '@')
				{
					std::string temp = content.substr(2, content.length() - 3);
					std::vector<std::string> path;
					boost::split(path, temp, boost::is_any_of("."));

					const LJSONValue* index = &parent;
					for (size_t i = 0; i < path.size(); ++i)
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
					value = content.substr(1, content.length() - 2);
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

			for (uint32_t i = 0; i < content.length(); ++i)
			{
				if (!isdigit(content[i]))
				{
					if (i == 0 && content[i] == '-')
					{
						continue;
					}

					if (!appearDot && content[i] == '.')
					{
						appearDot = true;
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
				value = boost::lexical_cast<double>(content);

				return true;
			}

			ERROR_STREAM("content is illegal.");

			return false;
		}
	}
}

