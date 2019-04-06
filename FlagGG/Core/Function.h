#ifndef __CUSTOM_FUNCTION__
#define __CUSTOM_FUNCTION__

#include <functional>

namespace FlagGG
{
	namespace Core
	{
		template < class FunctionDefine, class InstanceType = void >
		class FunctionImpl
		{
		public:
			using FunctionType = FunctionDefine(InstanceType::*);

			FunctionImpl(FunctionType func, InstanceType* instance) :
				func_(func),
				instance_(instance)
			{ }

			template < class ... Args >
			void operator()(Args ... args)
			{
				(instance_->*func_)(args...);
			}

		private:
			InstanceType* instance_;

			FunctionType func_;
		};

		template < class FunctionDefine >
		class FunctionImpl < FunctionDefine >
		{
		public:
			using FunctionType = FunctionDefine*;

			FunctionImpl(FunctionType func) :
				func_(func)
			{ }

			template < class ... Args >
			void operator()(Args ... args)
			{
				(*func_)(args...);
			}

		private:
			FunctionType func_;
		};

		template < class T >
		class Function
		{
		public:
			typedef Function<T> Type;
			using NativeType = T;

			Function()
			{ }

			Function(nullptr_t) :
				funcWrapper_(nullptr_t)
			{ }

			Function(const Type& value)
			{
				funcWrapper_ = value.funcWrapper_;
			}

			template < class F >
			Function(F func) :
				funcWrapper_(FunctionImpl<T>(func))
			{ }

			template < class F, class I >
			Function(F func, I* ins) :
				funcWrapper_(FunctionImpl<T, I>(func, ins))
			{ }

			template < class ... Args >
			void operator()(Args ... args)
			{
				funcWrapper_(args...);
			}

		private:
			std::function<T> funcWrapper_;
		};
	}
}

#endif
