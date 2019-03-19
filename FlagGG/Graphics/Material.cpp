#include "Material.h"
#include "Log.h"
#include "Code/Code.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		bool Material::BeginLoad(IOFrame::Stream::FileStream& fileStream)
		{
			// 现在没有具体配置，所以随便写一下，等后面加了json、xml等在改
			unsigned r, g, b, a;
			fileStream.ReadUInt32(r);
			fileStream.ReadUInt32(g);
			fileStream.ReadUInt32(b);
			fileStream.ReadUInt32(a);
			diffuse_ = Math::Color(r, g, b, a);

			fileStream.ReadUInt32(r);
			fileStream.ReadUInt32(g);
			fileStream.ReadUInt32(b);
			fileStream.ReadUInt32(a);
			ambient_ = Math::Color(r, g, b, a);

			fileStream.ReadUInt32(r);
			fileStream.ReadUInt32(g);
			fileStream.ReadUInt32(b);
			fileStream.ReadUInt32(a);
			specular_ = Math::Color(r, g, b, a);

			fileStream.ReadUInt32(r);
			fileStream.ReadUInt32(g);
			fileStream.ReadUInt32(b);
			fileStream.ReadUInt32(a);
			emissive_ = Math::Color(r, g, b, a);

			fileStream.ReadStream((char*)(&power_), sizeof(power_));

			return true;
		}

		bool Material::EndLoad()
		{
			return true;
		}
	}
}
