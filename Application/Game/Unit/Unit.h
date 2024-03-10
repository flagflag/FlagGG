#pragma once

#include <Core/Context.h>
#include <Scene/Node.h>
#include <Graphics/Material.h>
#include <Container/Str.h>
#include <Container/List.h>

namespace FlagGG
{

class SkeletonMeshComponent;

}

using namespace FlagGG;

class Unit : public Node
{
	OBJECT_OVERRIDE(Unit, Node);
public:
	Unit();

	virtual bool Load(const String& path);

	// 移动到某个点，和SetPosition不同的是：会根据速度慢慢往目标方向移动
	void MoveTo(const Vector3& position);

	void SetSpeed(float speed);

	float GetSpeed() const;

	virtual void PlayAnimation(const String& path, bool isLoop);

	void StopAnimation();

	Material* GetMaterial() const;

	void SetUnitId(Int32 unitId);

	Int32 GetUnitId() const;

	void OnUpdate(float timeStep);
	
protected:
	bool Load(const String& path, Node* node);

	void PlayAnimation(const String& path, bool isLoop, Node* node);

protected:
	Material* material_;

	float speed_;

	Int32 unitId_;
};

class CEUnit : public Unit
{
public:
	CEUnit();

	bool Load(const String& path) override;

	void PlayAnimation(const String& path, bool isLoop) override;
};

class SCEUnit : public Unit
{
public:
	SCEUnit();

	bool Load(const String& path) override;

	void PlayAnimation(const String& path, bool isLoop) override;

private:
	SkeletonMeshComponent* skeletonMeshComp_;
};
