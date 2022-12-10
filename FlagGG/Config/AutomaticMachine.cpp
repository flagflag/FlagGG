#include "Config/AutomaticMachine.h"

#include <string.h>

namespace FlagGG
{

#define AM_SET_EMPTY " \t\r\n"
#define AM_SET_EMPTY_NO_SPACE "\t\r\n"
#define AM_SET_ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define AM_SET_DIGIT "0123456789"
#define AM_SET_ILLEGAL "~!@#$%^&*?|\\/()<>[]{},;=\'\""
#define AM_MID "="

StateNode::StateNode()
{
	memset(next, 0, sizeof(next));
}

void AutomaticMachine::LinkSet(StatePtr state1, StatePtr state2, const char* set)
{
	size_t size = strlen(set);
	for (size_t i = 0; i < size; ++i)
	{
		state1->next[set[i]] = state2;
	}
}

void AutomaticMachine::LinkNoneSet(StatePtr state1, StatePtr state2, const char* set)
{
	LinkSet(state1, state2, set);
	for (size_t i = 0; i < CHAR_COUNT; ++i)
	{
		state1->next[i] = state1->next[i] == state2 ? nullptr : state2;
	}
}

void AutomaticMachine::LinkNoneSet(StatePtr state, const char* set)
{
	size_t size = strlen(set);
	for (size_t i = 0; i < size; ++i)
	{
		state->next[set[i]] = nullptr;
	}
}

#define __IGNORE__() \
	index = bufferStart; \
	StatePtr now = start_; \
	while (index < bufferEnd && now->next[*index] == start_) \
	{ \
		now = now->next[*index]; \
		++index; \
	}

#define __GET_TOKEN__(token) \
	const Byte* tokenStart = index; \
	while (index < bufferEnd && now->next[*index] && now->next[*index] != end_) \
	{ \
		now = now->next[*index]; \
		++index; \
	} \
	if (!now->next[*index]) return false; \
	token.Asign((const char*)tokenStart, index - tokenStart);

#define REMOVE_QUOTATION(__value__) \
	if (__value__.Length() >= 2 && __value__[0] == '\"' && __value__[__value__.Length() - 1] == '\"') \
	{ \
		__value__ = __value__.Substring(1, __value__.Length() - 2); \
	}

static void EscapeToken(String& token, bool remove)
{
	if (token.Length() > 0)
	{
		if (remove)
		{
			REMOVE_QUOTATION(token);
		}

		Int32 offset = 0;
		bool flag = false;
		for (UInt32 i = 0; i < token.Length(); ++i)
		{
			if (flag)
			{
				switch (token[i])
				{
				case '\\':
					token[i - offset] = '\\';
					break;

				case 'a':
					token[i - offset] = '\a';
					break;

				case 'b':
					token[i - offset] = '\b';
					break;

				case 'r':
					token[i - offset] = '\r';
					break;

				case 'n':
					token[i - offset] = '\n';
					break;

				case 't':
					token[i - offset] = '\t';
					break;

				case 'f':
					token[i - offset] = '\f';
					break;

				case 'v':
					token[i - offset] = '\v';
					break;

				case 'e':
					token[i - offset] = '\e';
					break;

				case '0':
					token[i - offset] = '\0';
					break;

				default:
					token[i - offset] = token[i];
					break;
				}

				flag = false;
			}
			else if (token[i] == '\\')
			{
				++offset;
				flag = true;
			}
			else if (offset > 0)
			{
				token[i - offset] = token[i];
			}
		}

		if (offset > 0)
		{
			token = token.Substring(0, token.Length() - offset);
		}
	}
}

EmptyAutomaticMachine::EmptyAutomaticMachine()
{
	start_ = &pool_[0];

	LinkSet(start_, start_, AM_SET_EMPTY);
}

bool EmptyAutomaticMachine::Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, String& token)
{
	__IGNORE__();

	return true;
}

static bool Same(const Byte* c1, const char* c2)
{
	return c1[0] == c2[0] && c1[1] == c2[1];
}

bool CommentAutomationMachine::Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, String& token)
{
	EmptyAutomaticMachine::Accapt(bufferStart, bufferEnd, index, token);

	if (index + 1 >= bufferEnd)
	{
		return false;
	}

	if ((*index) == '/' && (*index) == '/')
	{
		index += 2;
		const char* _start = (const char*)index;
		while (index < bufferEnd && (*index) != '\r' && (*index) != '\n') ++index;
		const char* _end = (const char*)index;

		if (_start && _end)
		{
			token = String(_start, _end - _start);
		}
	}

	return true;
}

