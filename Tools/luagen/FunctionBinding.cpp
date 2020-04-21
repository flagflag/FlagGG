#include "FunctionBinding.h"
#include "Generator.h"


// Helper functions.
static inline void outputLuaTypeCheckInstance(ostream& o, const FunctionBinding& b);
static inline void outputLuaTypeCheck(ostream& o, int index, const FunctionBinding::Param& p = 
    FunctionBinding::Param(FunctionBinding::Param::TYPE_OBJECT, FunctionBinding::Param::KIND_POINTER));
static inline void indent(ostream& o, int indentLevel);
static inline void outputBindingInvocation(ostream& o, const FunctionBinding& b, unsigned int paramCount, unsigned int indentLevel, int numBindings);
static inline void outputGetParam(ostream& o, const FunctionBinding::Param& p, int i, int indentLevel, bool offsetIndex, int numBindings);
static inline void outputMatchedBinding(ostream& o, const FunctionBinding& b, unsigned int paramCount, unsigned int indentLevel, int numBindings);
static inline void outputReturnValue(ostream& o, const FunctionBinding& b, int indentLevel, bool isDataBlock = false);
static inline void outputPushValue(ostream& o, const FunctionBinding::Param& b, int indentLevel);
static inline void outputGetValue(ostream& o, const FunctionBinding::Param& b, int index, int indentLevel);

FunctionBinding::Param::Param(FunctionBinding::Param::Type type, Kind kind, const string& info) : 
    type(type), kind(kind), info(info), hasDefaultValue(false), levelsOfIndirection(0), outParam(false)
{
}

bool FunctionBinding::Param::operator==(const Param& p) const
{
    return this->type == p.type && this->kind == p.kind && this->info == p.info;
}

bool FunctionBinding::Param::operator!=(const Param& p) const
{
    return !(*this == p);
}

FunctionBinding::FunctionBinding(string classname, string uniquename) : classname(classname), uniquename(uniquename), name(""), own(false)
{
}

string FunctionBinding::getFunctionName() const
{
    if (functionName.length() == 0)
    {
        functionName = "luaex_";
        functionName += uniquename;
    
        if (type == STATIC_FUNCTION ||
            type == STATIC_CONSTANT ||
            type == STATIC_VARIABLE)
            functionName += "_static";
        functionName += "_";

        if (returnParam.type == Param::TYPE_CONSTRUCTOR)
            functionName += "_init";
        else if (returnParam.type == Param::TYPE_DESTRUCTOR)
            functionName += "_gc";
        else
        {
            size_t i = name.rfind("::");
            if (i != name.npos)
                functionName += name.substr(i + 2);
            else
                functionName += name;
        }
    }

    return functionName;
}

unsigned int FunctionBinding::getMinParamCount() const
{
    for (unsigned int i = 0; i < paramTypes.size(); i++)
    {
        if (paramTypes[i].hasDefaultValue)
            return i;
    }
    return paramTypes.size();
}

