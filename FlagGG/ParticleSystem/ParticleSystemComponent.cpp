#include "ParticleSystemComponent.h"

namespace FlagGG
{

ParticleSystemComponent::ParticleSystemComponent()
{

}

Int32 ParticleSystemComponent::GetCurrentDetailMode() const
{
	return -1;
}

bool ParticleSystemComponent::GetFloatParameter(const String& inName,float& outFloat)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName)
		{
			if (param.paramType_ == PSPT_Scalar)
			{
				outFloat = param.scalar_;
				return true;
			}
			else if (param.paramType_ == PSPT_ScalarRand)
			{
				outFloat = param.scalar_ + (param.scalar_Low_ - param.scalar_) * randomStream_.FRand();
				return true;
			}
		}
	}

	return false;
}


bool ParticleSystemComponent::GetVectorParameter(const String& inName,Vector3& outVector)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName)
		{
			if (param.paramType_ == PSPT_Vector)
			{
				outVector = param.vector_;
				return true;
			}
			else if (param.paramType_ == PSPT_VectorRand)
			{
				Vector3 randValue(randomStream_.FRand(), randomStream_.FRand(), randomStream_.FRand());
				outVector = param.vector_ + (param.vector_Low_ - param.vector_) * randValue;
				return true;
			}
			else if (param.paramType_ == PSPT_VectorUnitRand)
			{
				return true;
			}
		}
	}

	return false;
}

bool ParticleSystemComponent::GetAnyVectorParameter(const String& inName, Vector3& outVector)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName)
		{
			if (param.paramType_ == PSPT_Vector)
			{
				outVector = param.vector_;
				return true;
			}
			if (param.paramType_ == PSPT_VectorRand)
			{
				Vector3 randValue(randomStream_.FRand(), randomStream_.FRand(), randomStream_.FRand());
				outVector = param.vector_ + (param.vector_Low_ - param.vector_) * randValue;
				return true;
			}
			else if (param.paramType_ == PSPT_VectorUnitRand)
			{
				return true;
			}
			if (param.paramType_ == PSPT_Scalar)
			{
				float OutFloat = param.scalar_;
				outVector = Vector3(OutFloat, OutFloat, OutFloat);
				return true;
			}
			if (param.paramType_ == PSPT_ScalarRand)
			{
				float OutFloat = param.scalar_ + (param.scalar_Low_ - param.scalar_) * randomStream_.FRand();
				outVector = Vector3(OutFloat, OutFloat, OutFloat);
				return true;
			}
			if (param.paramType_ == PSPT_Color)
			{
				outVector = Vector3(param.color_.r_, param.color_.g_, param.color_.b_);
				return true;
			}
		}
	}

	return false;
}


bool ParticleSystemComponent::GetColorParameter(const String& inName, Color& outColor)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName && param.paramType_ == PSPT_Color)
		{
			outColor = param.color_;
			return true;
		}
	}

	return false;
}


bool ParticleSystemComponent::GetNodeParameter(const String& inName, Node*& outNode)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName && param.paramType_ == PSPT_Node)
		{
			outNode = param.node_;
			return true;
		}
	}

	return false;
}


bool ParticleSystemComponent::GetMaterialParameter(const String& inName, Material*& outMaterial)
{
	// Always fail if we pass in no name.
	if (inName.Empty())
	{
		return false;
	}

	for (Int32 i = 0; i < instanceParameters_.Size(); i++)
	{
		const ParticleSysParam& param = instanceParameters_[i];
		if (param.name_ == inName && param.paramType_ == PSPT_Material)
		{
			outMaterial = param.material_;
			return true;
		}
	}

	return false;
}


}
