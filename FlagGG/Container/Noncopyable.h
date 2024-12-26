//
// 继承此类，禁用class拷贝
//

#pragma once

namespace FlagGG
{

class Noncopyable
{
protected:
	// 确保无法显示调用构造函数
	Noncopyable() {}
	// 无法多态
	~Noncopyable() {}

private:
	Noncopyable(const Noncopyable&);
	Noncopyable& operator=(const Noncopyable&);
};

}
