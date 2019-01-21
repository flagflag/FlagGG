#include "parse.h"
#include "Code/Code.h"
#include <fstream>

namespace FlagGG
{
	namespace Config
	{
		StateTransitionPtr makeShared()
		{
			return StateTransitionPtr(new StateTransition);
		}

		CustomParser::CustomParser()
		{ }

		CustomParser::~CustomParser()
		{ }

		static void linkEmpty(StateTransitionPtr from, StateTransitionPtr to)
		{
			from->next[' '] = to;
			from->next['\t'] = to;
			from->next['\r'] = to;
			from->next['\n'] = to;
			from->next['\0'] = to;
		}

		static void linkAlpha(StateTransitionPtr from, StateTransitionPtr to)
		{
			for (int i = 'a'; i <= 'z'; ++i)
			{
				from->next[i] = to;
			}
			for (int i = 'A'; i <= 'Z'; ++i)
			{
				from->next[i] = to;
			}
			from->next['_'] = to;
		}

		static void linkDigit(StateTransitionPtr from, StateTransitionPtr to)
		{
			for (int i = '0'; i <= '9'; ++i)
			{
				from->next[i] = to;
			}
		}

		static void link(StateTransitionPtr from, StateTransitionPtr to, char sign)
		{
			from->next[sign] = to;
		}

		void CustomParser::initialize()
		{
			m_root = newState;
			m_type_start = newState;
			m_type_end = newState;
			m_key_start = newState;
			m_key_end = newState;
			m_middle = newState;
			m_content_start = newState;
			m_content_end = newState;
			m_all_end = newState;
			m_left_bracket = newState;
			m_right_bracket = newState;
			m_empty = newState;
			m_error = newState;

			linkEmpty(m_root, m_root);

			linkAlpha(m_root, m_type_start);
			linkAlpha(m_type_start, m_type_start);
			linkEmpty(m_type_start, m_type_end);
			linkEmpty(m_type_end, m_type_end);

			linkAlpha(m_type_end, m_key_start);
			linkDigit(m_type_end, m_key_start);
			linkAlpha(m_key_start, m_key_start);
			linkDigit(m_key_start, m_key_start);
			linkEmpty(m_key_start, m_key_end);
			linkEmpty(m_key_end, m_key_end);

			link(m_key_start, m_middle, '=');
			link(m_key_end, m_middle, '=');
			linkEmpty(m_middle, m_middle);

			linkAlpha(m_middle, m_content_start);
			linkDigit(m_middle, m_content_start);
			linkAlpha(m_content_start, m_content_start);
			linkDigit(m_content_start, m_content_start);
			linkEmpty(m_content_start, m_content_end);
			linkEmpty(m_content_end, m_content_end);

			link(m_content_start, m_all_end, ';');
			link(m_content_end, m_all_end, ';');

			link(m_middle, m_left_bracket, '{');
			linkEmpty(m_left_bracket, m_left_bracket);
			linkAlpha(m_left_bracket, m_content_start);
			linkDigit(m_left_bracket, m_content_start);

			link(m_content_start, m_all_end, ',');
			link(m_content_end, m_all_end, ',');

			link(m_content_start, m_right_bracket, '}');
			link(m_content_end, m_right_bracket, '}');
			linkEmpty(m_right_bracket, m_empty);
			linkEmpty(m_empty, m_empty);
			link(m_empty, m_right_bracket, '}');
			link(m_right_bracket, m_right_bracket, '}');
			link(m_right_bracket, m_all_end, ',');
			link(m_right_bracket, m_all_end, ';');

			linkEmpty(m_error, m_error);
		}

		void CustomParser::start(const char* buffer, size_t buffer_size)
		{
			m_buffer_start = (const unsigned char*)buffer;
			m_buffer_end = (const unsigned char*)buffer + buffer_size;
			m_index = m_buffer_start;

			m_line_number = 1;
			m_col_number = 0;
		}

#define POS_COUNT \
	{  \
		if ((*m_index) == '\n') \
		{ \
			++m_line_number; \
			m_col_number = 0; \
		} \
		else \
		{ \
			++m_col_number; \
		} \
	}

		bool CustomParser::hasNext()
		{
			m_now = m_root;

			return m_index + 1 < m_buffer_end;
		}

		bool CustomParser::acceptType(std::string& type)
		{
			type = "";
			while (m_index < m_buffer_end && m_now && m_now != m_type_end && m_now->next[*m_index])
			{
				m_now = m_now->next[*m_index];
				if (m_now == m_type_start)
				{
					type += (*m_index);
				}
				POS_COUNT;
				++m_index;
			}

			return m_now == m_type_end || (m_now == m_root && m_index >= m_buffer_end);
		}

		bool CustomParser::acceptKey(std::string& key)
		{
			key = "";
			while (m_index < m_buffer_end && m_now && m_now != m_key_end && m_now != m_middle && m_now->next[*m_index])
			{
				m_now = m_now->next[*m_index];
				if (m_now == m_key_start)
				{
					key += (*m_index);
				}
				POS_COUNT;
				++m_index;
			}

			return m_now == m_key_end || m_now == m_middle;
		}