void FunctionBinding::write(ostream& o, const vector<FunctionBinding>& bindings)
{
    GP_ASSERT(bindings.size() > 0);

    // Print the function signature.
    o << "int " << bindings[0].getFunctionName() << "(LuaState* state)\n";
    o << "{\n";

    if (bindings.size() == 1 && bindings[0].type == FunctionBinding::MEMBER_VARIABLE)
    {
        // Validate the parameter count.
        o << "    // Validate the number of parameters.\n";
        o << "    if (state->getTop() > 2)\n";
        o << "    {\n";
        o << "        state->error(\"" << bindings[0].getFunctionName();
        o << "- Invalid number of parameters (expected 1 or 2).\");\n";
        o << "    }\n\n";
        
        // Get or set the member variable depending on the number of parameters.
        o << "    " << bindings[0].classname << "* instance = getInstance(state);\n";
        o << "    if (state->getTop() == 2)\n";
        o << "    {\n";
        outputGetParam(o, bindings[0].returnParam, 1, 2, false, 1);

        if (bindings[0].returnParam.kind == FunctionBinding::Param::KIND_POINTER &&
            bindings[0].returnParam.type != FunctionBinding::Param::TYPE_OBJECT &&
            bindings[0].returnParam.info.size() > 0)
        {
            o << "        memcpy(instance->" << bindings[0].name << ", param2, sizeof(";
            o << FunctionBinding::Param(bindings[0].returnParam.type) << ") * " << bindings[0].returnParam.info << ");\n";
        }
        else
        {
            o << "        instance->" << bindings[0].name << " = ";
            if (bindings[0].returnParam.type == FunctionBinding::Param::TYPE_OBJECT &&
                bindings[0].returnParam.kind != FunctionBinding::Param::KIND_POINTER)
            {
                o << "*";
            }
            o << "param2;\n";
        }

        o << "        return 0;\n";
        o << "    }\n";
        o << "    else\n";
        o << "    {\n";
        
        // Pass the return value back to Lua.
        if (bindings[0].returnParam.type == FunctionBinding::Param::TYPE_OBJECT)
        {
            switch (bindings[0].returnParam.kind)
            {
            case FunctionBinding::Param::KIND_POINTER:
                o << "        void* returnPtr = (void*)instance->" << bindings[0].name << ";\n";
                break;
            case FunctionBinding::Param::KIND_VALUE:
                o << "        void* returnPtr = (void*)new " << bindings[0].returnParam << "(instance->" << bindings[0].name << ");\n";
                break;
            case FunctionBinding::Param::KIND_REFERENCE:
                o << "        void* returnPtr = (void*)&(instance->" << bindings[0].name << ");\n";
                break;
            default:
                GP_ERROR("Invalid return value kind '%d'.", bindings[0].returnParam.kind);
                break;
            }
        }
        else
        {
            o << "        " << bindings[0].returnParam << " result = instance->" << bindings[0].name << ";\n";
        }
        outputReturnValue(o, bindings[0], 2);
        o << "    }\n";
        o << "}\n\n";
    }
    else if (bindings.size() == 1 && 
        (bindings[0].type == FunctionBinding::STATIC_VARIABLE ||
        bindings[0].type == FunctionBinding::GLOBAL_VARIABLE))
    {
        // Validate the parameter count.
        o << "    // Validate the number of parameters.\n";
        o << "    if (state->getTop() > 1)\n";
        o << "    {\n";
		o << "        state->error(\"" << bindings[0].getFunctionName();
		o << "- Invalid number of parameters (expected 0 or 1).\");\n";
        o << "    }\n\n";
        
        // Get or set the static variable depending on the number of parameters.
        o << "    if (state->getTop() == 1)\n";
        o << "    {\n";
        outputGetParam(o, bindings[0].returnParam, 0, 2, false, 1);

        if (bindings[0].returnParam.kind == FunctionBinding::Param::KIND_POINTER &&
            bindings[0].returnParam.type != FunctionBinding::Param::TYPE_OBJECT &&
            bindings[0].returnParam.info.size() > 0)
        {
            o << "        memcpy(";
            if (bindings[0].classname.size() > 0)
                o << bindings[0].classname << "::";
            o << bindings[0].name << ", param1, sizeof(";
            o << FunctionBinding::Param(bindings[0].returnParam.type) << ") * " << bindings[0].returnParam.info << ");\n";
        }
        else
        {
            o << "        ";
            if (bindings[0].classname.size() > 0)
                o << bindings[0].classname << "::";
            o << bindings[0].name << " = ";
            if (bindings[0].returnParam.type == FunctionBinding::Param::TYPE_OBJECT &&
                bindings[0].returnParam.kind != FunctionBinding::Param::KIND_POINTER)
            {
                o << "*";
            }
            o << "param1;\n";
        }
        o << "        return 0;\n";
        o << "    }\n";
        o << "    else\n";
        o << "    {\n";
        
        // Pass the return value back to Lua.
        if (bindings[0].returnParam.type == FunctionBinding::Param::TYPE_OBJECT)
        {
            switch (bindings[0].returnParam.kind)
            {
            case FunctionBinding::Param::KIND_POINTER:
                o << "        void* returnPtr = (void*)";
                if (bindings[0].classname.size() > 0)
                    o << bindings[0].classname << "::";
                o << bindings[0].name << ";\n";
                break;
            case FunctionBinding::Param::KIND_VALUE:
                o << "        void* returnPtr = (void*)new " << bindings[0].returnParam << "(";
                if (bindings[0].classname.size() > 0)
                    o << bindings[0].classname << "::";
                o << bindings[0].name << ");\n";
                break;
            case FunctionBinding::Param::KIND_REFERENCE:
                o << "        void* returnPtr = (void*)&(";
                if (bindings[0].classname.size() > 0)
                    o << bindings[0].classname << "::";
                o << bindings[0].name << ");\n";
                break;
            default:
                GP_ERROR("Invalid return value kind '%d'.", bindings[0].returnParam.kind);
                break;
            }
        }
        else
        {
            o << "        " << bindings[0].returnParam << " result = ";
            if (bindings[0].classname.size() > 0)
                o << bindings[0].classname << "::";
            o << bindings[0].name << ";\n";
        }
        outputReturnValue(o, bindings[0], 2);
        o << "    }\n";
        o << "}\n\n";
    }
    else if (bindings.size() == 1 && bindings[0].type == FunctionBinding::MEMBER_CONSTANT)
    {
        // Validate the parameter count.
        o << "    // Validate the number of parameters.\n";
        o << "    if (state->getTop() > 1)\n";
        o << "    {\n";
		o << "        state->error(\"" << bindings[0].getFunctionName();
		o << "- Invalid number of parameters (expected 1).\");\n";
        o << "    }\n\n";
        
        // Pass the return value back to Lua.
        o << "    " << bindings[0].classname << "* instance = getInstance(state);\n";
        if (bindings[0].returnParam.type == FunctionBinding::Param::TYPE_OBJECT)
        {
            switch (bindings[0].returnParam.kind)
            {
            case FunctionBinding::Param::KIND_POINTER:
                o << "    void* returnPtr = (void*)instance->" << bindings[0].name << ";\n";
                break;
            case FunctionBinding::Param::KIND_VALUE:
                o << "    void* returnPtr = (void*)new " << bindings[0].returnParam << "(instance->" << bindings[0].name << ");\n";
                break;
            case FunctionBinding::Param::KIND_REFERENCE:
                o << "    void* returnPtr = (void*)&(instance->" << bindings[0].name << ");\n";
                break;
            default:
                GP_ERROR("Invalid return value kind '%d'.", bindings[0].returnParam.kind);
                break;
            }
        }
        else
        {
            o << "    " << bindings[0].returnParam << " result = instance->" << bindings[0].name << ";\n";
        }
        outputReturnValue(o, bindings[0], 1);
        o << "}\n\n";
    }
    else if (bindings.size() == 1 && 
        (bindings[0].type == FunctionBinding::STATIC_CONSTANT ||
        bindings[0].type == FunctionBinding::GLOBAL_CONSTANT))
    {
        // Validate the parameter count.
        o << "    // Validate the number of parameters.\n";
        o << "    if (state->getTop() > 0)\n";
        o << "    {\n";
		o << "        state->error(\"" << bindings[0].getFunctionName();
		o << "- Invalid number of parameters (expected 0).\");\n";
        o << "    }\n\n";
        
        // Pass the return value back to Lua.
        if (bindings[0].returnParam.type == FunctionBinding::Param::TYPE_OBJECT)
        {
            switch (bindings[0].returnParam.kind)
            {
            case FunctionBinding::Param::KIND_POINTER:
                o << "    void* returnPtr = (void*)";
                if (bindings[0].classname.size() > 0)
                    o << bindings[0].classname << "::";
                o << bindings[0].name << ";\n";
                break;
            case FunctionBinding::Param::KIND_VALUE:
                o << "    void* returnPtr = (void*)new " << bindings[0].returnParam << "(";
                if (bindings[0].classname.size() > 0)
                    o << bindings[0].classname << "::";
                o << bindings[0].name << ");\n";
                break;
            case FunctionBinding::Param::KIND_REFERENCE:
                o << "    void* returnPtr = (void*)&(";
                if (bindings[0].classname.size() > 0)
                    o << bindings[0].classname << "::";
                o << bindings[0].name << ");\n";
                break;
            default:
                GP_ERROR("Invalid return value kind '%d'.", bindings[0].returnParam.kind);
                break;
            }
        }
        else
        {
            o << "    " << bindings[0].returnParam << " result = ";
            if (bindings[0].classname.size() > 0)
                o << bindings[0].classname << "::";
            o << bindings[0].name << ";\n";
        }
        outputReturnValue(o, bindings[0], 1);
        o << "}\n\n";
    }
    else
    {
        // Get all valid parameter counts.
        unsigned int paramCountOffset;
        map<unsigned int, vector<const FunctionBinding*> > paramCounts;
        for (unsigned int i = 0, count = bindings.size(); i < count; i++)
        {
            unsigned int minParamCount = bindings[i].getMinParamCount();
            paramCountOffset = (bindings[i].type == FunctionBinding::MEMBER_FUNCTION || bindings[i].type == FunctionBinding::STATIC_FUNCTION) ? 1 : 0;
            paramCounts[minParamCount + paramCountOffset].push_back(&bindings[i]);

            if (minParamCount < bindings[i].paramTypes.size())
            {
                for (unsigned int c = minParamCount + 1; c <= bindings[i].paramTypes.size(); c++)
                {
                    paramCounts[c + paramCountOffset].push_back(&bindings[i]);
                }
            }
        }
        
        // Get the parameter count.
        o << "    // Get the number of parameters.\n";
		//o << "    luaex_Error luaex_err;\n\n";
        o << "    int paramCount = state->getTop();\n\n";
        
        // Retrieve all the parameters and attempt to match them to a valid binding,
        // notifying the user if the number of parameters is invalid.
        o << "    // Attempt to match the parameters to a valid binding.\n";
        o << "    switch (paramCount)\n";
        o << "    {\n";
        map<unsigned int, vector<const FunctionBinding*> >::iterator iter;
        unsigned int checkCount = 0;
        for (iter = paramCounts.begin(); iter != paramCounts.end(); iter++)
        {
            o << "        case " << iter->first << ":\n";
            o << "        {\n";

            for (unsigned int i = 0, count = iter->second.size(); i < count; i++)
            {
                // Only indent if there are parameters.
                if (iter->first > 0)
                    indent(o, 3);

                outputMatchedBinding(o, *(iter->second[i]), iter->first, 3, bindings.size());
            }

            // Only print an else clause with error report if there are parameters.
            if (iter->first > 0)
            {
                indent(o, 3);
                o << "state->error(\"" << bindings[0].getFunctionName();
                o << " - Failed to match the given parameters to a valid function signature.\");\n";
            }
            
            o << "            break;\n";
            o << "        }\n";
        }
        
        o << "        default:\n";
        o << "        {\n";
        o << "            state->error(\"" << bindings[0].getFunctionName();
		o << "- Invalid number of parameters (expected "; 
        for (iter = paramCounts.begin(), checkCount = 1; iter != paramCounts.end(); iter++)
        {
            if (checkCount == paramCounts.size() && paramCounts.size() > 1)
                o << " or ";
            o << iter->first;
            checkCount++;

            if (checkCount < paramCounts.size())
                o << ", ";
        }
        o << ").\");\n";
        o << "            break;\n";
        o << "        }\n";
        o << "    }\n";
        o << "    return 0;\n";
        o << "}\n\n";
    }
}

