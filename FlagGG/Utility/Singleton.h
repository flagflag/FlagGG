#pragma once

namespace FlagGG
{

template <typename T>
class Singleton
{
public:
	static T& Instance()
	{
		static T instance;
		return instance;
	}

	static T* InstancePtr()
	{
		return &Instance();
	}

	Singleton(const Singleton& ins) = delete;

	Singleton& operator=(const Singleton& ins) = delete;

protected:
	Singleton() = default;

	virtual ~Singleton() = default;
};

}
