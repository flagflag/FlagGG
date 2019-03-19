#include "Material.h"
#include "Log.h"
#include "Code/Code.h"

#include <fstream>

namespace FlagGG
{
	namespace Graphics
	{
		bool Material::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			// 现在没有具体配置，所以随便写一下，等后面加了json、xml等在改
			unsigned r, g, b, a;
			stream->ReadUInt32(r);
			stream->ReadUInt32(g);
			stream->ReadUInt32(b);
			stream->ReadUInt32(a);
			diffuse_ = Math::Color(r, g, b, a);

			stream->ReadUInt32(r);
			stream->ReadUInt32(g);
			stream->ReadUInt32(b);
			stream->ReadUInt32(a);
			ambient_ = Math::Color(r, g, b, a);

			stream->ReadUInt32(r);
			stream->ReadUInt32(g);
			stream->ReadUInt32(b);
			stream->ReadUInt32(a);
			specular_ = Math::Color(r, g, b, a);

			stream->ReadUInt32(r);
			stream->ReadUInt32(g);
			stream->ReadUInt32(b);
			stream->ReadUInt32(a);
			emissive_ = Math::Color(r, g, b, a);

			stream->ReadStream((char*)(&power_), sizeof(power_));

			return true;
		}

		bool Material::EndLoad()
		{
			return true;
		}
	}
}
