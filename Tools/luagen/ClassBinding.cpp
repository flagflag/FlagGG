#include "ClassBinding.h"
#include "Generator.h"

static void generateInstanceGetter(ostream& o, const string& classname, const string& uniquename)
{
    o << "static " << classname << "* getInstance(LuaState* state)\n";
    o << "{\n";
	o << "    return (" << classname << "*)(state->getUserType(1, 0));\n";
    o << "}\n\n";
}

ClassBinding::ClassBinding(string classname, string refId, bool isExport, bool isScript) : classname(classname),
    refId(refId), inaccessibleConstructor(false), inaccessibleDestructor(false), _isExport(isExport), _isScript(isScript)
{
    // Get the class's name and namespace.
    this->classname = Generator::getInstance()->getClassNameAndNamespace(classname, &ns);

    // Calculate the unique name for the class.
    uniquename = Generator::getUniqueName(this->classname);
}

void ClassBinding::write(string dir, set<string>& includes, string* bindingNS)
{
	string baseClassString = "";
	string* baseClassUniqueName = NULL;
	if (base.size() != 0 && Generator::getInstance()->isClassBindingFromRef(base[0]))
	{
		baseClassString = Generator::getInstance()->getIdentifier(base[0]);
	}

    // Calculate the constructor string.
    size_t index = uniquename.rfind(SCOPE_REPLACEMENT);
    string constructorString = "";
    string* constructorUniqueName = NULL;
    if (index != uniquename.npos && index != uniquename.length())
        constructorString += uniquename.substr(index + SCOPE_REPLACEMENT_SIZE);
    else
        constructorString += uniquename;

    // Generate a constructor function if there isn't one
    // and the class doesn't have an inaccessible constructor.
    FunctionBinding b(classname, uniquename);
    b.name = constructorString;
    b.returnParam = FunctionBinding::Param(FunctionBinding::Param::TYPE_CONSTRUCTOR, FunctionBinding::Param::KIND_POINTER, refId);
    b.type = FunctionBinding::MEMBER_FUNCTION;
    b.own = true;

    map<string, vector<FunctionBinding> >::iterator iter = bindings.find(b.getFunctionName());
    if (iter != bindings.end())
    /*{
        if (!inaccessibleConstructor)
        {
            bindings[b.getFunctionName()].push_back(b);
            constructorUniqueName = new string(b.getFunctionName());
        }
    }
    else*/
    {
        constructorUniqueName = new string(iter->second[0].getFunctionName());
    }

    // Calculate the destructor string.
    index = uniquename.rfind(SCOPE_REPLACEMENT);
    string destructorString = "~";
    string* destructorUniqueName = NULL;
    if (index != uniquename.npos && index != uniquename.length())
        destructorString += uniquename.substr(index + SCOPE_REPLACEMENT_SIZE);
    else
        destructorString += uniquename;

    // Generate a destructor function if there isn't one
    // and the class doesn't have an inaccessible destructor
    // or the class is derived from Ref.
    b = FunctionBinding(classname, uniquename);
    b.name = destructorString;
    b.returnParam = FunctionBinding::Param(FunctionBinding::Param::TYPE_DESTRUCTOR);
    b.type = FunctionBinding::MEMBER_FUNCTION;

    iter = bindings.find(b.getFunctionName());
    if (iter != bindings.end())
    {
        destructorUniqueName = new string(iter->second[0].getFunctionName());
    }

    // Write out the implementation.
    {
        string path = dir + string("lua_") + uniquename + string(".cpp");
        ostringstream o;
        if (!o)
        {
            GP_ERROR("Failed to open file '%s' for generating Lua bindings.", path.c_str());
            return;
        }
        
        //o << "#include \"Base.h\"\n";
		string includeAfx = Generator::getIncludeAfx();
		if (!includeAfx.empty())
			o << "#include \"" << includeAfx <<"\"\n";
        o << "#include \"LuaState.h\"\n";
        //o << "#include \"lua_" << uniquename << ".h\"\n";

        // Ensure we include the original class header, even 
        // if the list of includes doesn't have it.
        //if (includes.find(include) == includes.end())
            o << "#include \"" << include << "\"\n";

		includes.erase(include);
        for (set<string>::const_iterator includeIter = includes.begin(); includeIter != includes.end(); includeIter++)
        {
            o << "#include \"" << *includeIter << "\"\n";
        }
        o << "\n";
        

        // If the original class is part of a namespace and we aren't generating into that namespace,
        // include its member with a 'using' statement.
        if (ns.length() > 0 && (!bindingNS || (*bindingNS != ns)))
        {
            o << "using namespace " << ns << ";\n\n";
        }

		if (ns.length() > 0 && callbacks.size() > 0)
		{
			o << "namespace " << *bindingNS << "\n";
			o << "{\n\n";
		}

		map<string, FunctionBinding>::iterator cbIter;
		cbIter = callbacks.begin();
		for (; cbIter != callbacks.end(); cbIter++)
		{
			FunctionBinding::writeCallback(o, cbIter->second);
		}

		if (ns.length() > 0 && callbacks.size() > 0)
		{
			o << "}\n";
		}

        if (bindingNS)
        {
            o << "namespace " << *bindingNS << "\n";
            o << "{\n\n";	
        }

		// Write out the function used to get the instance for
		// calling member functions and variables.
		generateInstanceGetter(o, classname, uniquename);

		// Write out the binding functions.
		iter = bindings.begin();
		for (; iter != bindings.end(); iter++)
		{
			FunctionBinding::write(o, iter->second);
		}

        // Write out the function used to register the class with Lua.
        o << "void luaex_" << uniquename << "_open(LuaState* L)\n";
        o << "{\n";
		
		o << "    L->moduleClass(\"" << uniquename << "\",";
		o << "\"" << baseClassString << "\",";
		o << ((destructorUniqueName) ? *destructorUniqueName : "NULL");
		o << ");\n";
		o << "    L->moduleBegin(\"" << uniquename << "\");\n";

		for (vector<string>::iterator enumIter = _enums.begin(); enumIter != _enums.end(); enumIter++)
		{
			Generator::getInstance()->writeEnum(o, *enumIter, 3);
		}

		iter = bindings.begin();
		for (; iter != bindings.end(); iter++)
		{
			for (unsigned int i = 0, count = iter->second.size(); i < count; i++)
			{
				if (iter->second[i].returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
				{
					o << "      L->moduleFunction(\"new\", " << iter->second[i].getFunctionName() << ");\n";
					break;
				}
				else if (iter->second[i].returnParam.type != FunctionBinding::Param::TYPE_DESTRUCTOR)
				{
					UINT len = iter->second[i].name.length();
					char* name = new char[ len + 1 ];
					strncpy(name, iter->second[i].name.c_str(), len);
					name[ len ] = 0;
					//gui
					//*name = toupper(*name);

					o << "      L->moduleFunction(\"" << name << "\", " << iter->second[i].getFunctionName() << ");\n";

					delete []name;
					break;
				}
			}
		}

		o << "    L->moduleEnd();\n";

        o << "}\n\n";

        if (bindingNS)
            o << "}\n";

        writeFile(path, o.str());
    }

    SAFE_DELETE(constructorUniqueName);
    SAFE_DELETE(destructorUniqueName);
}