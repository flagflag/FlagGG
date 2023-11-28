#pragma once

#include "FileSystem/Archive.h"

namespace FlagGG
{

class LocalFileArchive : public Archive
{
public:
	explicit LocalFileArchive(const String& absoluteDirectory);

	~LocalFileArchive() override;


};

}
