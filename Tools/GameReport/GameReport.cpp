#include <IOFrame/IOFrame.h>
#include <AsyncFrame/AsyncFrame.h>
#include <Utility/SystemHelper.h>
#include <Container/Vector.h>
#include <Log.h>

#include "GameReport.h"

static FlagGG::IOFrame::Acceptor::IOAcceptorPtr g_pAcceptor;
static FlagGG::Container::Vector<FlagGG::IOFrame::Channel::IOChannelPtr> g_vecChannels;
static FlagGG::IOFrame::Buffer::IOBufferPtr g_pBuffer;

void GameReport_CPUProfilerBegin(const char* pszName)
{

}

void GameReport_CPUProfilerEnd()
{

}

void GameReport_CPUProfilerBeginFrame()
{
	if (!g_pAcceptor)
	{
		g_pAcceptor = FlagGG::IOFrame::UDP::CreateAcceptor(
			FlagGG::IOFrame::Handler::EventHandlerPtr(new FlagGG::IOFrame::Handler::NullEventHandler())
		);

		g_pBuffer = FlagGG::IOFrame::UDP::CreateBuffer();
	}
}

void GameReport_CPUProfilerEndFrame()
{
	if (g_pBuffer && g_vecChannels.Size() > 0)
	{
		g_pBuffer->ClearIndex();
		g_pBuffer->WriteStream("", 0);
		for (auto pChannel : g_vecChannels)
		{
			pChannel->Write(g_pBuffer);
		}
	}
}