void FunctionBinding::writeCallback( ostream& o, const FunctionBinding& binding )
{
	int returnParams = 0;
	if (!(binding.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR ||
		binding.returnParam.type == FunctionBinding::Param::TYPE_DESTRUCTOR ||
		binding.returnParam.type == FunctionBinding::Param::TYPE_VOID ||
		binding.returnParam.type == FunctionBinding::Param::TYPE_VARARGS||
		binding.returnParam.type == FunctionBinding::Param::TYPE_UNRECOGNIZED))
	{
		returnParams = 1;
	}

	// Print the function signature.
	o << binding.returnParam << " " << binding.classname << "::" << binding.name << "_callback(";
	size_t size = binding.paramTypes.size();
	for (size_t index = 0; index < size; index++)
	{
		o << binding.paramTypes[index] << " " << binding.paramTypes[index].name;
		if (index != size - 1)
		{
			o << ", ";
		}
	}
	o << ")\n";
	o << "{\n";
	o << "    LuaState* state = LuaState::instance();\n";
	o << "    state->pushUserType((void*)this, getClassName().c_str());\n";
	o << "    while (state->getMetaTable(-1))\n";
	o << "    {\n";
	o << "        state->remove(-2);\n";
	o << "        state->pushString(\"" << binding.name << "\");\n";
	o << "        state->rawget(-2);\n";
	o << "        if (!state->isNil(-1))\n";
	o << "        {\n";
	o << "            state->pushUserType((void*)this, getClassName().c_str());\n";

	for (size_t index = 0; index < size; index++)
	{
		outputPushValue(o, binding.paramTypes[index],3);
	}

	o << "            if (!state->execute(\"\", " << size+1 <<"," << returnParams << ", 0))\n";
	o << "            {\n";
	o << "                const char* error = state->getString(-1);\n";
	o << "                Con::errorf(SCRIPT, \"Unable to execute object func %s, the error is : %s\",\" " << binding.classname << "::" << binding.name << "\", error);\n";
	if (returnParams == 1)
		o << "                return (" << binding.returnParam << ")(0);\n";
	o << "            }\n";

	outputGetValue(o, binding.returnParam, -1, 3);

	o << "        }\n";
	o << "        else\n";
	o << "        {\n";
	o << "            state->pop(1);\n";
	o << "        }\n";
	o << "    }\n";
	o << "    state->pop(1);\n\n";


	o << "    if (isModDynamicFields())\n";
	o << "    {\n";
	o << "        int32 idFunc = findDynamicFunctionHandler(\"" << binding.name << "\");\n";
	o << "        String funcPath = findDynamicFunctionLocation(\"" << binding.name << "\");\n";
	o << "        if(idFunc != LuaState::smNoRef)\n";
	o << "        {\n";
	o << "            state->rawgeti(LuaState::smGlobalIndex, idFunc);\n";
	o << "            state->pushUserType((void*)this, getClassName().c_str());\n";

	for (size_t index = 0; index < size; index++)
	{
		outputPushValue(o, binding.paramTypes[index],3);
	}

	o << "            if (!state->execute(funcPath, " << size+1 <<"," << returnParams << ", 0))\n";
	o << "            {\n";
	o << "                const char* error = state->getString(-1);\n";
	o << "                Con::errorf(SCRIPT, \"Unable to execute object func %s, the error is : %s\",\" " << binding.classname << "::" << binding.name << "\", error);\n";
	if (returnParams == 1)
		o << "                return (" << binding.returnParam << ")(0);\n";
	o << "            }\n";

	outputGetValue(o, binding.returnParam, -1, 3);

	o << "         }\n";
	if (returnParams == 1)
	{
		o << "         else\n";
		o << "         {\n";
		o << "            Con::warnf(SCRIPT, \"The func %s is not found\",\"" << binding.classname << "::" << binding.name << "\");\n";

		o << "            return (" << binding.returnParam << ")(0);\n";
		o << "         }\n";
	}

	o << "    }\n";
	if (returnParams == 1)
	{
		o << "    else\n";
		o << "    {\n";
		o << "        Con::warnf(SCRIPT, \"The func flag %s is not ModDynamicFields\",\"" << binding.classname << "::" << binding.name << "\");\n";
		if (returnParams == 1)
			o << "        return (" << binding.returnParam << ")(0);\n";
		o << "    }\n";
	}

	o << "}\n\n";
}

