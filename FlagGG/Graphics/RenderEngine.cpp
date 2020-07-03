#include "Graphics/RenderEngine.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Light.h"
#include "Math/Math.h"
#include "Log.h"
#include "IOFrame/Buffer/IOBufferAux.h"
#include "Resource/ResourceCache.h"
#include "Math/Color.h"
#include "bgfx/bgfx.h"
#include "bx/math.h"

namespace FlagGG
{
	namespace Graphics
	{
		class BgfxCallback : public bgfx::CallbackI
		{
		protected:
			void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override
			{
				FLAGGG_LOG_CRITICAL("bgfx => file[{}] line[{}] error[{}] msg[{}].", _filePath, _line, _code, _str);
			}

			void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override
			{

			}

			void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override
			{

			}

			void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override
			{

			}

			void profilerEnd() override
			{

			}

			uint32_t cacheReadSize(uint64_t _id) override
			{
				return 0;
			}

			bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override
			{
				return false;
			}

			void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override
			{

			}

			void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) override
			{

			}

			void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) override
			{

			}

			void captureEnd() override
			{

			}

			void captureFrame(const void* _data, uint32_t _size) override
			{

			}
		};
		static BgfxCallback BGFX_CALLBACK;

		RenderEngine::RenderEngine(Core::Context* context) :
			context_(context)
		{
			shaderParameters_.AddParametersDefine<Math::Matrix3x4>(SP_WORLD_MATRIX);
			shaderParameters_.AddParametersDefine<Math::Matrix3x4>(SP_VIEW_MATRIX);
			shaderParameters_.AddParametersDefine<Math::Matrix4>(SP_PROJVIEW_MATRIX);
			shaderParameters_.AddParametersDefine<float>(SP_DELTA_TIME);
			shaderParameters_.AddParametersDefine<float>(SP_ELAPSED_TIME);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_CAMERA_POS);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_LIGHT_POS);
			shaderParameters_.AddParametersDefine<Math::Vector3>(SP_LIGHT_DIR);
			shaderParameters_.AddParametersDefine<Math::Matrix3x4>(SP_LIGHT_VIEW_MATRIX);
			shaderParameters_.AddParametersDefine<Math::Matrix4>(SP_LIGHT_PROJVIEW_MATRIX);

			for (auto& it : samplerUniform)
			{
				it.idx = bgfx::kInvalidHandle;
			}
		}

		void RenderEngine::CreateShadowRasterizerState()
		{
			shadowRasterizerState_.depthWrite_ = true;
			shadowRasterizerState_.scissorTest_ = false;
			shadowRasterizerState_.fillMode_ = FILL_SOLID;
			shadowRasterizerState_.cullMode_ = CULL_FRONT;
		}

		void RenderEngine::Initialize()
		{
			CreateShadowRasterizerState();

			bgfx::Init init;
			init.type = bgfx::RendererType::Direct3D11;
			init.resolution.width = 200;
			init.resolution.height = 200;
			init.resolution.reset = 0u;
			init.callback = &BGFX_CALLBACK;
			if (!bgfx::init(init))
			{
				FLAGGG_LOG_ERROR("bgfx::init failed.");
			}
		}

		void RenderEngine::Uninitialize()
		{
		}

		void RenderEngine::SetTextureQuality(MaterialQuality quality)
		{
			textureQuality_ = quality;
		}

		MaterialQuality RenderEngine::GetTextureQuality()
		{
			return textureQuality_;
		}

		UInt32 RenderEngine::GetFormat(Resource::CompressedFormat format)
		{
			switch (format)
			{
			case Resource::CF_RGBA:
				// return DXGI_FORMAT_R8G8B8A8_UNORM;
				return bgfx::TextureFormat::RGBA8U;

			case Resource::CF_DXT1:
				// return DXGI_FORMAT_BC1_UNORM;
				return bgfx::TextureFormat::BC1;

			case Resource::CF_DXT3:
				// return DXGI_FORMAT_BC2_UNORM;
				return bgfx::TextureFormat::BC2;

			case Resource::CF_DXT5:
				// return DXGI_FORMAT_BC3_UNORM;
				return bgfx::TextureFormat::BC3;

			default:
				return 0;
			}
		}

		UInt32 RenderEngine::GetAlphaFormat()
		{
			// return DXGI_FORMAT_A8_UNORM;
			return bgfx::TextureFormat::A8;
		}

		UInt32 RenderEngine::GetLuminanceFormat()
		{
			// return DXGI_FORMAT_R8_UNORM;
			return bgfx::TextureFormat::R8;
		}

		UInt32 RenderEngine::GetLuminanceAlphaFormat()
		{
			// return DXGI_FORMAT_R8G8_UNORM;
			return bgfx::TextureFormat::RG8;
		}

		UInt32 RenderEngine::GetRGBFormat()
		{
			// return DXGI_FORMAT_R8G8B8A8_UNORM;
			return bgfx::TextureFormat::RGBA8;
		}

		UInt32 RenderEngine::GetRGBAFormat()
		{
			// return DXGI_FORMAT_R8G8B8A8_UNORM;
			return bgfx::TextureFormat::RGBA8;
		}

		UInt32 RenderEngine::GetRGBA16Format()
		{
			// return DXGI_FORMAT_R16G16B16A16_UNORM;
			return bgfx::TextureFormat::RGBA16;
		}

		UInt32 RenderEngine::GetRGBAFloat16Format()
		{
			// return DXGI_FORMAT_R16G16B16A16_FLOAT;
			return bgfx::TextureFormat::RGBA16F;
		}

		UInt32 RenderEngine::GetRGBAFloat32Format()
		{
			// return DXGI_FORMAT_R32G32B32A32_FLOAT;
			return bgfx::TextureFormat::RGBA32F;
		}

		UInt32 RenderEngine::GetRG16Format()
		{
			// return DXGI_FORMAT_R16G16_UNORM;
			return bgfx::TextureFormat::RG16;
		}

		UInt32 RenderEngine::GetRGFloat16Format()
		{
			// return DXGI_FORMAT_R16G16_FLOAT;
			return bgfx::TextureFormat::RG16F;
		}

		UInt32 RenderEngine::GetRGFloat32Format()
		{
			// return DXGI_FORMAT_R32G32_FLOAT;
			return bgfx::TextureFormat::RG32F;
		}

		UInt32 RenderEngine::GetFloat16Format()
		{
			// return DXGI_FORMAT_R16_FLOAT;
			return bgfx::TextureFormat::R16F;
		}

		UInt32 RenderEngine::GetFloat32Format()
		{
			// return DXGI_FORMAT_R32_FLOAT;
			return bgfx::TextureFormat::R32F;
		}

		UInt32 RenderEngine::GetLinearDepthFormat()
		{
			// return DXGI_FORMAT_R32_FLOAT;
			return bgfx::TextureFormat::R32F;
		}

		UInt32 RenderEngine::GetDepthStencilFormat()
		{
			// return DXGI_FORMAT_R24G8_TYPELESS;
			return bgfx::TextureFormat::D24;
		}

		UInt32 RenderEngine::GetReadableDepthFormat()
		{
			// return DXGI_FORMAT_R24G8_TYPELESS;
			return bgfx::TextureFormat::D24;
		}

		UInt32 RenderEngine::GetFormat(const Container::String& formatName)
		{
			Container::String nameLower = formatName.ToLower().Trimmed();

			if (nameLower == "a")
				return GetAlphaFormat();
			if (nameLower == "l")
				return GetLuminanceFormat();
			if (nameLower == "la")
				return GetLuminanceAlphaFormat();
			if (nameLower == "rgb")
				return GetRGBFormat();
			if (nameLower == "rgba")
				return GetRGBAFormat();
			if (nameLower == "rgba16")
				return GetRGBA16Format();
			if (nameLower == "rgba16f")
				return GetRGBAFloat16Format();
			if (nameLower == "rgba32f")
				return GetRGBAFloat32Format();
			if (nameLower == "rg16")
				return GetRG16Format();
			if (nameLower == "rg16f")
				return GetRGFloat16Format();
			if (nameLower == "rg32f")
				return GetRGFloat32Format();
			if (nameLower == "r16f")
				return GetFloat16Format();
			if (nameLower == "r32f" || nameLower == "float")
				return GetFloat32Format();
			if (nameLower == "lineardepth" || nameLower == "depth")
				return GetLinearDepthFormat();
			if (nameLower == "d24s8")
				return GetDepthStencilFormat();
			if (nameLower == "readabledepth" || nameLower == "hwdepth")
				return GetReadableDepthFormat();

			return GetRGBFormat();
		}

		UInt32 RenderEngine::GetMaxBonesNum()
		{
			return 64;
		}

		ShaderParameters& RenderEngine::GetShaderParameters()
		{
			return shaderParameters_;
		}

		void RenderEngine::PostRenderBatch(const Container::Vector<Container::SharedPtr<Batch>>& batches)
		{
			batches_.Clear();
			for (const auto& batch : batches)
			{
				batches_.Push(batch);
			}
		}

		void RenderEngine::SetShaderParameter(Scene::Camera* camera, const RenderContext* renderContext)
		{
// 			if (!renderContext)
// 				return;
// 
// 			// 视图矩阵，投影矩阵，蒙皮矩阵等
// 			if (camera && renderContext->worldTransform_ && renderContext->numWorldTransform_)
// 			{
// 				shaderParameters_.SetValue(SP_WORLD_MATRIX, *renderContext->worldTransform_);
// 				shaderParameters_.SetValue(SP_VIEW_MATRIX, camera->GetViewMatrix());
// 				shaderParameters_.SetValue(SP_PROJVIEW_MATRIX, camera->GetProjectionMatrix() * camera->GetViewMatrix());
// 				shaderParameters_.SetValue(SP_CAMERA_POS, camera->GetNode()->GetWorldPosition());
// 
// 				if (renderContext->geometryType_ == GEOMETRY_SKINNED)
// 				{
// 					skinMatrix_ = renderContext->worldTransform_;
// 					numSkinMatrix_ = renderContext->numWorldTransform_;
// 				}
// 			}

			camera_ = camera;

			renderContext_ = renderContext;

			inShaderParameters_ = renderContext ? renderContext->shaderParameters_ : nullptr;
		}

		void RenderEngine::SetVertexBuffers(const Container::Vector<Container::SharedPtr<VertexBuffer>>& vertexBuffers)
		{
			vertexBuffers_.Clear();
			for (auto vertexBuffer : vertexBuffers)
				vertexBuffers_.Push(vertexBuffer);
			vertexBufferDirty_ = true;
		}

		void RenderEngine::SetIndexBuffer(IndexBuffer* indexBuffer)
		{
			indexBuffer_ = indexBuffer;
			indexBufferDirty_ = true;
		}

		void RenderEngine::SetVertexShader(Shader* shader)
		{
			vertexShader_ = shader;
			vertexShaderDirty_ = true;
		}

		void RenderEngine::SetPixelShader(Shader* shader)
		{
			pixelShader_ = shader;
			pixelShaderDirty_ = true;
		}

		void RenderEngine::SetTextures(const Container::Vector<Container::SharedPtr<Texture>>& textures)
		{
			for (UInt32 i = 0; i < MAX_TEXTURE_CLASS; ++i)
			{
				textures_[i] = i < textures.Size() && textures[i] ? textures[i] : defaultTextures_[i];
			}

			texturesDirty_ = true;
		}

		void RenderEngine::SetDefaultTextures(TextureClass index, Texture* texture)
		{
			defaultTextures_[index] = texture;
		}

		void RenderEngine::SetRasterizerState(RasterizerState rasterizerState)
		{
			rasterizerState_ = rasterizerState;
			rasterizerStateDirty_ = true;
		}

		void RenderEngine::SetPrimitiveType(PrimitiveType primitiveType)
		{
			primitiveType_ = primitiveType;
		}

		static const char* bgfxSamplerUniform[]=
		{
			"s_texColor",
			"s_texDiff",
			"s_texNormal",
			"s_texSpecular",
			"s_texEmissive",
			"s_texEnvironment",
			"s_texShadowmap",
		};
		static_assert(_countof(bgfxSamplerUniform) == MAX_TEXTURE_CLASS, "bgfxSamplerUniform num invalid.");

		void RenderEngine::PreDraw(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 vertexCount)
		{
			bgfx::setViewFrameBuffer(0, renderTarget_->GetSrcHandler<bgfx::FrameBufferHandle>());

			bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, Math::Color::BLACK.ToHash(), 1.0f, 0);

			if (vertexBufferDirty_)
			{
				for (UInt8 i = 0; i < vertexBuffers_.Size(); ++i)
				{
					auto vb = vertexBuffers_[i];
					if (vb)
					{
						if (vb->IsDynamic())
							bgfx::setVertexBuffer(i, vb->GetSrcHandler<bgfx::DynamicVertexBufferHandle>(), vertexStart, vertexCount);
						else
							bgfx::setVertexBuffer(i, vb->GetSrcHandler<bgfx::VertexBufferHandle>(), vertexStart, vertexCount);
					}
				}
				vertexBufferDirty_ = false;
			}

			if (indexBufferDirty_)
			{
				if (indexBuffer_)
				{
					if (indexBuffer_->IsDynamic())
						bgfx::setIndexBuffer(indexBuffer_->GetSrcHandler<bgfx::DynamicIndexBufferHandle>(), indexStart, indexCount);
					else
						bgfx::setIndexBuffer(indexBuffer_->GetSrcHandler<bgfx::IndexBufferHandle>(), indexStart, indexCount);
				}
				indexBufferDirty_ = false;
			}

			if (texturesDirty_)
			{
				for (UInt8 i = 0; i < MAX_TEXTURE_CLASS; ++i)
				{
					auto tex = textures_[i] ? textures_[i] : defaultTextures_[i];

					if (tex)
					{
						if (samplerUniform[i].idx == bgfx::kInvalidHandle)
						{
							samplerUniform[i] = bgfx::createUniform(bgfxSamplerUniform[i], bgfx::UniformType::Sampler);
						}

						bgfx::setTexture(i, samplerUniform[i], tex->GetSrcHandler<bgfx::TextureHandle>());
					}
				}
				texturesDirty_ = false;
			}

			if (inShaderParameters_)
			{
				inShaderParameters_->SubmitUniforms();
				inShaderParameters_ = nullptr;
			}

			if (camera_ && viewport_)
			{
				float view[16];
				camera_->GetViewMatrix(view);

				float proj[16];
				camera_->GetProjectionMatrix(proj);

				bgfx::setViewTransform(0, view, proj);

				bgfx::setViewRect(0, viewport_->GetX(), viewport_->GetY(), viewport_->GetWidth(), viewport_->GetHeight());

				float time = 666;
				float mtx[16];
				bx::mtxRotateXY(mtx, time + 5 * 0.21f, time + 5 * 0.37f);
				mtx[12] = -15.0f + float(5)*3.0f;
				mtx[13] = -15.0f + float(5)*3.0f;
				mtx[14] = 0.0f;
				bgfx::setTransform(mtx);
			}

			if (rasterizerStateDirty_)
			{
				UInt64 state = 0u;

				switch (rasterizerState_.cullMode_)
				{
				case CULL_FRONT:
					state |= BGFX_STATE_CULL_CW;
					break;
						
				case CULL_BACK:
					state |= BGFX_STATE_CULL_CCW;
					break;

				default:
					break;
				}

				switch (rasterizerState_.fillMode_)
				{
				case FILL_WIREFRAME:
					state |= BGFX_STATE_WRITE_RGB;
					break;
						
				case FILL_SOLID:
					state |= BGFX_STATE_WRITE_RGB;
					break;
				}

				state |= BGFX_STATE_WRITE_A;
				state |= BGFX_STATE_WRITE_Z;
				state |= BGFX_STATE_DEPTH_TEST_LESS;
				state |= BGFX_STATE_MSAA;

				bgfx::setState(state);

				rasterizerStateDirty_ = false;
			}
		}

		void RenderEngine::DrawCallIndexed(UInt32 indexStart, UInt32 indexCount, UInt32 vertexStart, UInt32 vertexCount)
		{
			PreDraw(indexStart, indexCount, vertexStart, vertexCount);

			bgfx::ProgramHandle handle = bgfx::createProgram(
				vertexShader_->GetSrcHandler<bgfx::ShaderHandle>(),
				pixelShader_->GetSrcHandler<bgfx::ShaderHandle>());

			bgfx::submit(0, handle);
		}

		void RenderEngine::DrawCall(UInt32 vertexStart, UInt32 vertexCount)
		{
			PreDraw(0, 0, vertexStart, vertexCount);

			// draw call
			// deviceContext_->Draw(vertexCount, vertexStart);

			bgfx::ProgramHandle handle = bgfx::createProgram(
				vertexShader_->GetSrcHandler<bgfx::ShaderHandle>(),
				pixelShader_->GetSrcHandler<bgfx::ShaderHandle>());

			bgfx::submit(0, handle);
		}

		void RenderEngine::SetRenderTarget(Viewport* viewport, bool renderShadowMap)
		{
			renderTarget_ = viewport->GetRenderTarget();
			depthStencil_ = viewport->GetDepthStencil();
			renderShadowMap_ = renderShadowMap;
			renderTargetDirty_ = true;
		}

		void RenderEngine::Render(Viewport* viewport)
		{
			if (!viewport) return;

			Scene::Camera* camera = viewport->GetCamera();
			if (!camera)
				return;

			Scene::Scene* scene = viewport->GetScene();
			if (!scene)
				return;
			Container::PODVector<RenderContext*> renderContexts;
			scene->Render(renderContexts);

			viewport_ = viewport;

			float aspect = (float)viewport->GetWidth() / viewport->GetHeight();

			Container::PODVector<Scene::Light*> lights;
			scene->GetLights(lights);
			
			SetRenderTarget(viewport, true);

			// 放射相机不处理阴影
			if (!camera->GetUseReflection())
			{
				for (auto light : lights)
				{
					for (const auto& renderContext : renderContexts)
					{
						if ((renderContext->viewMask_ & camera->GetViewMask()) != renderContext->viewMask_)
							continue;
						if (!renderContext->renderPass_)
							continue;
						auto it = renderContext->renderPass_->Find(RENDER_PASS_TYPE_SHADOW);
						if (it != renderContext->renderPass_->End())
						{
							light->SetAspect(aspect);
							SetRasterizerState(shadowRasterizerState_);
							SetShaderParameter(light, renderContext);
							SetVertexShader(it->second_.vertexShader_);
							SetPixelShader(it->second_.pixelShader_);
							for (const auto& geometry : renderContext->geometries_)
							{
								SetVertexBuffers(geometry->GetVertexBuffers());
								SetIndexBuffer(geometry->GetIndexBuffer());
								SetPrimitiveType(geometry->GetPrimitiveType());
								DrawCallIndexed(geometry->GetIndexStart(), geometry->GetIndexCount(),
									geometry->GetVertexStart(), geometry->GetVertexCount());
							}
						}
					}
				}
			}
			else
			{
				// 这步操作用来清理阴影贴图
				PreDraw(0, 0, 0, 0);
			}

			SetRenderTarget(viewport, false);
			for (const auto& renderContext : renderContexts)
			{
				if ((renderContext->viewMask_ & camera->GetViewMask()) != renderContext->viewMask_)
					continue;

				if (lights.Size() > 0
					// 要不要传灯光的参数，与是否加了阴影通道没关系
					/* &&
					renderContext->renderPass_ &&
					renderContext->renderPass_->Contains(RENDER_PASS_TYPE_SHADOW)*/)
				{
					Scene::Node* lightNode = lights[0]->GetNode();
					lights[0]->SetAspect(aspect);
					shaderParameters_.SetValue(SP_LIGHT_POS, lightNode->GetWorldPosition());
					shaderParameters_.SetValue(SP_LIGHT_DIR, lightNode->GetWorldRotation() * Math::Vector3::FORWARD);
					shaderParameters_.SetValue(SP_LIGHT_VIEW_MATRIX, lights[0]->GetViewMatrix());
					shaderParameters_.SetValue(SP_LIGHT_PROJVIEW_MATRIX, lights[0]->GetProjectionMatrix() * lights[0]->GetViewMatrix());
				}

				camera->SetAspect(aspect);
				SetRasterizerState(renderContext->rasterizerState_);
				SetShaderParameter(camera, renderContext);
				SetVertexShader(renderContext->vertexShader_);
				SetPixelShader(renderContext->pixelShader_);
				SetTextures(renderContext->textures_);

				for (const auto& geometry : renderContext->geometries_)
				{
					SetVertexBuffers(geometry->GetVertexBuffers());
					SetIndexBuffer(geometry->GetIndexBuffer());
					SetPrimitiveType(geometry->GetPrimitiveType());
					DrawCallIndexed(geometry->GetIndexStart(), geometry->GetIndexCount(),
						geometry->GetVertexStart(), geometry->GetVertexCount());
				}
			}
		}

		void RenderEngine::RenderBatch(Viewport* viewport)
		{
			static Container::PODVector<VertexElement> vertexElement =
			{
				VertexElement(VE_VECTOR3, SEM_POSITION, 0),
				VertexElement(VE_VECTOR2, SEM_TEXCOORD, 0),
				VertexElement(VE_UBYTE4, SEM_COLOR, 0)
			};
			static Container::SharedPtr<Graphics::VertexBuffer> vertexBuffer;
			static Container::Vector<Container::SharedPtr<Graphics::VertexBuffer>> vertexBuffers;
			static Container::SharedPtr<Graphics::Shader> vertexShader;
			static Container::SharedPtr<Graphics::Shader> pixelShader;
			static Container::SharedPtr<Graphics::ShaderCode> vertexShaderCode;
			static Container::SharedPtr<Graphics::ShaderCode> pixelShaderCode;
			static Container::Vector<Container::SharedPtr<Graphics::Texture>> textures;

			if (!viewport)
				return;

			viewport_ = viewport;

			SetRenderTarget(viewport, false);

			if (!vertexBuffer)
			{
				vertexBuffer = new Graphics::VertexBuffer();
			}

			if (!vertexShaderCode && !pixelShaderCode)
			{
				auto* cache = context_->GetVariable<Resource::ResourceCache>("ResourceCache");

				vertexShaderCode = cache->GetResource<Graphics::ShaderCode>("Shader/UI.hlsl");
				vertexShader = vertexShaderCode->GetShader(VS, {});

				pixelShaderCode = cache->GetResource<Graphics::ShaderCode>("Shader/UI.hlsl");
				pixelShader = pixelShaderCode->GetShader(PS, {});
			}

			for (const auto& batch : batches_)
			{
				vertexBuffer->SetSize(batch->GetVertexCount(), vertexElement);
				char* data = (char*)vertexBuffer->Lock(0, vertexBuffer->GetVertexCount());
				memcpy(data, batch->GetVertexs(), batch->GetVertexCount() * batch->GetVertexSize());
				vertexBuffer->Unlock();

				vertexBuffers.Clear();
				vertexBuffers.Push(vertexBuffer);

				SetVertexBuffers(vertexBuffers);
				SetVertexShader(vertexShader);
				SetPixelShader(pixelShader);

				textures.Clear();
				if (batch->GetTexture())
				{
					textures.Push(Container::SharedPtr<Graphics::Texture>(batch->GetTexture()));
					SetTextures(textures);
				}

				switch (batch->GetType())
				{
				case Graphics::DRAW_LINE:
					SetPrimitiveType(PRIMITIVE_LINE);
					break;

				case Graphics::DRAW_TRIANGLE:
					SetPrimitiveType(PRIMITIVE_TRIANGLE);
					break;
				}

				DrawCall(0, batch->GetVertexCount());
			}
		}
	}
}