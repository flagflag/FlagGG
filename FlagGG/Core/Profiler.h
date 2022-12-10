#pragma once

#include "Container/Str.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{

class FlagGG_API ProfilerBlock
{
public:
	ProfilerBlock(ProfilerBlock* parent, const char* name) :
		name_(nullptr),
		time_(0),
		maxTime_(0),
		count_(0),
		parent_(parent),
		frameTime_(0),
		frameMaxTime_(0),
		frameCount_(0),
		intervalTime_(0),
		intervalMaxTime_(0),
		intervalCount_(0),
		totalTime_(0),
		totalMaxTime_(0),
		totalCount_(0)
	{
		if (name)
		{
			unsigned nameLength = String::CStringLength(name);
			name_ = new char[nameLength + 1];
			memcpy(name_, name, nameLength + 1);
		}
	}

	virtual ~ProfilerBlock()
	{
		for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
		{
			delete *i;
			*i = nullptr;
		}

		delete[] name_;
	}

	void Begin()
	{
		timer_.Reset();
		++count_;
	}

	void End()
	{
		long long time = timer_.GetUSec(false);
		if (time > maxTime_)
			maxTime_ = time;
		time_ += time;
	}

	void EndFrame()
	{
		frameTime_ = time_;
		frameMaxTime_ = maxTime_;
		frameCount_ = count_;
		intervalTime_ += time_;
		if (maxTime_ > intervalMaxTime_)
			intervalMaxTime_ = maxTime_;
		intervalCount_ += count_;
		totalTime_ += time_;
		if (maxTime_ > totalMaxTime_)
			totalMaxTime_ = maxTime_;
		totalCount_ += count_;
		time_ = 0;
		maxTime_ = 0;
		count_ = 0;

		for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
			(*i)->EndFrame();
	}

	/// Begin new profiling interval.
	void BeginInterval()
	{
		intervalTime_ = 0;
		intervalMaxTime_ = 0;
		intervalCount_ = 0;

		for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
			(*i)->BeginInterval();
	}

	/// Return child block with the specified name.
	ProfilerBlock* GetChild(const char* name)
	{
		for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
		{
			if (!String::Compare((*i)->name_, name, true))
				return *i;
		}

		auto* newBlock = new ProfilerBlock(this, name);
		children_.Push(newBlock);

		return newBlock;
	}

	// Profiler������
	char* name_;
	// ʱ��ͳ����
	HiresTimer timer_;
	// ��ǰ֡�ĺ�ʱ
	long long time_;
	// ��ǰ֡Ϊֹ����ʱ
	long long maxTime_;
	// ��ǰ֡Ϊֹ���ô���
	unsigned count_;
	// Profiler�鸸�׽ڵ�
	ProfilerBlock* parent_;
	// Profiler�麢�ӽڵ�
	PODVector<ProfilerBlock*> children_;
	// ǰһ֡��ʱ
	long long frameTime_;
	// ǰһ֡Ϊֹ����ʱ
	long long frameMaxTime_;
	// ǰһ֡���ô���
	unsigned frameCount_;
	// ��ǰProfiler�����ʱ
	long long intervalTime_;
	// ��ǰProfiler��������ʱ
	long long intervalMaxTime_;
	// ��ǰProfiler������ô���
	unsigned intervalCount_;
	// �ܺ�ʱ
	long long totalTime_;
	// ������ʱ
	long long totalMaxTime_;
	// �ܵ��ô���
	unsigned totalCount_;
};

class FlagGG_API Profiler
{
public:
	explicit Profiler();
	~Profiler();

	void BeginBlock(const char* name)
	{
		current_ = current_->GetChild(name);
		current_->Begin();
	}

	void EndBlock()
	{
		current_->End();
		if (current_->parent_)
			current_ = current_->parent_;
	}

	void BeginFrame();
	void EndFrame();
	void BeginInterval();

	const String& PrintData(bool showUnused = false, bool showTotal = false, unsigned maxDepth = F_MAX_UNSIGNED) const;
	const ProfilerBlock* GetCurrentBlock() { return current_; }
	const ProfilerBlock* GetRootBlock() { return root_; }

protected:
	void PrintData(ProfilerBlock* block, String& output, unsigned depth, unsigned maxDepth, bool showUnused, bool showTotal) const;

	ProfilerBlock* current_;
	ProfilerBlock* root_;
	unsigned intervalFrames_;
};

class FlagGG_API AutoProfileBlock
{
public:
	AutoProfileBlock(Profiler* profiler, const char* name) :
		profiler_(profiler)
	{
		if (profiler_)
			profiler_->BeginBlock(name);
	}

	~AutoProfileBlock()
	{
		if (profiler_)
			profiler_->EndBlock();
	}

private:
	Profiler* profiler_;
};

}

