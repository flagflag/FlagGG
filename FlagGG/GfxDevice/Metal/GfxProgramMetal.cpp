#include "GfxProgramMetal.h"
#include "GfxShaderMetal.h"
#include "Log.h"
#include "Math/HalfFloat.h"

namespace FlagGG
{

GfxProgramMetal::GfxProgramMetal()
	: needProcessReflect_(true)
{

}

GfxProgramMetal::~GfxProgramMetal()
{

}

void GfxProgramMetal::Link(GfxShader* vertexShader, GfxShader* pixelShader)
{
	if (!vertexShader || !pixelShader)
	{
		FLAGGG_LOG_ERROR("Vertex shader or pixel shader is null.");
		return;
	}

	GfxShaderMetal* metalVS = vertexShader->Cast<GfxShaderMetal>();
	GfxShaderMetal* metalPS = pixelShader->Cast<GfxShaderMetal>();

	if (!metalVS || !metalPS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	GfxProgram::Link(vertexShader, pixelShader);
}

void GfxProgramMetal::LinkComputeShader(GfxShader* computeShader)
{
	if (!computeShader)
	{
		FLAGGG_LOG_ERROR("Compute shader is null.");
		return;
	}

	GfxShaderMetal* metalCS = computeShader->Cast<GfxShaderMetal>();

	if (!metalCS)
	{
		FLAGGG_LOG_ERROR("Not opengl shader.");
		return;
	}

	// TODO:

	GfxProgram::LinkComputeShader(computeShader);
}

void GfxProgramMetal::ProcessReflectIfNeed(mtlpp::RenderPipelineReflection* reflect)
{
	if (needProcessReflect_)
	{
		ProcessReflect(reflect);
		needProcessReflect_ = false;
	}
}

void GfxProgramMetal::ApplyUniformSetter(MetalConstantBufferVariableDesc& varDesc)
{
	switch (varDesc.dataType_)
	{
	case mtlpp::DataType::Int:
	case mtlpp::DataType::Int2:
	case mtlpp::DataType::Int3:
	case mtlpp::DataType::Int4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			memcpy(targetData, uniformValue, sizeof(Int32) * ((Int32)desc.dataType_ - (Int32)mtlpp::DataType::Int + 1));
		};
	}
	break;

	case mtlpp::DataType::UInt:
	case mtlpp::DataType::UInt2:
	case mtlpp::DataType::UInt3:
	case mtlpp::DataType::UInt4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			memcpy(targetData, uniformValue, sizeof(UInt32) * ((Int32)desc.dataType_ - (Int32)mtlpp::DataType::UInt + 1));
		};
	}
	break;

	case mtlpp::DataType::Float:
	case mtlpp::DataType::Float2:
	case mtlpp::DataType::Float3:
	case mtlpp::DataType::Float4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			memcpy(targetData, uniformValue, sizeof(float) * ((Int32)desc.dataType_ - (Int32)mtlpp::DataType::Float + 1));
		};
	}
	break;

	case mtlpp::DataType::Float2x2:
	case mtlpp::DataType::Float2x3:
	case mtlpp::DataType::Float2x4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			memcpy(targetData, uniformValue, sizeof(float) * ((Int32)desc.dataType_ - (Int32)mtlpp::DataType::Float2x2 + 4));
		};
	}
	break;

	case mtlpp::DataType::Float3x2:
	case mtlpp::DataType::Float3x3:
	case mtlpp::DataType::Float3x4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			memcpy(targetData, uniformValue, sizeof(float) * ((Int32)desc.dataType_ - (Int32)mtlpp::DataType::Float3x2 + 6));
		};
	}
	break;

	case mtlpp::DataType::Float4x2:
	case mtlpp::DataType::Float4x3:
	case mtlpp::DataType::Float4x4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			memcpy(targetData, uniformValue, sizeof(float) * ((Int32)desc.dataType_ - (Int32)mtlpp::DataType::Float4x2 + 8));
		};
	}
	break;

	case mtlpp::DataType::Half:
	case mtlpp::DataType::Half2:
	case mtlpp::DataType::Half3:
	case mtlpp::DataType::Half4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			const float* floatValue = (const float*)uniformValue;
			half_float::half* halfTargetData = (half_float::half*)targetData;
			UInt32 floatCount = (UInt32)desc.dataType_ - (UInt32)mtlpp::DataType::Half + 1;
			for (UInt32 i = 0; i < floatCount; ++i)
			{
				halfTargetData[i] = floatValue[i];
			}
		};
	}
	break;

	case mtlpp::DataType::Half2x2:
	case mtlpp::DataType::Half2x3:
	case mtlpp::DataType::Half2x4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			const float* floatValue = (const float*)uniformValue;
			half_float::half* halfTargetData = (half_float::half*)targetData;
			UInt32 floatCount = (UInt32)desc.dataType_ - (UInt32)mtlpp::DataType::Half2x2 + 4;
			for (UInt32 i = 0; i < floatCount; ++i)
			{
				halfTargetData[i] = floatValue[i];
			}
		};
	}
	break;

	case mtlpp::DataType::Half3x2:
	case mtlpp::DataType::Half3x3:
	case mtlpp::DataType::Half3x4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			const float* floatValue = (const float*)uniformValue;
			half_float::half* halfTargetData = (half_float::half*)targetData;
			UInt32 floatCount = (UInt32)desc.dataType_ - (UInt32)mtlpp::DataType::Half3x2 + 6;
			for (UInt32 i = 0; i < floatCount; ++i)
			{
				halfTargetData[i] = floatValue[i];
			}
		};
	}
	break;

	case mtlpp::DataType::Half4x2:
	case mtlpp::DataType::Half4x3:
	case mtlpp::DataType::Half4x4:
	{
		varDesc.setterFunc_ = [](const MetalConstantBufferVariableDesc& desc, const void* uniformValue, void* targetData)
		{
			const float* floatValue = (const float*)uniformValue;
			half_float::half* halfTargetData = (half_float::half*)targetData;
			UInt32 floatCount = (UInt32)desc.dataType_ - (UInt32)mtlpp::DataType::Half4x2 + 8;
			for (UInt32 i = 0; i < floatCount; ++i)
			{
				halfTargetData[i] = floatValue[i];
			}
		};
	}
	break;
	}
}