TypeKeywordAutomaticMachine::TypeKeywordAutomaticMachine()
{
	start_ = &pool_[0];
	token_ = &pool_[1];
	end_ = &pool_[2];

	LinkSet(start_, start_, AM_SET_EMPTY);

	LinkSet(start_, token_, AM_SET_ALPHA);

	LinkSet(token_, token_, AM_SET_ALPHA AM_SET_DIGIT "_");

	LinkSet(token_, end_, AM_SET_EMPTY AM_MID "{},");

	keywords_.emplace_back("var");
	keywords_.emplace_back("bool");
	keywords_.emplace_back("int");
	keywords_.emplace_back("float");
	keywords_.emplace_back("string");
	keywords_.emplace_back("object");
	keywords_.emplace_back("array");
}

bool TypeKeywordAutomaticMachine::Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, String& token)
{
	__IGNORE__();

	__GET_TOKEN__(token);

	bool findIt = false;
	for (auto it = keywords_.begin(); it != keywords_.end(); ++it)
	{
		if ((*it) == token)
		{
			findIt = true;
			break;
		}
	}

	return findIt;
}

VariableAutomaticMachine::VariableAutomaticMachine()
{
	start_ = &pool_[0];
	token_ = &pool_[1];
	end_ = &pool_[2];
	quotationStart_ = &pool_[3];
	quotationEnd_ = &pool_[4];
	escapes_ = &pool_[5];

	LinkNoneSet(start_, token_, AM_SET_EMPTY AM_SET_ILLEGAL);
	LinkNoneSet(token_, token_, AM_SET_EMPTY AM_SET_ILLEGAL);

	LinkSet(start_, start_, AM_SET_EMPTY);

	LinkSet(start_, quotationStart_, "\"");
	LinkNoneSet(quotationStart_, quotationStart_, "\r\n\\");
	LinkSet(quotationStart_, escapes_, "\\");
	LinkNoneSet(escapes_, quotationStart_, "\r\n");
	LinkSet(quotationStart_, quotationEnd_, "\"");

	LinkSet(token_, end_, AM_SET_EMPTY AM_MID);
	LinkSet(quotationEnd_, end_, AM_SET_EMPTY AM_MID);
}

bool VariableAutomaticMachine::Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, String& token)
{
	__IGNORE__();

	__GET_TOKEN__(token);

	EscapeToken(token, true);

	return true;
}

ContentAutomaticMachine::ContentAutomaticMachine()
{
	start_ = &pool_[0];
	token_ = &pool_[1];
	end_ = &pool_[2];
	quotationStart_ = &pool_[3];
	quotationEnd_ = &pool_[4];
	escapes_ = &pool_[5];
	leftBracket_ = &pool_[6];
	rightBracket_ = &pool_[7];
	textStart_ = &pool_[8];
	textEnd_ = &pool_[9];
	textEscapes_ = &pool_[10];

	LinkNoneSet(start_, token_, AM_SET_EMPTY AM_SET_ILLEGAL);
	LinkNoneSet(token_, token_, AM_SET_EMPTY AM_SET_ILLEGAL);

	LinkSet(start_, start_, AM_SET_EMPTY);

	LinkSet(start_, quotationStart_, "\"");
	LinkNoneSet(quotationStart_, quotationStart_, "\r\n\\");
	LinkSet(quotationStart_, escapes_, "\\");
	LinkNoneSet(escapes_, quotationStart_, "\r\n");
	LinkSet(quotationStart_, quotationEnd_, "\"");

	LinkSet(start_, leftBracket_, "(");
	LinkSet(leftBracket_, textStart_, "\"");
	LinkNoneSet(textStart_, textStart_, "\\");
	LinkSet(textStart_, textEscapes_, "\\");
	LinkNoneSet(textEscapes_, textStart_, "");
	LinkSet(textStart_, textEnd_, "\"");
	LinkSet(textEnd_, rightBracket_, ")");

	LinkSet(token_, end_, AM_SET_EMPTY ";,}]");
	LinkSet(quotationEnd_, end_, AM_SET_EMPTY ";,}]");
	LinkSet(rightBracket_, end_, AM_SET_EMPTY ";,}]");
}

bool ContentAutomaticMachine::Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, String& token)
{
	__IGNORE__();

	__GET_TOKEN__(token);

	EscapeToken(token, false);

	return true;
}

}

