#pragma  once

#include "Container/Str.h"
#include "Config/AutomaticMachine.h"
#include "Config/LJSONValue.h"

namespace FlagGG
{

class FlagGG_API LJSONError
{
public:
	void DumpError(const Byte* bufferStart, const Byte* bufferEnd, const Byte* bufferIndex, const String& content);

	void SetFilePath(const String& filePath);

	void Dump();

	void Clear();

	bool HasError();

private:
	String filePath_;

	Vector<String> errors_;
};

// Like JSON Parser
class FlagGG_API LJSONParser
{
public:
	LJSONParser();

	virtual ~LJSONParser();

	bool Load(const char* buffer, UInt32 bufferSize, LJSONValue& value);

protected:
	bool StartAccept(const LJSONValue& parent, LJSONValue& node, const String& rootType);

	bool HasNext();

	bool AcceptComment();

	String AcceptType();

	bool AcceptKey(String& key);

	bool AcceptContent(String& content);

	bool AcceptValidChar(char& c);

	void Back();

	bool ToValue(const LJSONValue& parent, const String& type, const String& content, LJSONValue& value);

private:
	const Byte* bufferStart_;
	const Byte* bufferEnd_;
	const Byte* index_;

	AutomaticMachinePtr emptyState_;
	AutomaticMachinePtr commentState_;
	AutomaticMachinePtr keywordState_;
	AutomaticMachinePtr variableState_;
	AutomaticMachinePtr contentState_;

	LJSONError error_;
};

}


