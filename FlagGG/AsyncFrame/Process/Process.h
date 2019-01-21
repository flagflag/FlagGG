#ifndef __PROCESS__
#define __PROCESS__

#include "Export.h"

#include "AsyncFrame/KernelObject/HandleObject.h"
#include "AsyncFrame/KernelObject/Runtime.h"

#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Process
		{
			class FlagGG_API ParameterList
			{
			public:
				ParameterList();

				virtual ~ParameterList();

				//--------------------------------- utf-16 -----------------------------

				void add(const std::wstring& param_name);

				void add(const std::wstring& param_name, const std::wstring& param);

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

			class FlagGG_API ProcessObject : public KernelObject::HandleObject, public KernelObject::Runtime
			{
			public:
				ProcessObject();

				~ProcessObject() override = default;

				uint32_t getID();

				void stop() override;

				void waitForStop() override;

				void waitForStop(uint32_t wait_time) override;

				friend class Builder;

			private:
				uint32_t m_id;
			};

			typedef std::shared_ptr < ProcessObject > ProcessObjectPtr;

			class FlagGG_API Builder
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