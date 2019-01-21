#include "Process.h"
#include "Code/Code.h"
#include "Allocator/SmartMemory.hpp"

#if WIN32 || WIN64
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace FlagGG
{
	namespace AsyncFrame
	{
		namespace Process
		{
			ParameterList::ParameterList()
			{

			}

			ParameterList::~ParameterList()
			{

			}

			void ParameterList::add(const std::wstring& param_name)
			{
				m_list.emplace_back(param_name);
			}

			void ParameterList::add(const std::wstring& param_name, const std::wstring& param)
			{
				m_list.emplace_back(param_name + L"=" + param);
			}

			void ParameterList::add(const std::wstring& param_name, int32_t param)
			{
				m_list.emplace_back(param_name + L"=" + std::to_wstring(param));
			}

			void ParameterList::add(const std::wstring& param_name, int64_t param)
			{
				m_list.emplace_back(param_name + L"=" + std::to_wstring(param));
			}

			void ParameterList::add(const std::wstring& param_name, double param)
			{
				m_list.emplace_back(param_name + L"=" + std::to_wstring(param));
			}

			void ParameterList::add(const std::wstring& param_name, long double param)
			{
				m_list.emplace_back(param_name + L"=" + std::to_wstring(param));
			}

			void ParameterList::add(const std::string& param_name)
			{
				add(Code::Utf8ToWide(param_name));
			}

			void ParameterList::add(const std::string& param_name, std::string& param)
			{
				add(Code::Utf8ToWide(param_name), Code::Utf8ToWide(param));
			}

			void ParameterList::add(const std::string& param_name, int32_t param)
			{
				add(Code::Utf8ToWide(param_name), param);
			}

			void ParameterList::add(const std::string& param_name, int64_t param)
			{
				add(Code::Utf8ToWide(param_name), param);
			}

			void ParameterList::add(const std::string& param_name, double param)
			{
				add(Code::Utf8ToWide(param_name), param);
			}

			void ParameterList::add(const std::string& param_name, long double param)
			{
				add(Code::Utf8ToWide(param_name), param);
			}


			ProcessObject::ProcessObject()
				: m_id(0)
			{ }

			uint32_t ProcessObject::getID()
			{
				return m_id;
			}

			void ProcessObject::stop()
			{
#if WIN32 || WIN64
				TerminateProcess(m_handle, -1);
#else

#endif
			}

			void ProcessObject::waitForStop()
			{
#if WIN32 || WIN64
				WaitForSingleObject(m_handle, INFINITE);
#else
				waitpid(*((pid_t*)m_handle), nullptr, 0);
#endif
			}

			void ProcessObject::waitForStop(uint32_t wait_time)
			{
#if WIN32 || WIN64
				WaitForSingleObject(m_handle, wait_time);
#else

#endif
			}


			ProcessObjectPtr Builder::createProcess(const std::wstring& proc_path)
			{
				return createProcess(proc_path, L"");
			}

			ProcessObjectPtr Builder::createProcess(const std::wstring& proc_path, const std::wstring& param)
			{
#if WIN32 || WIN64
				STARTUPINFOW startup_info;
				ZeroMemory(&startup_info, sizeof startup_info);
				startup_info.cb = sizeof(startup_info);

				PROCESS_INFORMATION process_info;
				ZeroMemory(&process_info, sizeof process_info);

				Allocator::SmartMemory < wchar_t > mry(param.size() + 2);
				wchar_t* _param = mry.get();
				wsprintfW(_param, L" %s", param.c_str());

				if (!CreateProcessW(
					proc_path.c_str(), _param,
					nullptr, nullptr, FALSE, 0, nullptr, nullptr,
					&startup_info, &process_info))
				{
					return nullptr;
				}

				ProcessObjectPtr proc_obj = ProcessObjectPtr(new ProcessObject());
				proc_obj->m_handle = process_info.hProcess;
				proc_obj->m_id = process_info.dwProcessId;

				return proc_obj;
#else
				pid_t pid = fork();
				// something error!
				if (pid == -1)
				{
					exit(-1);
				}
				// child process
				else if (pid == 0)
				{
					std::string _proc_path = Code::WideToAnsi(proc_path);
					std::string _param = Code::WideToAnsi(param);

					if (0 != execl(_proc_path.c_str(), _param.c_str()))
					{
						exit(-1);
					}

					exit(0);
				}
				// main process
				else
				{
					ProcessObjectPtr proc_obj = ProcessObjectPtr(new ProcessObject());
					proc_obj->m_id = pid;
					proc_obj->m_handle = &(proc_obj->m_id);

					return proc_obj;
				}
#endif
			}

			ProcessObjectPtr Builder::createProcess(const std::wstring& proc_path, const ParameterList& param)
			{
				std::wstring _param = L"";

				for (size_t i = 0; i < param.m_list.size(); ++i)
				{
					if (i != 0) _param += L" ";
					_param += param.m_list[i];
				}

				return createProcess(proc_path, _param);
			}

			ProcessObjectPtr Builder::createProcess(const std::string& proc_path)
			{
				return createProcess(Code::Utf8ToWide(proc_path));
			}

			ProcessObjectPtr Builder::createProcess(const std::string& proc_path, const std::string& param)
			{
				return createProcess(Code::Utf8ToWide(proc_path), Code::Utf8ToWide(param));
			}

			ProcessObjectPtr Builder::createProcess(const std::string& proc_path, const ParameterList& param)
			{
				return createProcess(Code::Utf8ToWide(proc_path), param);
			}
		}
	}
}