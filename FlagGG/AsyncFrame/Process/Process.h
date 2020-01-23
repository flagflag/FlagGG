#ifndef __PROCESS__
#define __PROCESS__

#include "Export.h"

#include "AsyncFrame/KernelObject/HandleObject.h"
#include "AsyncFrame/KernelObject/Runtime.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Core/BaseTypes.h"

#include <string>
#include <vector>

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

				void Add(const std::wstring& param_name);

				void Add(const std::wstring& param_name, const std::wstring& param);

				void Add(const std::wstring& param_name, Int32 param);

				void Add(const std::wstring& param_name, Int64 param);

				void Add(const std::wstring& param_name, double param);

				void Add(const std::wstring& param_name, long double param);

				//------------------------------------ utf-8 -----------------------------

				void Add(const std::string& param_name);

				void Add(const std::string& param_name, std::string& param);

				void Add(const std::string& param_name, Int32 param);

				void Add(const std::string& param_name, Int64 param);

				void Add(const std::string& param_name, double param);

				void Add(const std::string& param_name, long double param);

				friend class Builder;

			private:
				std::vector < std::wstring > list_;
			};

			class FlagGG_API ProcessObject : public KernelObject::HandleObject, public KernelObject::Runtime, public Container::RefCounted
			{
			public:
				ProcessObject();

				~ProcessObject() override = default;

				UInt32 GetID();

				void Stop() override;

				void WaitForStop() override;

				void WaitForStop(UInt32 wait_time) override;

				friend class Builder;

			private:
				UInt32 id_;
			};

			typedef Container::SharedPtr < ProcessObject > ProcessObjectPtr;

			class FlagGG_API Builder
			{
			public:
				//--------------------------------- utf-16 ------------------------------------------

				static ProcessObjectPtr CreateProcess(const std::wstring& proc_path);

				static ProcessObjectPtr CreateProcess(const std::wstring& proc_path, const std::wstring& param);

				static ProcessObjectPtr CreateProcess(const std::wstring& proc_path, const ParameterList& param);

				//------------------------------------ utf-8 ----------------------------------------

				static ProcessObjectPtr CreateProcess(const std::string& proc_path);

				static ProcessObjectPtr CreateProcess(const std::string& proc_path, const std::string& param);

				static ProcessObjectPtr CreateProcess(const std::string& proc_path, const ParameterList& param);
			};
		}
	}
}

#endif