#include "Material.h"
#include "Log.h"
#include "Code/Code.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		bool Material::LoadFile(const Container::String& fileName)
		{
			std::ifstream stream;
			stream.open(fileName.CString());
			if (!stream.is_open())
			{
				FLAGGG_LOG_ERROR("load file [%s] failed", fileName.CString());

				return false;
			}

			// 现在没有具体配置，所以随便写一下，等后面加了json、xml等在改
			unsigned r, g, b, a;
			if (stream >> r >> g >> b >> a)
			{
				diffuse_ = Math::Color(r, g, b, a);
			}

			if (stream >> r >> g >> b >> a)
			{
				ambient_ = Math::Color(r, g, b, a);
			}

			if (stream >> r >> g >> b >> a)
			{
				specular_ = Math::Color(r, g, b, a);
			}

			if (stream >> r >> g >> b >> a)
			{
				emissive_ = Math::Color(r, g, b, a);
			}

			stream >> power_;
		}
	}
}
