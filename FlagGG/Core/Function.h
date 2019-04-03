#ifndef __CUSTOM_FUNCTION__
#define __CUSTOM_FUNCTION__

namespace FlagGG
{
	namespace Core
	{
		template < class FunctionDefine, class InstanceType = void >
		class Function
		{
		public:
			using FunctionType = FunctionDefine(InstanceType::*);

			Function(FunctionType func, InstanceType* instance) :
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
		class Function < FunctionDefine >
		{
		public:
			using FunctionType = FunctionDefine*;

			Function(FunctionType func) :
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
	}
}

#endif
