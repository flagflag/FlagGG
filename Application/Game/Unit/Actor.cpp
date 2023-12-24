#include "Actor.h"

Actor::Actor()
{

}

Actor::~Actor()
{

}

bool Actor::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	return false;
}

bool Actor::EndLoad()
{
	return false;
}

bool Actor::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
	return false;
}

bool Actor::EndSave()
{
	return false;
}