bool FunctionBinding::signaturesMatch(const FunctionBinding& b1, const FunctionBinding& b2)
{
    if (b1.type == b2.type)
    {
        bool namesMatch = b1.name == b2.name;

        // Ignore class qualifiers on member function bindings (to support inherited overloaded bindings).
        if (b1.type == FunctionBinding::MEMBER_CONSTANT ||
            b1.type == FunctionBinding::MEMBER_FUNCTION ||
            b1.type == FunctionBinding::MEMBER_VARIABLE)
        {
            string b1name = b1.name;
            string b2name = b2.name;
            size_t i = b1name.rfind("::");
            if (i != b1name.npos)
                b1name = b1name.substr(i + 2);
            i = b2name.rfind("::");
            if (i != b2name.npos)
                b2name = b2name.substr(i + 2);
    
            namesMatch = b1name == b2name;
        }

        // Check the binding's name, parameter types and return value type.
        if (namesMatch)
        {
            if (b1.paramTypes.size() != b2.paramTypes.size())
                return false;

            for (unsigned int i = 0, count = b1.paramTypes.size(); i < count; i++)
            {
                if (b1.paramTypes[i] != b2.paramTypes[i])
                    return false;
            }

            return true;
        }
    }
    return false;
}

std::string getTypeName(const FunctionBinding::Param& param)
{
    switch (param.type)
    {
    case FunctionBinding::Param::TYPE_VOID:
        return "void";
    case FunctionBinding::Param::TYPE_BOOL:
        return "bool";
    case FunctionBinding::Param::TYPE_CHAR:
        return "int8";
    case FunctionBinding::Param::TYPE_SHORT:
        return "int16";
    case FunctionBinding::Param::TYPE_INT:
        return "int32";
    case FunctionBinding::Param::TYPE_LONG:
        return "long";
    case FunctionBinding::Param::TYPE_UCHAR:
        return "uint8";
    case FunctionBinding::Param::TYPE_USHORT:
        return "uint16";
    case FunctionBinding::Param::TYPE_UINT:
        return "uint32";
	case FunctionBinding::Param::TYPE_UINT64:
		return "uint64";
    case FunctionBinding::Param::TYPE_ULONG:
        return "unsigned long";
    case FunctionBinding::Param::TYPE_FLOAT:
        return "float32";
    case FunctionBinding::Param::TYPE_DOUBLE:
        return "float64";
    case FunctionBinding::Param::TYPE_ENUM:
        return Generator::getInstance()->getIdentifier(param.info).c_str();
    case FunctionBinding::Param::TYPE_STRING:
        if (param.info == "string")
            return "String";
        else
            return "const char";
    case FunctionBinding::Param::TYPE_OBJECT:
	case FunctionBinding::Param::TYPE_STRUCT:
    case FunctionBinding::Param::TYPE_CONSTRUCTOR:
        return Generator::getInstance()->getIdentifier(param.info).c_str();
    case FunctionBinding::Param::TYPE_UNRECOGNIZED:
        return param.info.c_str();
    case FunctionBinding::Param::TYPE_DESTRUCTOR:
    default:
        return "";
    }
}

ostream& operator<<(ostream& o, const FunctionBinding::Param& param)
{
    o << getTypeName(param);

    if (param.kind == FunctionBinding::Param::KIND_POINTER)
    {
        for (int i = 0; i < param.levelsOfIndirection; ++i)
            o << "*";
    }

    return o;
}

// ---------------------------------------------
// Helper functions

static inline void outputLuaTypeCheckInstance(ostream& o, const FunctionBinding& b)
{
	if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR || b.type == FunctionBinding::STATIC_FUNCTION)
		o << "state->isUserTable(1, \"" << b.classname << "\", 0)";
	else
		o << "state->isUserType(1, \"" << b.classname << "\", 0)";
}

static inline void outputLuaTypeCheck(ostream& o, int index, const FunctionBinding::Param& p)
{
    switch (p.type)
    {
    case FunctionBinding::Param::TYPE_BOOL:
		/*if (p.kind == FunctionBinding::Param::KIND_POINTER) 
		{
		o << "(lua_type(state, " << index << ") == LUA_TTABLE || ";
		o << "lua_type(state, " << index << ") == LUA_TLIGHTUSERDATA)";
		}
		else*/
        {
            o << "state->isBoolean(" << index << ", 0)";
        }
        break;
	case FunctionBinding::Param::TYPE_TABLE:
		/*if (p.kind == FunctionBinding::Param::KIND_POINTER) 
		{
		o << "(lua_type(state, " << index << ") == LUA_TTABLE || ";
		o << "lua_type(state, " << index << ") == LUA_TLIGHTUSERDATA)";
		}
		else*/
        {
			o << "state->isTable(" << index << ", 0)";
        }
        break;
    case FunctionBinding::Param::TYPE_CHAR:
    case FunctionBinding::Param::TYPE_SHORT:
    case FunctionBinding::Param::TYPE_INT:
    case FunctionBinding::Param::TYPE_LONG:
    case FunctionBinding::Param::TYPE_UCHAR:
    case FunctionBinding::Param::TYPE_USHORT:
    case FunctionBinding::Param::TYPE_UINT:
    case FunctionBinding::Param::TYPE_ULONG:
    case FunctionBinding::Param::TYPE_FLOAT:
    case FunctionBinding::Param::TYPE_DOUBLE:
	case FunctionBinding::Param::TYPE_ENUM:
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER) 
        {
            o << "(lua_type(state, " << index << ") == LUA_TTABLE || ";
            o << "lua_type(state, " << index << ") == LUA_TLIGHTUSERDATA)";
        }
        else*/
        {
			o << "state->isNumber(" << index << ", 0)";
        }
        break;
	case FunctionBinding::Param::TYPE_UINT64:
		{
			o << "state->isString(" << index << ", 0)";
		}
		break;
    case FunctionBinding::Param::TYPE_STRING:
		if (p.kind == FunctionBinding::Param::KIND_POINTER) 
		{
			o << "(state->isString(" << index << ", 0) || ";
			o << "state->isNil(" << index << "))";
		}
		else
		{
			o << "state->isString(" << index << ", 0)";
			
		}
        break;
    case FunctionBinding::Param::TYPE_OBJECT:
		if (p.kind == FunctionBinding::Param::KIND_POINTER) 
		{
			o << "(state->isUserType(" << index << ", \"" << Generator::getInstance()->getIdentifier(p.info) << "\", 0) || ";
			o << "state->isNil(" << index << "))";
		}
		else
		{
			o << "state->isUserType( " << index << ", \"" << Generator::getInstance()->getIdentifier(p.info) << "\", 0)";

		}
        break;
	case FunctionBinding::Param::TYPE_REF:
			o << "!state->isNil( " << index << " )";
		break;
	case FunctionBinding::Param::TYPE_FUNCTION:
			o << "state->isFunction( " << index << " )";
		break;
	case FunctionBinding::Param::TYPE_STRUCT:
		o << "state->isString( " << index << ",  0)";
		break;
    case FunctionBinding::Param::TYPE_CONSTRUCTOR:
    case FunctionBinding::Param::TYPE_DESTRUCTOR:
    case FunctionBinding::Param::TYPE_VOID:
    case FunctionBinding::Param::TYPE_VARARGS:
    default:
        o << "state->isNoObj(" << index << ")";
    }
}

