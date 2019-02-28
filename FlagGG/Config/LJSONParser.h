#pragma  once

#include "Config/AutomaticMachine.h"
#include "Config/LJSONValue.h"

namespace FlagGG
{
	namespace Config
	{
		class LJSONError
		{
		public:
			void DumpError(const Byte* bufferStart, const Byte* bufferEnd, const Byte* bufferIndex, const std::string& content);

			void SetFilePath(const std::string& filePath);

			void Dump();

			void Clear();

			bool HasError();

		private:
			std::string filePath_;

			std::vector<std::string> errors_;
		};

		// Like JSON Parser
		class LJSONParser
		{
		public:
			LJSONParser();

			virtual ~LJSONParser();

			bool Load(const char* buffer, uint32_t bufferSize, LJSONValue& value);

		protected:
			bool StartAccept(const LJSONValue& parent, LJSONValue& node, const std::string& rootType);

			bool HasNext();

			bool AcceptComment();

			std::string AcceptType();

			bool AcceptKey(std::string& key);

			bool AcceptContent(std::string& content);

			bool AcceptValidChar(char& c);

			void Back();

			bool ToValue(const LJSONValue& parent, const std::string& type, const std::string& content, LJSONValue& value);

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


