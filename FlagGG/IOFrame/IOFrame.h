#pragma once

#include "IOFrame/Buffer/IOBuffer.h"
#include "IOFrame/Acceptor/IOAcceptor.h"
#include "IOFrame/Connector/IOConnector.h"
#include "IOFrame/Channel/IOChannel.h"
#include "IOFrame/Context/IOContext.h"
#include "IOFrame/Handler/IOHandler.h"
#include "IOFrame/Handler/EventHandler.h"
#include "IOFrame/ThreadPool/IOThreadPool.h"

namespace FlagGG { namespace IOFrame {

namespace TCP
{

IOThreadPoolPtr FlagGG_API CreateThreadPool(USize threadCount);

Buffer::IOBufferPtr FlagGG_API CreateBuffer();

Acceptor::IOAcceptorPtr FlagGG_API CreateAcceptor(Handler::EventHandler* handler, USize threadCount);

Connector::IOConnectorPtr FlagGG_API CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& threadPool);

}

namespace UDP
{

IOThreadPoolPtr FlagGG_API CreateThreadPool();

Buffer::IOBufferPtr FlagGG_API CreateBuffer();

Acceptor::IOAcceptorPtr FlagGG_API CreateAcceptor(Handler::EventHandler* handler);

Connector::IOConnectorPtr FlagGG_API CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& threadPool);

}

}}