static inline void indent(ostream& o, int indentLevel)
{
    for (int k = 0; k < indentLevel; k++)
        o << "    ";
}

static inline void outputBindingInvocation(ostream& o, const FunctionBinding& b, unsigned int paramCount, unsigned int indentLevel, int numBindings)
{
	bool isNonStatic = (b.type == FunctionBinding::MEMBER_FUNCTION || b.type == FunctionBinding::STATIC_FUNCTION);

    // Get the passed in parameters.
    for (unsigned int i = 0, count = paramCount - (isNonStatic ? 1 : 0); i < count; i++)
    {
        outputGetParam(o, b.paramTypes[i], i, indentLevel, isNonStatic, numBindings);
    }

    // Get the instance for member functions.
    if ((b.type == FunctionBinding::MEMBER_FUNCTION) &&
        b.returnParam.type != FunctionBinding::Param::TYPE_CONSTRUCTOR &&
        b.returnParam.type != FunctionBinding::Param::TYPE_DESTRUCTOR)
    {
        indent(o, indentLevel);
        o << b.classname << "* instance = getInstance(state);\n";
    }

    if (b.returnParam.type == FunctionBinding::Param::TYPE_DESTRUCTOR)
    {
		indent(o, indentLevel);
		o << b.classname << "* instance = (" << b.classname << "*)state->getUserType(1, 0);\n";
		indent(o, indentLevel);

		if (Generator::getInstance()->isReferenced(b.classname))
			o << "instance->unref();\n";
		else
			o << "delete instance;\n";
    }
    else
    {
        // Create a variable to hold the return type (if appropriate).
        if (!(b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR || 
            b.returnParam.type == FunctionBinding::Param::TYPE_VOID || 
            b.returnParam.type == FunctionBinding::Param::TYPE_OBJECT))
        {
            indent(o, indentLevel);
            o << b.returnParam << " result = ";
        }

        // For functions that return objects, create the appropriate user data in Lua.
		if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR && 
			b.returnParam.kind == FunctionBinding::Param::KIND_POINTER)
		{
			indent(o, indentLevel);
			o << b.returnParam << "* returnPtr = ";
		}
        else if (b.returnParam.type == FunctionBinding::Param::TYPE_OBJECT)
        {
            indent(o, indentLevel);
            switch (b.returnParam.kind)
            {
			case FunctionBinding::Param::KIND_REFPTR:
				o << "ref_ptr<" << b.returnParam  << "> returnPtr = ";
				break;
            case FunctionBinding::Param::KIND_POINTER:
                o << b.returnParam  << " returnPtr = ";
                break;
            case FunctionBinding::Param::KIND_VALUE:
                o << b.returnParam << "* returnPtr = new " << b.returnParam << "(";
                break;
            case FunctionBinding::Param::KIND_REFERENCE:
                o << b.returnParam << "* returnPtr = &(";
                break;
            default:
                GP_ERROR("Invalid return value kind '%d'.", b.returnParam.kind);
                break;
            }
        }

        if (b.type == FunctionBinding::STATIC_FUNCTION)
        {
            if (b.returnParam.type == FunctionBinding::Param::TYPE_VOID)
                indent(o, indentLevel);
            o << b.classname << "::" << b.name << "(";
        }
        else if (b.type == FunctionBinding::GLOBAL_FUNCTION)
        {
            if (b.returnParam.type == FunctionBinding::Param::TYPE_VOID)
                indent(o, indentLevel);
            o << b.name << "(";
        }
        else if (b.type == FunctionBinding::MEMBER_FUNCTION)
        {
            if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
            {
                o << "new " << Generator::getInstance()->getIdentifier(b.returnParam.info) << "(";
            }
            else
            {
                if (b.returnParam.type == FunctionBinding::Param::TYPE_VOID)
                    indent(o, indentLevel);
                o << "instance->" << b.name << "(";
            }
        }

        // Pass the arguments.
        for (unsigned int i = 0, count = paramCount - ((isNonStatic) ? 1 : 0); i < count; i++)
        {
			if (b.paramTypes[i].type == FunctionBinding::Param::TYPE_TABLE)
				continue;

			if (b.paramTypes[i].type == FunctionBinding::Param::TYPE_VOID && b.paramTypes[i].kind != FunctionBinding::Param::KIND_POINTER)
				continue;

            if (b.paramTypes[i].type == FunctionBinding::Param::TYPE_OBJECT && (b.paramTypes[i].kind != FunctionBinding::Param::KIND_POINTER && b.paramTypes[i].kind != FunctionBinding::Param::KIND_REFPTR))
                o << "*";

			if (b.paramTypes[i].type == FunctionBinding::Param::TYPE_STRUCT && b.paramTypes[i].kind == FunctionBinding::Param::KIND_POINTER)
				o << "&";

            o << "param" << i + 1;

            if (i != count - 1)
                o << ", ";
        }

        // Output the matching parenthesis for the case where a non-pointer object is being returned.
        if (b.returnParam.type == FunctionBinding::Param::TYPE_OBJECT && b.returnParam.kind != FunctionBinding::Param::KIND_POINTER && b.returnParam.kind != FunctionBinding::Param::KIND_REFPTR)
            o << ")";

        o << ");\n";
    }

	bool isDataBlock = false;
	if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
	{
		string classname = Generator::getInstance()->getUniqueNameFromRef(b.returnParam.info);
		isDataBlock = Generator::getInstance()->isDataBlock(classname);
	}

	if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR && 
		paramCount == 2 && b.paramTypes[0].type == FunctionBinding::Param::TYPE_TABLE)
	{
		indent(o, indentLevel);
		o << "returnPtr->initFieldData(state, 2);\n";
	}

    outputReturnValue(o, b, indentLevel, isDataBlock);
}

void writeObjectTemplateType(ostream& o, const FunctionBinding::Param& p)
{
    o << getTypeName(p);
    for (int i = 0; i < p.levelsOfIndirection-1; ++i)
        o << "*";
}

