#pragma once

#include "Export.h"
#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Core/BaseTypes.h"

namespace FlagGG { namespace IOFrame { namespace Acceptor {

class FlagGG_API IOAcceptor : public RefCounted
{
public:
	virtual ~IOAcceptor() = default;

	virtual bool Bind(const char* ip, UInt16 port) = 0;

	virtual void Start() = 0;

	virtual void Stop() = 0;

	virtual void WaitForStop() = 0;
};

typedef SharedPtr<IOAcceptor> IOAcceptorPtr;

}}}
