#pragma once

#include <Scene/Node.h>
#include <Resource/StreamObject.h>

using namespace FlagGG;

class Actor : public Node, public StreamObject
{
	OBJECT_OVERRIDE(Actor, Node);
public:
	Actor();

	~Actor() override;

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

	bool BeginSave(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndSave() override;

protected:
	SharedPtr<Node> sceneObject_;
};
