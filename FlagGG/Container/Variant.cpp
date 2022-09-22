#include "Variant.h"
#include "Utility/Format.h"

namespace FlagGG
{
	namespace Container
	{
		void StringToVariant(const String& inStr, FVariant& outValue)
		{
			auto typeStrTail = inStr.Find('(');
			if (typeStrTail == String::NPOS)
			{
				outValue = atof(inStr.CString());
			}
			else
			{
				auto valueTail = inStr.Find(')', typeStrTail);
				if (valueTail == String::NPOS)
				{
					outValue = 0;
				}
				else
				{
					String typeStr = inStr.Substring(0, typeStrTail);
					String valueStr = inStr.Substring(typeStrTail + 1, valueTail - typeStrTail - 1).Replaced(" ", "");
					Vector<String> valueArray = valueStr.Split(',');

					if (typeStr == "register")
					{
						outValue.Set<unsigned>(atoi(valueStr.CString()));
					}
					else if (typeStr == "Color")
					{
						Math::Color color;
						if (valueArray.Size() > 0)
							color.r_ = Utility::Format::ToFloat(valueArray[0]);
						if (valueArray.Size() > 1)
							color.g_ = Utility::Format::ToFloat(valueArray[1]);
						if (valueArray.Size() > 2)
							color.b_ = Utility::Format::ToFloat(valueArray[2]);
						if (valueArray.Size() > 3)
							color.a_ = Utility::Format::ToFloat(valueArray[3]);
						outValue = color;
						// 这样写效率更高，不用转换成Variant，但是忘记重载左值的Set函数了，后面改了再修正吧-0-!
						// outValue.Set<Math::Color>(color);
					}
				}
			}
		}
	}
}
