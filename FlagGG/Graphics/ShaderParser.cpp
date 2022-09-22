#include "Container/ArrayPtr.h"
#include "Resource/ResourceCache.h"
#include "ShaderParser.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		static Container::String TrimmedEx(Container::String& str)
		{
			unsigned trimStart = 0;
			unsigned trimEnd = str.Length();

			while (trimStart < trimEnd)
			{
				char c = str[trimStart];
				if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
					break;
				++trimStart;
			}
			while (trimEnd > trimStart)
			{
				char c = str[trimEnd - 1];
				if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
					break;
				--trimEnd;
			}

			return str.Substring(trimStart, trimEnd - trimStart);
		}

		ShaderParser::ShaderParser(Core::Context* context) :
			context_(context)
		{

		}

		bool ShaderParser::Parse(const char* buffer, UInt32 bufferSize)
		{
			index_ = buffer;
			eof_ = buffer + bufferSize;

			Container::String token = AcceptToken();

			if (token != "Shader")
			{
				FLAGGG_LOG_ERROR("Known shader.");
				return false;
			}

			// 这里有问题，后面改一下
			if (!AcceptToken('{', token))
				return false;

			if (!IsString(token))
			{
				FLAGGG_LOG_ERROR("Known shader.");
				return false;
			}

			fileName_ = token.Substring(1, token.Length() - 2);

			if (!ParseShader())
				return false;

			for (auto& it : passShaderSourceCodeMap_)
			{
				auto& sourceCode = it.second_.sourceCode_;
				Container::String out;
				if (!PreCompileShaderCode(sourceCode.Buffer(), sourceCode.Buffer() + sourceCode.Size(), out))
				{
					FLAGGG_LOG_ERROR("Prepare compile shader code => failed[pass:%s].", it.second_.passName_.CString());
				}
			}
			
			return true;
		}

		void ShaderParser::Clear()
		{
			properties_.Clear();
			shaderTags_ = ShaderTags{};
			passShaderSourceCodeMap_.Clear();
		}

		bool ShaderParser::PreCompileShaderCode(const char* head, const char* tail, Container::String& out)
		{
			while (head != tail)
			{
				while (head != tail && (*head == '\n' || *head == '\r' || *head == ' ' || *head == '\t')) ++head;
				if (head + 10 < tail &&
					head[0] == '#' &&
					head[1] == 'i' &&
					head[2] == 'n' &&
					head[3] == 'c' &&
					head[4] == 'l' &&
					head[5] == 'u' &&
					head[6] == 'd' &&
					head[7] == 'e')
				{
					head += 8;
					while (head != tail && (*head == ' ' || *head == '\t')) ++head;
					if (*head == '\"')
					{
						++head;
						const char* temp = head;
						while (temp != tail && *temp != '\"') ++temp;

						Container::String relativePath(head, temp - head);

						if (*temp != '\"')
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: missing '\"' behind #include\"{}.", relativePath.CString());
							return false;
						}

						head = temp + 1;

						while (head != tail && (*head == ' ' || *head == '\t')) ++head;

						// 这个判断不严谨，懒得写了，将就用着。
						if (head != tail && *head != '\r' && *head != '\n')
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: missing wrap behind #include\"{}\".", relativePath.CString());
							return false;
						}

						auto file = context_->GetVariable<FlagGG::Resource::ResourceCache>("ResourceCache")->GetFile(relativePath);
						if (!file)
						{
							FLAGGG_LOG_ERROR("Pre compile shader error: can not open file[{}].", relativePath.CString());
							return false;
						}

						UInt32 bufferSize = 0u;
						Container::SharedArrayPtr<char> buffer;
						file->ToBuffer(buffer, bufferSize);
						if (!PreCompileShaderCode(buffer.Get(), buffer.Get() + bufferSize, out))
							return false;
					}
					else
					{
						FLAGGG_LOG_ERROR("Pre compile shader error: missing filename behind #include.");
						return false;
					}
				}
				else
				{
					break;
				}
			}

			out.Append(head, tail - head);
		}

		bool ShaderParser::ParseShader()
		{
			if (!AcceptLeft())
				return false;
		
			for (UInt32 times = 0; times < 2; ++times)
			{
				Container::String token = AcceptKey();
				bool ret = false;

				if (token == "Properties")
				{
					ret = ParseProperties();
				}
				else if (token == "SubShader")
				{
					ret = ParseSubShader();
				}

				if (!ret)
					return false;
			}

			return AcceptRight();
		}

		bool ShaderParser::ParseProperties()
		{
			if (!AcceptLeft())
				return false;

			ShaderProperty property;
			while (AcceptProperty(property))
			{
				properties_.Push(property);
			}

			return AcceptRight();
		}

		bool ShaderParser::ParseSubShader()
		{
			if (!AcceptLeft())
				return false;

			Container::String token;

			while (AcceptToken('{', token))
			{
				if (token == "Tags")
				{
					if (!ParseTags())
						return false;
				}
				else if (token == "Pass")
				{
					if (!ParsePass())
						return false;
				}
				else
				{
					return false;
				}
			}

			return AcceptRight();
		}

		bool ShaderParser::ParsePass()
		{
			if (!AcceptLeft())
				return false;

			PassShaderSourceCode pass;

			while (true)
			{
				Container::String key = AcceptKey();
				if (key.Empty())
					break;

				if (key == "Name")
				{
					if (!AcceptToken('\n', pass.passName_))
					{
						return false;
					}
				}
				else if(key == "Vertex")
				{
					if (!ParseVertex(pass.vertexVaryingDef_, pass.inputVar_, pass.prepareDefines_))
						return false;
				}
				else if (key == "Pixel")
				{
					if (!ParsePixel(pass.pixelVaryingDef_, pass.outputVar_))
						return false;
				}
				else if (key == "CGPROGRAM")
				{
					if (!ParseSourceCode(pass.sourceCode_))
						return false;
				}
			}

			if (pass.passName_.Front() == '\"' && pass.passName_.Back() == '\"')
				pass.passName_ = pass.passName_.Substring(1, pass.passName_.Length() - 2);
			auto& _pass = passShaderSourceCodeMap_[pass.passName_];
			_pass.fileName_ = fileName_;
			_pass.passName_ = pass.passName_;
			_pass.vertexVaryingDef_.Swap(pass.vertexVaryingDef_);
			_pass.pixelVaryingDef_.Swap(pass.pixelVaryingDef_);
			_pass.prepareDefines_.Swap(pass.prepareDefines_);
			_pass.inputVar_.Swap(pass.inputVar_);
			_pass.outputVar_.Swap(pass.outputVar_);
			_pass.sourceCode_.Swap(pass.sourceCode_);
			for (auto& property : properties_)
			{
				switch (property.type_)
				{
				case ShaderPropertyInt:
					_pass.shaderParameterDefines_.Push("uniform int " + property.name_ + ";");
					break;

				case ShaderPropertyFloat:
					_pass.shaderParameterDefines_.Push("uniform float " + property.name_ + ";");
					break;

				case ShaderPropertyVector2:
					_pass.shaderParameterDefines_.Push("uniform vec2 " + property.name_ + ";");
					break;

				case ShaderPropertyVector3:
					_pass.shaderParameterDefines_.Push("uniform vec3 " + property.name_ + ";");
					break;

				case ShaderPropertyVector4:
					_pass.shaderParameterDefines_.Push("uniform vec4 " + property.name_ + ";");
					break;

				case ShaderPropertySampler2D:
					_pass.shaderParameterDefines_.Push(Utility::Format::ToString("SAMPLER2D(%s, %d);",
						property.name_.CString(), property.defaultValue_.Get<unsigned>()));
					break;

				case ShaderPropertySampler2DArray:
					_pass.shaderParameterDefines_.Push(Utility::Format::ToString("SAMPLER2DARRAY(%s, %d);",
						property.name_.CString(), property.defaultValue_.Get<unsigned>()));
					break;

				case ShaderPropertySampler3D:
					_pass.shaderParameterDefines_.Push(Utility::Format::ToString("SAMPLER3D(%s, %d);",
						property.name_.CString(), property.defaultValue_.Get<unsigned>()));
					break;

				case ShaderPropertySamplerCube:
					_pass.shaderParameterDefines_.Push(Utility::Format::ToString("SAMPLERCUBE(%s, %d);",
						property.name_.CString(), property.defaultValue_.Get<unsigned>()));
					break;
				}
			}

			return AcceptRight();
		}

		bool ShaderParser::ParseTags()
		{
			if (!AcceptLeft())
				return false;

			Container::String key;

			if (!AcceptToken('=', key))
			{
				return false;
			}

			Container::String value;

			if (!AcceptToken('\n', value))
			{
				return false;
			}

			if (key == "\"RenderType\"")
			{
				shaderTags_.renderType_ = value;
			}

			return AcceptRight();
		}

		static Container::String AcceptVar(const Container::String& line)
		{
			Container::String ret;
			const char* index = line.CString();
			const char* eof = index + line.Length();
			while (index != eof && *index != ' ' && *index != '\t') ++index;
			while (index != eof && (*index == ' ' || *index == '\t'))++index;
			while (index != eof && *index != ' ' && *index != '\t' && *index != ':')
			{
				ret.Append(*index);
				++index;
			}
			return ret;
		}

		static Container::String AcceptDesc(const Container::String& line)
		{
			const char* index = line.CString() + line.Length() - 1;
			const char* eof = line.CString();
			while (index != eof && (*index == '\r' || *index == '\n' || *index == ' ' || *index == '\t')) --index;
			const char* tail = index;
			while (index != eof && (isalpha(*index) || isdigit(*index))) --index;
			if (index == eof)
				return "";
			Container::String ret(index + 1, tail - index);
			return ret;
		}

		static Container::HashMap<Container::String, Container::String> descToName =
		{
			{ "POSITION", "a_position" },
			{ "NORMAL", "a_normal" },
			{ "TANGENT", "a_tangent" },
			{ "BITANGENT", "a_bitangent", },
			{ "COLOR", "a_color0" },
			{ "COLOR0", "a_color0" },
			{ "COLOR1", "a_color1" },
			{ "COLOR2", "a_color2" },
			{ "COLOR3", "a_color3" },
			{ "BLENDINDICES", "a_indices" },
			{ "BLENDWEIGHT", "a_weight" },
			{ "TEXCOORD", "a_texcoord0" },
			{ "TEXCOORD0", "a_texcoord0" },
			{ "TEXCOORD1", "a_texcoord1" },
			{ "TEXCOORD2", "a_texcoord2" },
			{ "TEXCOORD3", "a_texcoord3" },
			{ "TEXCOORD4", "a_texcoord4" },
			{ "TEXCOORD5", "a_texcoord5" },
			{ "TEXCOORD6", "a_texcoord6" },
			{ "TEXCOORD7", "a_texcoord7" },
			{ "DATA", "i_data0" },
			{ "DATA0", "i_data0" },
			{ "DATA1", "i_data1" },
			{ "DATA2", "i_data2" },
			{ "DATA3", "i_data3" },
			{ "DATA4", "i_data4" },
		};

		bool ShaderParser::ParseVertex(Container::PODVector<char>& varyingDef, Container::Vector<Container::String>& varName, Container::String& prepareDefines)
		{
			if (!AcceptLeft())
				return false;

			Container::String token;
			while (AcceptLine(token))
			{
				auto name = AcceptVar(token);
				auto desc = AcceptDesc(token);

				if (descToName.Contains(desc))
				{
					auto srcName = descToName[desc];

					token.Replace(name, srcName);

					UInt32 size = varyingDef.Size();
					varyingDef.Resize(varyingDef.Size() + token.Length() + 2);
					memcpy(&varyingDef[0] + size, token.CString(), token.Length());
					varyingDef[varyingDef.Size() - 2] = ';';
					varyingDef[varyingDef.Size() - 1] = '\n';

					varName.Push(srcName);
					prepareDefines += "#define " + name + " " + srcName + "\n";
				}
				else
				{
					FLAGGG_LOG_ERROR("Vertex Desc Error => the desc[%s] is invalid.", desc.CString());
					return false;
				}
			}

			return AcceptRight();
		}

		bool ShaderParser::ParsePixel(Container::PODVector<char>& varyingDef, Container::Vector<Container::String>& varName)
		{
			if (!AcceptLeft())
				return false;

			Container::String token;
			while (AcceptLine(token))
			{
				UInt32 size = varyingDef.Size();
				varyingDef.Resize(varyingDef.Size() + token.Length() + 2);
				memcpy(&varyingDef[0] + size, token.CString(), token.Length());
				varyingDef[varyingDef.Size() - 2] = ';';
				varyingDef[varyingDef.Size() - 1] = '\n';

				varName.Push(AcceptVar(token));
			}

			return AcceptRight();
		}

		bool ShaderParser::ParseSourceCode(Container::PODVector<char>& sourceCode)
		{
			while (!IsEof())
			{
				sourceCode.Push(*index_);
				Foward();

				if (sourceCode.Size() >= 5 &&
					sourceCode[sourceCode.Size() - 5] == 'E' &&
					sourceCode[sourceCode.Size() - 4] == 'N' &&
					sourceCode[sourceCode.Size() - 3] == 'D' &&
					sourceCode[sourceCode.Size() - 2] == 'C' &&
					sourceCode[sourceCode.Size() - 1] == 'G')
				{
					sourceCode.Resize(sourceCode.Size() - 5);
					return true;
				}
			}

			return false;
		}

		bool ShaderParser::IsEof()
		{
			return index_ == eof_;
		}

		void ShaderParser::Foward()
		{
			++index_;
		}

		void ShaderParser::Back()
		{
			--index_;
		}

		bool ShaderParser::IsEmptyChar()
		{
			char ch = *index_;
			return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
		}

		bool ShaderParser::IsString(const Container::String& token)
		{
			return token.Length() > 1 && token.Front() == '\"' && token.Back() == '\"';
		}

		void ShaderParser::AcceptEmptyChar()
		{
			while (!IsEof() && IsEmptyChar())
			{
				Foward();
			}
		}

		char ShaderParser::AcceptChar()
		{
			char ch = *index_;
			++index_;
			return ch;
		}

		Container::String ShaderParser::AcceptToken()
		{
			AcceptEmptyChar();
			Container::String token;
			while (!IsEof() && !IsEmptyChar())
			{
				token.Append(*index_);
				Foward();
			}
			return token;
		}

		Container::String ShaderParser::AcceptKey()
		{
			AcceptEmptyChar();
			Container::String key;
			while (!IsEof() && isalpha(*index_))
			{
				key.Append(*index_);
				Foward();
			}
			return key;
		}

		bool ShaderParser::AcceptToken(char ch, Container::String& token)
		{
			AcceptEmptyChar();
			token.Clear();
			while (!IsEof() && *index_ != ch && *index_ != '}')
			{
				token.Append(*index_);
				Foward();
			}
			token = TrimmedEx(token);
			return !IsEof() && *index_ == ch;
		}

		bool ShaderParser::AcceptLeft()
		{
			AcceptEmptyChar();
			char ch = AcceptChar();
			return ch == '{';
		}

		bool ShaderParser::AcceptRight()
		{
			AcceptEmptyChar();
			char ch = AcceptChar();
			return ch == '}';
		}

		bool ShaderParser::AcceptEqualSign()
		{
			AcceptEmptyChar();
			char ch = AcceptChar();
			return ch == '=';
		}

		bool ShaderParser::AcceptLine(Container::String& token)
		{
			AcceptEmptyChar();
			token.Clear();
			while (!IsEof() && *index_ != '\r' && *index_ != '\n' && *index_ != '}')
			{
				token.Append(*index_);
				Foward();
			}
			token = TrimmedEx(token);
			return !IsEof() && (*index_ == '\r' || *index_ == '\n');
		}

		static ShaderPropertyType StringToShaderPropertyType(const Container::String& inStr)
		{
#define GET_SHADER_PROPERTY_TYPE_IF(inStr, typeStr, typeEnum) \
		if (inStr == #typeStr) \
			return ShaderProperty ## typeEnum

			GET_SHADER_PROPERTY_TYPE_IF(inStr, Int, Int);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, Float, Float);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, Vector2, Vector2);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, Vector3, Vector3);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, Vector4, Vector4);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, 2D, Sampler2D);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, 2DArray, Sampler2DArray);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, 3D, Sampler3D);
			GET_SHADER_PROPERTY_TYPE_IF(inStr, Cube, SamplerCube);
			return ShaderPropertyUnknown;
		}

		bool ShaderParser::AcceptProperty(ShaderProperty& property)
		{
			bool ret = AcceptRight();
			Back();

			if (ret)
				return false;

			if (!AcceptToken('(', property.name_))
			{
				return false;
			}

			Foward();

			if (!AcceptToken(',', property.desc_))
			{
				return false;
			}

			Foward();

			Container::String token;

			// type
			if (!AcceptToken(')', token))
			{
				return false;
			}

			property.type_ = StringToShaderPropertyType(token);

			Foward();

			if (!AcceptEqualSign())
			{
				false;
			}

			// default value
			if (!AcceptToken('\n', token))
			{
				return false;
			}

			StringToVariant(token, property.defaultValue_);

			Foward();

			return true;
		}
	}
}
