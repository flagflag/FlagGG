#ifndef __INTERPRETER__
#define __INTERPRETER__

#include <string>

namespace FlagGG
{
	namespace LScript
	{
		class LSInterpreter
		{
		public:
			void executeCode(const std::string& code);

			void executeFile(const std::string& file_path);

		private:

		};
	}
}

#endif