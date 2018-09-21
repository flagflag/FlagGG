#ifndef __PARSE__
#define __PARSE__

#include <string>
#include <memory>
#include <set>

namespace FlagGG
{
	namespace Config
	{
		struct StateTransition;
		typedef std::shared_ptr < StateTransition > StateTransitionPtr;
		struct StateTransition
		{
			StateTransitionPtr next[256];
		};

#define newState StateTransitionPtr(new StateTransition)

		class CustomParser
		{
		public:
			CustomParser();

			virtual ~CustomParser();

			//初始化自动机
			void initialize();

			void start(const char* buffer, size_t buffer_size);

			bool hasNext();

			bool acceptType(std::string& type);

			bool acceptKey(std::string& key);

			bool acceptContent(std::string& content);

			bool acceptMiddle();

			bool acceptLeftBracket();

			bool acceptRightBracket();

			bool acceptEnd(char& end_sign);

			void catchErrorPosition();

			size_t getErrorLineNumber();

			size_t getErrorColNumber();

		private:
			const unsigned char* m_buffer_start;
			const unsigned char* m_buffer_end;
			const unsigned char* m_index;

			StateTransitionPtr m_now;

			StateTransitionPtr m_root;

			StateTransitionPtr m_type_start;

			StateTransitionPtr m_type_end;

			StateTransitionPtr m_key_start;

			StateTransitionPtr m_key_end;

			StateTransitionPtr m_middle;

			StateTransitionPtr m_left_bracket;

			StateTransitionPtr m_right_bracket;

			StateTransitionPtr m_empty;

			StateTransitionPtr m_content_start;

			StateTransitionPtr m_content_end;

			StateTransitionPtr m_all_end;

			StateTransitionPtr m_error;

			int m_line_number;
			int m_col_number;
		};

		class Reader
		{
		public:
			Reader();

			bool load(const std::wstring& file_path);

			std::string getErrorMessage();

		protected:
			bool startAccept(int level);

		private:
			CustomParser m_parser;

			std::string m_error_info;
		};
	}
}

#endif