void GfxProgramMetal::ProcessReflect(mtlpp::RenderPipelineReflection* reflect)
{
	ProcessReflect(reflect->GetVertexArguments(), VS);
	ProcessReflect(reflect->GetFragmentArguments(), PS);
}

void GfxProgramMetal::ProcessReflect(const ns::Array<mtlpp::Argument>& arguments, ShaderType type)
{
	for (UInt32 i = 0; i < arguments.GetSize(); ++i)
	{
		auto& arg = arguments[i];
		switch (arg.GetType())
		{
		case mtlpp::ArgumentType::Buffer:
		{
			if (arg.GetBufferDataType() == mtlpp::DataType::Struct)
			{
				auto& desc = constantBufferDescs_[type][arg.GetIndex()];
				desc.name_ = arg.GetName().GetCStr();
				desc.size_ = arg.GetBufferDataSize();

				mtlpp::StructType structType = arg.GetBufferStructType();
				const ns::Array<mtlpp::StructMember> uniformVars = structType.GetMembers();
				for (UInt32 j = 0; j < uniformVars.GetSize(); ++j)
				{
					auto& uniformVar = uniformVars[j];
					auto& varDesc = desc.variableDescs_.EmplaceBack();
					varDesc.name_ = uniformVar.GetName().GetCStr();
					varDesc.offset_ = uniformVar.GetOffset();
					varDesc.dataType_ = uniformVar.GetDataType();
					ApplyUniformSetter(varDesc);
				}
			}
		}
		break;

		case mtlpp::ArgumentType::Texture:
		{
			MetalShaderTextureDesc& desc = textureDescs_[type][arg.GetIndex()];
			desc.textureName_ = arg.GetName().GetCStr();
		}
		break;

		case mtlpp::ArgumentType::Sampler:
		{
			MetalShaderTextureDesc& desc = textureDescs_[type][arg.GetIndex()];
			desc.samplerName_ = arg.GetName().GetCStr();
		}
		break;
		}
	}
}

}