		bool CustomParser::acceptContent(std::string& content)
		{
			content = "";
			while (m_index < m_buffer_end && m_now && m_now != m_content_end && m_now != m_all_end && m_now != m_right_bracket && m_now->next[*m_index])
			{
				m_now = m_now->next[*m_index];
				if (m_now == m_content_start)
				{
					content += (*m_index);
				}
				POS_COUNT;
				++m_index;
			}

			return (m_now == m_content_start && m_index >= m_buffer_end) || m_now == m_content_end || m_now == m_all_end || m_now == m_right_bracket;
		}

		bool CustomParser::acceptMiddle()
		{
			while (m_index < m_buffer_end && m_now && m_now != m_middle)
			{
				m_now = m_now->next[*m_index];
				POS_COUNT;
				++m_index;
			}

			return m_now == m_middle;
		}

		bool CustomParser::acceptLeftBracket()
		{
			while (m_index < m_buffer_end && m_now && m_now != m_left_bracket && m_now->next[*m_index] != m_content_start)
			{
				m_now = m_now->next[*m_index];
				POS_COUNT;
				++m_index;
			}

			return m_now == m_left_bracket;
		}

		bool CustomParser::acceptRightBracket()
		{
			while (m_index < m_buffer_end && m_now && m_now != m_right_bracket)
			{
				m_now = m_now->next[*m_index];
				POS_COUNT;
				++m_index;
			}

			bool result = m_now == m_right_bracket;

			if (result)
			{
				m_now = m_now->next[*m_index];
				POS_COUNT;
				++m_index;
			}

			return result;
		}

		bool CustomParser::acceptEnd(char& end_sign)
		{
			while (m_index < m_buffer_end && m_now && m_now != m_all_end && m_now != m_right_bracket)
			{
				m_now = m_now->next[*m_index];
				POS_COUNT;
				++m_index;
			}

			if (m_now == m_all_end || m_now == m_right_bracket)
			{
				end_sign = *(m_index - 1);
			}

			return m_now == m_all_end || m_now == m_right_bracket;
		}

		void CustomParser::catchErrorPosition()
		{
			m_now = m_error;
			m_index -= 2;
			while (m_index >= m_buffer_start && m_now && m_now == m_error)
			{
				m_now = m_now->next[*m_index];
				if ((*m_index) == '\n')
				{
					--m_line_number;
				}
				--m_index;
			}

			m_col_number = 0;
			while (m_index >= m_buffer_start && (*m_index) != '\n')
			{
				++m_col_number;
				--m_index;
			}
		}

		size_t CustomParser::getErrorLineNumber()
		{
			return m_line_number;
		}

		size_t CustomParser::getErrorColNumber()
		{
			return m_col_number;
		}

		Reader::Reader()
		{
		}

		std::string Reader::getErrorMessage()
		{
			return m_error_info;
		}

		bool Reader::load(const std::wstring& file_path)
		{
			std::ifstream file_reader;
			std::string _file_path = Code::WideToAnsi(file_path);
			file_reader.open(_file_path, std::ios::in | std::ios::binary | std::ios::ate);
			if (!file_reader.is_open())
			{
				return false;
			}

			size_t file_size = file_reader.tellg();
			file_reader.seekg(0, std::ios::beg);

			char* buffer = new char[file_size + 1];
			file_reader.read(buffer, file_size);
			buffer[file_size] = '\0';

			m_parser.initialize();
			m_parser.start(buffer, file_size);

			return startAccept(0);
		}

#define ERROR_STREAM(error_message) \
	m_parser.catchErrorPosition(); \
	m_error_info.assign("(" + std::to_string(m_parser.getErrorLineNumber()) + "," + std::to_string(m_parser.getErrorColNumber()) + "): " + error_message);

		bool Reader::startAccept(int level)
		{
			while (m_parser.hasNext())
			{
				std::string type;
				if (m_parser.acceptType(type))
				{
					if (type.empty())
					{
						printf("\nend\n");
						return true;
					}

					if (type != "var")
					{
						ERROR_STREAM("illegal type");
						return false;
					}

					printf("%s ", type.c_str());

					std::string key;
					if (m_parser.acceptKey(key))
					{
						printf("%s ", key.c_str());

						if (m_parser.acceptMiddle())
						{
							printf("=");
							bool accept_ok = false;
						
							if (m_parser.acceptLeftBracket())
							{
								printf("\n{\n");
								accept_ok = startAccept(level + 1) && m_parser.acceptRightBracket();
								if (accept_ok)
									printf("\n}");
							}
							else
							{
								std::string content;
								accept_ok = m_parser.acceptContent(content);

								if (accept_ok)
									printf(" %s", content.c_str());
							}
							if (accept_ok)
							{
								char end_sign = '\0';
								if (m_parser.acceptEnd(end_sign))
								{
									if (level == 0)
									{
										if (end_sign == ';')
										{
											printf(";\n");
										}
										else
										{
											ERROR_STREAM("end sign error");
											return false;
										}
									}
									else
									{
										if (end_sign == ',')
										{
											printf(",\n");
										}
										else if (end_sign == '}')
										{
											return true;
										}
										else
										{
											ERROR_STREAM("end sign error");
											return false;
										}
									}
								}
								else
								{
									ERROR_STREAM("missing end sign");
									return false;
								}
							}
							else
							{
								ERROR_STREAM("illegal content");
								return false;
							}
						}
						else
						{
							ERROR_STREAM("missing middle sign");
							return false;
						}
					}
					else
					{
						ERROR_STREAM("illegal key");
						return false;
					}
				}
				else
				{
					ERROR_STREAM("illegal type");
					return false;
				}
			}
		}
	}
}
