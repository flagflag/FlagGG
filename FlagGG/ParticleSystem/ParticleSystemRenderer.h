//
// 处理粒子渲染更新，每个渲染管线都持有一个ParticleRenderer（实际的渲染指令仍旧在底层渲染管线中执行，这里只是做渲染数据的处理）
//

#pragma once

#include "Core/Object.h"
#include "AsyncFrame/Mutex.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Scene/Component.h"
#include "Container/Ptr.h"
#include "Container/Vector.h"

namespace FlagGG
{

class ParticleSystemComponent;
class ParticleModuleRequired;
struct ParticleEmitterInstance;
struct BaseParticle;

class VertexDescription;
class Geometry;
struct RenderPiplineContext;

struct ParticleMeshData
{
	VertexDescription* vertexDesc_;
	PODVector<UInt8> vertexData_;
	UInt32 vertexCount_;
	PODVector<UInt32> indexData_;
	Geometry* geometry_;
};

class ParticleMeshDataBuilder
{
public:
	~ParticleMeshDataBuilder();

	// 开始收集粒子网格数据
	void BeginDataCollection();

	// 结束收集粒子网格数据
	void EndDataCollection();

	// 分配一个粒子网格数据
	ParticleMeshData* Alloc(VertexDescription* vertexDesc);


	// 获取粒子真实的大小
	static Vector3 GetParticleSize(const BaseParticle& particle, ParticleModuleRequired* requireModule);

	//
	static void ApplyOrbitToPosition(Int32 orbitModuleOffset, const BaseParticle& particle, bool useLocalSpace, const Matrix3x4& localToWorld, Vector3& particlePosition, Vector3& particleOldPosition);

	//
	static Vector3 GetCameraOffsetFromPayload(Int32 cameraPayloadOffset, const BaseParticle& particle, const Vector3& particlePosition, const Vector3& cameraPosition);

	//
	static Vector3 ParticleMeshDataBuilder::GetCameraOffset(float cameraPayloadOffset, Vector3 dirToCamera);

	//
	static void GetDynamicValueFromPayload(Int32 dynamicPayloadOffset, const BaseParticle& particle, Vector4& outDynamicData);

private:
	Mutex mutex_;

	PODVector<ParticleMeshData*> particleMeshDataSet_;

	PODVector<ParticleMeshData*> particleMeshDataPool_;

	// 所有粒子的顶点将被合并在一个GpuBuffer中（在移动端可以比较好的改善同步GPU数据产生的耗时）
	SharedPtr<VertexBuffer> vertexBuffer_;

	// 所有粒子的索引数据合并在一个GpuBuffer中（在移动端可以比较好的改善同步GPU数据产生的耗时）
	SharedPtr<IndexBuffer> indexBuffer_;
};

class FlagGG_API ParticleSystemRenderer : public Component
{
	OBJECT_OVERRIDE(ParticleSystemRenderer, Component);
public:
	ParticleSystemRenderer();

	~ParticleSystemRenderer() override;

	// 提交到渲染更新队列
	void PostToRenderUpdateQueue(ParticleSystemComponent* particleSystemComponent);

	// 渲染更新
	void RenderUpdate(const RenderPiplineContext* renderPiplineContext);

private:
	ParticleMeshDataBuilder particleMeshDataBuilder_;

	// 待渲染更新的粒子组件，粒子组件每帧需要调用接口提交当前帧是否渲染。
	PODVector<ParticleSystemComponent*> particleSystemComponents_;
};

}
