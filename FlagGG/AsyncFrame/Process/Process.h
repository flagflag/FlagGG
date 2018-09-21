#ifndef __PROCESS__
#define __PROCESS__

#include "AsyncFrame\KernelObject\HandleObject.h"
#include "AsyncFrame\KernelObject\Runtime.h"

#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

#include <windows.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Process
		{
			class ParameterList
			{
			public:
				ParameterList();

				virtual ~ParameterList();

				//--------------------------------- utf-16 -----------------------------

				void add(const std::wstring& param_name);

				void add(const std::wstring& param_name, std::wstring& param);

				void add(const std::wstring& param_name, int32_t param);

				void add(const std::wstring& param_name, int64_t param);

				void add(const std::wstring& param_name, double param);

				void add(const std::wstring& param_name, long double param);

				//------------------------------------ utf-8 -----------------------------

				void add(const std::string& param_name);

				void add(const std::string& param_name, std::string& param);

				void add(const std::string& param_name, int32_t param);

				void add(const std::string& param_name, int64_t param);

				void add(const std::string& param_name, double param);

				void add(const std::string& param_name, long double param);

				friend class Builder;

			private:
				std::vector < std::wstring > m_list;
			};

			class ProcessObject : public KernelObject::HandleObject, public KernelObject::Runtime
			{
			public:
				ProcessObject();

				virtual ~ProcessObject();

				DWORD getID();

				virtual void stop() override;

				virtual void waitForStop() override;

				virtual void waitForStop(DWORD wait_time) override;

				friend class Builder;

			private:
				DWORD m_id;
			};

			typedef std::shared_ptr < ProcessObject > ProcessObjectPtr;

			class Builder
			{
			public:
				//--------------------------------- utf-16 ------------------------------------------

				static ProcessObjectPtr createProcess(const std::wstring& proc_path);

				static ProcessObjectPtr createProcess(const std::wstring& proc_path, const std::wstring& param);

				static ProcessObjectPtr createProcess(const std::wstring& proc_path, const ParameterList& param);

				//------------------------------------ utf-8 ----------------------------------------

				static ProcessObjectPtr createProcess(const std::string& proc_path);

				static ProcessObjectPtr createProcess(const std::string& proc_path, const std::string& param);

				static ProcessObjectPtr createProcess(const std::string& proc_path, const ParameterList& param);
			};
		}
	}
}

#endif