void writePointerParameter(ostream& o, const char* primitiveType, const FunctionBinding::Param& p, int paramNum, int luaParamIndex, int indentLevel)
{
    o << "ScriptUtil::LuaArray<";
    writeObjectTemplateType(o, p);
    //o << "> param" << paramNum << "Pointer = ScriptUtil::get" << primitiveType << "Pointer(" << luaParamIndex << ");\n";
    o << "> param" << paramNum << " = ScriptUtil::get" << primitiveType << "Pointer(" << luaParamIndex << ");\n";
    //indent(o, indentLevel);
    //o << p << " param" << paramNum << " = (" << p << ")param" << paramNum << "Pointer;\n";
}

static inline void outputGetParam(ostream& o, const FunctionBinding::Param& p, int i, int indentLevel, bool offsetIndex, int numBindings)
{
    indent(o, indentLevel);
    o << "// Get parameter " << i + 1 << " off the stack.\n";

    int paramIndex = (offsetIndex) ? i + 2 : i + 1;

    switch (p.type)
    {
    case FunctionBinding::Param::TYPE_BOOL:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "Bool", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "bool param" << i+1 << " = state->getBoolean(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_CHAR:
        indent(o, indentLevel);
        o << "char param" << i+1 << " = (char)state->getInteger(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_SHORT:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "Short", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "short param" << i+1 << " = (short)state->getInteger(" << paramIndex << ");\n";
		break;
    case FunctionBinding::Param::TYPE_INT:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "Int", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "int param" << i+1 << " = (int)state->getInteger(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_LONG:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "Long", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "long param" << i+1 << " = (long)state->getInteger(" << paramIndex << ");\n";
		break;
    case FunctionBinding::Param::TYPE_UCHAR:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "UnsignedChar", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "unsigned char param" << i+1 << " = (unsigned char)state->getInteger(" << paramIndex << ");\n";
		break;
    case FunctionBinding::Param::TYPE_USHORT:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "UnsignedShort", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "unsigned short param" << i+1 << " = (unsigned short)state->getInteger(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_UINT:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "UnsignedInt", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "unsigned int param" << i+1 << " = (unsigned int)state->getInteger(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_UINT64:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "UnsignedInt", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "uint64 param" << i+1 << " = StringConverter::parseUint64(state->getString(" << paramIndex << "));\n";
        break;
    case FunctionBinding::Param::TYPE_ULONG:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "UnsignedLong", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "unsigned long param" << i+1 << " = (unsigned long)state->getInteger(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_FLOAT:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "Float", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "float param" << i+1 << " = (float)state->getNumber(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_DOUBLE:
        indent(o, indentLevel);
        /*if (p.kind == FunctionBinding::Param::KIND_POINTER)
            writePointerParameter(o, "Double", p, i+1, paramIndex, indentLevel);
        else*/
        	o << "double param" << i+1 << " = (double)state->getNumber(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_STRING:
        indent(o, indentLevel);
        o << p << " param" << i+1 << " = state->getString(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_ENUM:
        indent(o, indentLevel);
        o << p << " param" << i+1 << " = (" << p << ")" << "(state->getInteger(" << paramIndex << "));\n";
        break;
	case FunctionBinding::Param::TYPE_REF:
		indent(o, indentLevel);
		o << "state->pushValue(" << paramIndex << ");\n";
		indent(o, indentLevel);
		o << "RefHandle param" << i+1 << " = (RefHandle)" << "(state->ref());\n";
		break;
	case FunctionBinding::Param::TYPE_FUNCTION:
		indent(o, indentLevel);
		o << "state->pushValue(" << paramIndex << ");\n";
		indent(o, indentLevel);
		o << "RefHandle param" << i+1 << " = (RefHandle)" << "(state->ref());\n";
		break;
    case FunctionBinding::Param::TYPE_UNRECOGNIZED:
        // Attempt to retrieve the unrecognized type as an unsigned integer.
        indent(o, indentLevel);
        o << "GP_WARN(\"Attempting to get parameter " << i + 1 << " with unrecognized type " << p.info << " as an unsigned integer.\");\n";
        indent(o, indentLevel);
        o << p << " param" << i+1 << " = (" << p.info << ")state->getNumber(" << paramIndex << ");\n";
        break;
    case FunctionBinding::Param::TYPE_OBJECT:
        {
            indent(o, indentLevel);
			if (p.kind != FunctionBinding::Param::KIND_POINTER)
				o << p << "* param" << i+1 << " = (" << p << "*)state->getUserType(" << paramIndex << ", 0);\n";
			else
				o << p << " param" << i+1 << " = (" << p << ")state->getUserType(" << paramIndex << ", 0);\n";
        }
        break;
	case FunctionBinding::Param::TYPE_STRUCT:
		{
			indent(o, indentLevel);
			//if (p.kind != FunctionBinding::Param::KIND_POINTER)
				o << getTypeName(p) << " param" << i+1 << " = state->getStruct<"<< getTypeName(p) << ">(" << paramIndex << ");\n";
		}
		break;
    case FunctionBinding::Param::TYPE_CONSTRUCTOR:
    case FunctionBinding::Param::TYPE_DESTRUCTOR:
    case FunctionBinding::Param::TYPE_VOID:
    default:
        // Ignore these cases.
        break;
    }

    o << "\n";
}

static inline void outputMatchedBinding(ostream& o, const FunctionBinding& b, unsigned int paramCount, unsigned int indentLevel, int numBindings)
{
    bool isNonStatic = (b.type == FunctionBinding::MEMBER_FUNCTION || b.type == FunctionBinding::STATIC_FUNCTION);

    // If the current invocation of the function takes zero parameters, then invoke the binding.
    if (paramCount == 0)
    {
        outputBindingInvocation(o, b, paramCount, indentLevel, numBindings);
    }
    else
    {
        // NOTE: The way this currently works may cause some issues since Lua
        // has a smaller set of types than C++. There may be cases where functions
        // that take types with less precision (i.e. int vs. float) are called
        // when the user in fact wanted to call the version with more precision.
        // (this will only happen for overloaded functions).

        if (numBindings > 1)
        {
            o << "do\n";
            indent(o, indentLevel);
            o << "{\n";
            indent(o, ++indentLevel);
        }

        o << "if (";
        for (unsigned int i = 0, count = paramCount; i < count; i++)
        {
            if (isNonStatic && i == 0)
            {
                // This is always the "this / self" pointer for a member function
                outputLuaTypeCheckInstance(o, b);
            }
            else
            {
                // Function parameter
                outputLuaTypeCheck(o, i + 1, b.paramTypes[(isNonStatic ? i - 1 : i)]);
            }

            if (i == count - 1)
                o << ")\n";
            else
            {
                o << " &&\n";
                indent(o, indentLevel + 1);
            }
        }
        indent(o, indentLevel);
        o << "{\n";
            
        outputBindingInvocation(o, b, paramCount, indentLevel + 1, numBindings);

        indent(o, indentLevel);
        o << "}\n";

        if (numBindings > 1)
        {
            indent(o, --indentLevel);
            o << "} while (0);\n";
        }

        o << "\n";
    }
}
static inline void outputPushValue(ostream& o, const FunctionBinding::Param& p, int indentLevel)
{
	indent(o, indentLevel);
	switch (p.type)
	{
	case FunctionBinding::Param::TYPE_BOOL:
		o << "state->pushBoolean(" << p.name <<");\n";
		break;
	case FunctionBinding::Param::TYPE_CHAR:
	case FunctionBinding::Param::TYPE_SHORT:
	case FunctionBinding::Param::TYPE_INT:
	case FunctionBinding::Param::TYPE_LONG:
	case FunctionBinding::Param::TYPE_UCHAR:
	case FunctionBinding::Param::TYPE_USHORT:
	case FunctionBinding::Param::TYPE_UINT:
	case FunctionBinding::Param::TYPE_ULONG:
	case FunctionBinding::Param::TYPE_ENUM:
		o << "state->pushInteger(" << p.name <<");\n";
		break;
	case FunctionBinding::Param::TYPE_FLOAT:
	case FunctionBinding::Param::TYPE_DOUBLE:
		o << "state->pushNumber(" << p.name <<");\n";
		break;
	case FunctionBinding::Param::TYPE_UINT64:
		o << "state->pushString(StringConverter::toString(" << p.name <<").c_str());\n";
		break;
	case FunctionBinding::Param::TYPE_STRING:
		if (p.info == "string")
			o << "state->pushString(" << p.name <<".c_str());\n";
		else
			o << "state->pushString(" << p.name <<");\n";
		break;
	case FunctionBinding::Param::TYPE_OBJECT:
		//o << "state->pushUserType((void*)" << p.name  <<", "<<  p.name << "->getClassName().c_str()" <<");\n";
		if (Generator::getInstance()->isReferenced(Generator::getInstance()->getIdentifier(p.info)))
			o << "state->pushUserType(" << p.name  <<", \""<<  getTypeName(p) <<"\");\n";
		else
			o << "state->pushUserType((void*)" << p.name  <<", \""<<  getTypeName(p) <<"\");\n";
		break;
	case FunctionBinding::Param::TYPE_STRUCT:
		o << "state->pushStruct(" << p.name <<");\n";
		break;
	default:
		break;;
	}
}
static inline void outputGetValue(ostream& o, const FunctionBinding::Param& p, int index, int indentLevel)
{
    switch (p.type)
    {
    case FunctionBinding::Param::TYPE_BOOL:
        indent(o, indentLevel);
        o << "return" << " state->getBoolean(" << index << ");\n";
        break;
    case FunctionBinding::Param::TYPE_CHAR:
    case FunctionBinding::Param::TYPE_SHORT:
    case FunctionBinding::Param::TYPE_INT:
    case FunctionBinding::Param::TYPE_LONG:
    case FunctionBinding::Param::TYPE_UCHAR:
    case FunctionBinding::Param::TYPE_USHORT:
    case FunctionBinding::Param::TYPE_UINT:
    case FunctionBinding::Param::TYPE_ULONG:
    case FunctionBinding::Param::TYPE_ENUM:
		indent(o, indentLevel);
		o << " return" << " (" << p << ")" << "(state->getInteger(" << index << "));\n";
		break;
	case FunctionBinding::Param::TYPE_FLOAT:
	case FunctionBinding::Param::TYPE_DOUBLE:
        indent(o, indentLevel);
        o << " return" << " (" << p << ")" << "(state->getNumber(" << index << "));\n";
        break;
	case FunctionBinding::Param::TYPE_STRING:
		indent(o, indentLevel);
		o << " return" << " (" << p << ")" << "(state->getString(" << index << "));\n";
		break;
	case FunctionBinding::Param::TYPE_UINT64:
		indent(o, indentLevel);
		o << " return" << " (" << p << ")" << "StringConverter::parseUint64(state->getString(" << index << "));\n";
		break;
	case FunctionBinding::Param::TYPE_REF:
		indent(o, indentLevel);
		o << "state->pushValue(" << index << ");\n";
		indent(o, indentLevel);
		o << "return" << " (RefHandle)" << "(state->ref());\n";
		break;
	case FunctionBinding::Param::TYPE_FUNCTION:
		indent(o, indentLevel);
		o << "state->pushValue(" << index << ");\n";
		indent(o, indentLevel);
		o << "return" << " (RefHandle)" << "(state->ref());\n";
		break;
    case FunctionBinding::Param::TYPE_UNRECOGNIZED:
        indent(o, indentLevel);
        o << "GP_WARN(\"Attempting to get parameter " << " with unrecognized type " << p.info << " as an unsigned integer.\");\n";
        break;
    case FunctionBinding::Param::TYPE_OBJECT:
        {
            indent(o, indentLevel);
			if (p.kind != FunctionBinding::Param::KIND_POINTER)
				o << "return" << " (" << p << "*)state->getUserType(" << index << ", 0);\n";
			else
				o << "return" << " (" << p << ")state->getUserType(" << index << ", 0);\n";
        }
        break;
	case FunctionBinding::Param::TYPE_STRUCT:
		{
			indent(o, indentLevel);
			//if (p.kind != FunctionBinding::Param::KIND_POINTER)
			o << " return" << " state->getStruct<"<< getTypeName(p) << ">(" << index << ");\n";
		}
		break;
    case FunctionBinding::Param::TYPE_CONSTRUCTOR:
    case FunctionBinding::Param::TYPE_DESTRUCTOR:
    case FunctionBinding::Param::TYPE_VOID:
    default:
        // Ignore these cases.
        break;;
    }
}
static inline void outputReturnValue(ostream& o, const FunctionBinding& b, int indentLevel, bool isDataBlock)
{
    // Pass the return value back to Lua.
    if (!(b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR || 
        b.returnParam.type == FunctionBinding::Param::TYPE_DESTRUCTOR ||
        b.returnParam.type == FunctionBinding::Param::TYPE_VOID || 
        b.returnParam.type == FunctionBinding::Param::TYPE_OBJECT))
    {
        o << "\n";
        indent(o, indentLevel);
        o << "// Push the return value onto the stack.\n";

        // If the return type is a basic type pointer, return it as light user data.
        if (b.returnParam.type != FunctionBinding::Param::TYPE_ENUM &&
            b.returnParam.type != FunctionBinding::Param::TYPE_STRING &&
            b.returnParam.kind == FunctionBinding::Param::KIND_POINTER)
        {
            indent(o, indentLevel);
            o << "state->pushUserData(result);\n";
            indent(o, indentLevel);
            o << "return 1;\n";
            return;
        }
    }

	int returnNum = 1;
    indent(o, indentLevel);
    switch (b.returnParam.type)
    {
    case FunctionBinding::Param::TYPE_BOOL:
        o << "state->pushBoolean(result);\n";
        break;
    case FunctionBinding::Param::TYPE_CHAR:
    case FunctionBinding::Param::TYPE_SHORT:
    case FunctionBinding::Param::TYPE_INT:
    case FunctionBinding::Param::TYPE_LONG:
        o << "state->pushInteger(result);\n";
        break;
    case FunctionBinding::Param::TYPE_UNRECOGNIZED:
        o << "GP_WARN(\"Attempting to return value with unrecognized type " << b.returnParam.info << " as an unsigned integer.\");\n";
        indent(o, indentLevel);
    case FunctionBinding::Param::TYPE_UCHAR:
    case FunctionBinding::Param::TYPE_USHORT:
    case FunctionBinding::Param::TYPE_UINT:
    case FunctionBinding::Param::TYPE_ULONG:
        o << "state->pushInteger(result);\n";
        break;
	case FunctionBinding::Param::TYPE_UINT64:
		o << "state->pushString(StringConverter::toString(result).c_str());\n";
		break;
    case FunctionBinding::Param::TYPE_FLOAT:
    case FunctionBinding::Param::TYPE_DOUBLE:
        o << "state->pushNumber(result);\n";
        break;
    case FunctionBinding::Param::TYPE_ENUM:
        o << "state->pushNumber(result);\n";
        break;
    case FunctionBinding::Param::TYPE_STRING:
        if (b.returnParam.info == "String")
            o << "state->pushString(result.c_str());\n";
        else
            o << "state->pushString(result);\n";
        break;
    case FunctionBinding::Param::TYPE_OBJECT:
		if (b.returnParam.kind == FunctionBinding::Param::KIND_REFPTR)
		{
			o << "if (returnPtr)\n";
			indent(o, indentLevel);
			o << "{\n";
			indent(o, indentLevel + 1);
			//o << "state->pushUserType((void*)returnPtr, returnPtr->getClassName().c_str());\n";
			if (Generator::getInstance()->isReferenced(Generator::getInstance()->getIdentifier(b.returnParam.info)))
				o << "state->pushUserType(returnPtr.get(), \"" <<  getTypeName(b.returnParam) <<"\");\n";
			else
				o << "state->pushUserType((void*)returnPtr.get(), \"" <<  getTypeName(b.returnParam) <<"\");\n";
			indent(o, indentLevel);
			o << "}\n";
			indent(o, indentLevel);
			o << "else\n";
			indent(o, indentLevel);
			o << "{\n";
			indent(o, indentLevel + 1);
			o << "state->pushNil();\n";
			indent(o, indentLevel);
			o << "}\n";
			break;
		}
		else
		{
			o << "if (returnPtr)\n";
			indent(o, indentLevel);
			o << "{\n";
			indent(o, indentLevel + 1);
			//o << "state->pushUserType((void*)returnPtr, returnPtr->getClassName().c_str());\n";
			if (Generator::getInstance()->isReferenced(Generator::getInstance()->getIdentifier(b.returnParam.info)))
				o << "state->pushUserType(returnPtr, \"" <<  getTypeName(b.returnParam) <<"\");\n";
			else
				o << "state->pushUserType((void*)returnPtr, \"" <<  getTypeName(b.returnParam) <<"\");\n";
			indent(o, indentLevel);
			o << "}\n";
			indent(o, indentLevel);
			o << "else\n";
			indent(o, indentLevel);
			o << "{\n";
			indent(o, indentLevel + 1);
			o << "state->pushNil();\n";
			indent(o, indentLevel);
			o << "}\n";
			break;
		}

    case FunctionBinding::Param::TYPE_CONSTRUCTOR:
        o << "if (returnPtr && returnPtr->registerObject())\n";
        indent(o, indentLevel);
        o << "{\n";
        indent(o, indentLevel + 1);
		if (isDataBlock)
		{
			o << "String errorStr;\n";
			indent(o, indentLevel + 1);
			o << "if (returnPtr->preload(errorStr))\n";
			indent(o, indentLevel + 1);
			o << "{\n";
			indent(o, indentLevel + 2);
		}
		if (Generator::getInstance()->isReferenced(Generator::getInstance()->getIdentifier(b.returnParam.info)))
			o << "state->pushUserType(returnPtr, \""  << Generator::getInstance()->getUniqueNameFromRef(b.returnParam.info) << "\");\n";
		else
			o << "state->pushUserType((void*)returnPtr, \""  << Generator::getInstance()->getUniqueNameFromRef(b.returnParam.info) << "\");\n";

		if (isDataBlock)
		{
			indent(o, indentLevel + 1);
			o << "}\n";
			indent(o, indentLevel + 1);
			o << "else\n";
			indent(o, indentLevel + 1);
			o << "{\n";
			indent(o, indentLevel + 2);
			o << "Con::errorf(SCRIPT, \"preload failed for %s: %s.\", returnPtr->getName(), errorStr.c_str());\n";
			indent(o, indentLevel + 2);
			o << "returnPtr->deleteObject();\n";
			indent(o, indentLevel + 2);
			o << "state->pushNil();\n";
			indent(o, indentLevel + 1);
			o << "}\n";
		}

        indent(o, indentLevel);
        o << "}\n";
        indent(o, indentLevel);
        o << "else\n";
        indent(o, indentLevel);
        o << "{\n";
        indent(o, indentLevel + 1);
        o << "state->pushNil();\n";
        indent(o, indentLevel);
        o << "}\n";
        break;
	case FunctionBinding::Param::TYPE_STRUCT:
		o << "state->pushStruct(result);\n";
		break;
    case FunctionBinding::Param::TYPE_DESTRUCTOR:
    case FunctionBinding::Param::TYPE_VOID:
    default:
        returnNum = 0;
        break;
    }

	for (unsigned int i = 0, count = b.paramTypes.size(); i < count; i++)
	{
		FunctionBinding::Param p = b.paramTypes[i];
		if (p.outParam)
		{
			stringstream out;
			out<<"param"<<i+1;
			p.name = out.str();
			outputPushValue(o, p, indentLevel);
			returnNum++;
		}
	}

    o << "\n";
    indent(o, indentLevel);
    o << "return " << returnNum <<";\n";
}
