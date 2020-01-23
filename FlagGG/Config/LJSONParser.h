#pragma  once

#include "Container/Str.h"
#include "Config/AutomaticMachine.h"
#include "Config/LJSONValue.h"

namespace FlagGG
{
	namespace Config
	{
		class FlagGG_API LJSONError
		{
		public:
			void DumpError(const Byte* bufferStart, const Byte* bufferEnd, const Byte* bufferIndex, const Container::String& content);

			void SetFilePath(const Container::String& filePath);

			void Dump();

			void Clear();

			bool HasError();

		private:
			Container::String filePath_;

			Container::Vector<Container::String> errors_;
		};

		// Like JSON Parser
		class FlagGG_API LJSONParser
		{
		public:
			LJSONParser();

			virtual ~LJSONParser();

			bool Load(const char* buffer, UInt32 bufferSize, LJSONValue& value);

		protected:
			bool StartAccept(const LJSONValue& parent, LJSONValue& node, const Container::String& rootType);

			bool HasNext();

			bool AcceptComment();

			Container::String AcceptType();

			bool AcceptKey(Container::String& key);

			bool AcceptContent(Container::String& content);

			bool AcceptValidChar(char& c);

			void Back();

			bool ToValue(const LJSONValue& parent, const Container::String& type, const Container::String& content, LJSONValue& value);

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
}


