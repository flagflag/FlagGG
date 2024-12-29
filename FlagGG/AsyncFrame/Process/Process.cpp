#include "Process.h"
#include "Code/Code.h"
#include "Allocator/SmartMemory.hpp"

#if PLATFORM_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace FlagGG
{

ParameterList::ParameterList()
{

}

ParameterList::~ParameterList()
{

}

void ParameterList::Add(const std::wstring& param_name)
{
	list_.emplace_back(param_name);
}

void ParameterList::Add(const std::wstring& param_name, const std::wstring& param)
{
	list_.emplace_back(param_name + L"=" + param);
}

void ParameterList::Add(const std::wstring& param_name, Int32 param)
{
	list_.emplace_back(param_name + L"=" + std::to_wstring(param));
}

void ParameterList::Add(const std::wstring& param_name, Int64 param)
{
	list_.emplace_back(param_name + L"=" + std::to_wstring(param));
}

void ParameterList::Add(const std::wstring& param_name, double param)
{
	list_.emplace_back(param_name + L"=" + std::to_wstring(param));
}

void ParameterList::Add(const std::wstring& param_name, long double param)
{
	list_.emplace_back(param_name + L"=" + std::to_wstring(param));
}

void ParameterList::Add(const std::string& param_name)
{
	Add(Utf8ToWide(param_name));
}

void ParameterList::Add(const std::string& param_name, std::string& param)
{
	Add(Utf8ToWide(param_name), Utf8ToWide(param));
}

void ParameterList::Add(const std::string& param_name, Int32 param)
{
	Add(Utf8ToWide(param_name), param);
}

void ParameterList::Add(const std::string& param_name, Int64 param)
{
	Add(Utf8ToWide(param_name), param);
}

void ParameterList::Add(const std::string& param_name, double param)
{
	Add(Utf8ToWide(param_name), param);
}

void ParameterList::Add(const std::string& param_name, long double param)
{
	Add(Utf8ToWide(param_name), param);
}


ProcessObject::ProcessObject()
	: id_(0)
{ }

UInt32 ProcessObject::GetID()
{
	return id_;
}

void ProcessObject::Stop()
{
#if PLATFORM_WINDOWS
	TerminateProcess(handle_, -1);
#else

#endif
}

void ProcessObject::WaitForStop()
{
#if PLATFORM_WINDOWS
	WaitForSingleObject(handle_, INFINITE);
#else
	waitpid(*((pid_t*)handle_), nullptr, 0);
#endif
}

void ProcessObject::WaitForStop(UInt32 wait_time)
{
#if PLATFORM_WINDOWS
	WaitForSingleObject(handle_, wait_time);
#else

#endif
}

#ifdef CreateProcess
#undef CreateProcess
#endif

ProcessObjectPtr Builder::CreateProcess(const std::wstring& proc_path)
{
	return CreateProcess(proc_path, L"");
}

ProcessObjectPtr Builder::CreateProcess(const std::wstring& proc_path, const std::wstring& param)
{
#if PLATFORM_WINDOWS
	STARTUPINFOW startup_info;
	ZeroMemory(&startup_info, sizeof startup_info);
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;
	ZeroMemory(&process_info, sizeof process_info);

	SmartMemory < wchar_t > mry(param.size() + 2);
	wchar_t* _param = mry.Get();
	wsprintfW(_param, L" %s", param.c_str());

	if (!CreateProcessW(
		proc_path.c_str(), _param,
		nullptr, nullptr, FALSE, 0, nullptr, nullptr,
		&startup_info, &process_info))
	{
		return nullptr;
	}

	ProcessObjectPtr proc_obj = ProcessObjectPtr(new ProcessObject());
	proc_obj->handle_ = process_info.hProcess;
	proc_obj->id_ = process_info.dwProcessId;

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
		std::string _proc_path = WideToAnsi(proc_path);
		std::string _param = WideToAnsi(param);

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
		proc_obj->id_ = pid;
		proc_obj->handle_ = &(proc_obj->id_);

		return proc_obj;
	}
#endif
}

ProcessObjectPtr Builder::CreateProcess(const std::wstring& proc_path, const ParameterList& param)
{
	std::wstring _param = L"";

	for (USize i = 0; i < param.list_.size(); ++i)
	{
		if (i != 0) _param += L" ";
		_param += param.list_[i];
	}

	return CreateProcess(proc_path, _param);
}

ProcessObjectPtr Builder::CreateProcess(const std::string& proc_path)
{
	return CreateProcess(Utf8ToWide(proc_path));
}

ProcessObjectPtr Builder::CreateProcess(const std::string& proc_path, const std::string& param)
{
	return CreateProcess(Utf8ToWide(proc_path), Utf8ToWide(param));
}

ProcessObjectPtr Builder::CreateProcess(const std::string& proc_path, const ParameterList& param)
{
	return CreateProcess(Utf8ToWide(proc_path), param);
}

}