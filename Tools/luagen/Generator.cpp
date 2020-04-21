#include "Generator.h"

Generator* Generator::__instance = NULL;

// Warning flags.
static bool __printTemplateWarning = false;
static bool __printVarargWarning = false;
static bool __printOperatorWarning = false;
static bool __printMulPointWarning = false;

// Utility functions (local to this file).
static string trim(const string& str);
static string stripTypeQualifiers(const string& typeStr, FunctionBinding::Param::Kind& kind, int& levelsOfIndirection, bool& outParam);
static inline bool isWantedFileNormal(const string& s);
static inline bool isNamespaceFile(const string& s);
static inline bool isGeneratedBindingFile(const string& s);
static bool getFileList(string directory, vector<string>& files, bool (*isWantedFile)(const string& s));
static bool isReservedKeyword(string name);
static inline void indent(ostream& o, int indentLevel);

static string __exportTag = EXPORT_TAG;
static string __globalfile = LUA_GLOBAL_FILENAME;
static string __allbindingfile = LUA_ALL_BINDINGS_FILENAME;
static string __globalfunc = LUA_GLOBAL_FUNCNAME;
static string __allbindingfunc = LUA_ALL_BINDINGS_FUNCNAME;
static string __regfunc = LUA_REG_FUNCNAME;
static string __includeAfx = "";



Generator* Generator::getInstance()
{
    if (!__instance)
        __instance = new Generator();

    return __instance;
}

void Generator::releaseInstance()
{
    SAFE_DELETE(__instance);
}

string Generator::getUniqueName(string name)
{
    // Calculate the unique name for the given identifier (class, struct, enum, etc).
    string uniquename = name;
    size_t i = uniquename.find("::");
    while (i != uniquename.npos)
    {
        uniquename.replace(i, 2, SCOPE_REPLACEMENT);
        i = uniquename.find("::");
    }

    return uniquename;
}

string Generator::getUniqueNameFromRef(string refId)
{
    return getUniqueName(getIdentifier(refId));
}

vector<string> Generator::getScopePath(string classname, string ns)
{
    size_t i = classname.find("::");
    vector<string> scopePath;
	if (!ns.empty())
		scopePath.push_back(ns);
    while (i != classname.npos)
    {
        string scope = classname.substr(0, i);
        if (scope != ns)
            scopePath.push_back(scope);
        classname = classname.substr(i + 2);
        i = classname.find("::");
    }

    return scopePath;
}

string Generator::getIdentifier(string refId)
{
    if (_refIds.find(refId) != _refIds.end())
        return _refIds[refId];
    return refId;
}

string Generator::getClassNameAndNamespace(string classname, string* ns)
{
    size_t index = classname.find("::");
    if (index != classname.npos)
    {
        string classNS = classname.substr(0, index);
        if (_namespaces.find(classNS) != _namespaces.end())
        {
            if (ns)
                *ns = classNS;

            string tmp = classname.substr(index + 2);
            if (_namespaces[classNS].find(tmp) != _namespaces[classNS].end())
                return classname.substr(index + 2);
            else
            {
                size_t tmpIndex = tmp.find("::");
                if (tmpIndex != tmp.npos)
                {
                    tmp = tmp.substr(0, tmpIndex);
                    if (_namespaces[classNS].find(tmp) != _namespaces[classNS].end())
                    {
                        _namespaces[classNS].insert(classname.substr(index + 2));
                        return classname.substr(index + 2);
                    }
                }
            }
        }
    }

    return classname;
}

void Generator::setIdentifier(string refId, string id)
{
    _refIds[refId] = id;
}

bool Generator::isDerived(const ClassBinding& c, string classname)
{
    for (unsigned int i = 0; i < c.derived.size(); i++)
    {
        // If the derived class is not in the ref ID table, then it
        // is a hidden (protected, private, etc.) class, so don't consider it.
        if (_refIds.find(c.derived[i]) != _refIds.end())
        {
            const string& derivedClassName = getIdentifier(c.derived[i]);
            if (derivedClassName == classname || 
                (_classes.find(derivedClassName) != _classes.end() &&
                isDerived(_classes[derivedClassName], classname)))
            {
                return true;
            }
        }
    }
    return false;
}

bool Generator::isDataBlock(string classname)
{
	map<string, ClassBinding>::iterator iter = _classes.find(DATABLOCK_NAME);
	if (iter != _classes.end())
	{
		 return isDerived(iter->second, classname);
	}
	return false;
}


bool Generator::isReferenced( string classname )
{
	map<string, ClassBinding>::iterator iter = _classes.find(REFERENCED_NAME);
	map<string, ClassBinding>::iterator iterChild = _classes.find(classname);
	if ((iter != _classes.end()) && (iterChild != _classes.end()))
	{
		return iterChild->second.inaccessibleDestructor && isDerived(iter->second, classname);
	}
	return false;
}


string Generator::getCompoundName(XMLElement* node)
{
    // Get the name of the namespace, class, struct, or file that we are processing.
    XMLElement* nameElement = node->FirstChildElement("compoundname");
    if (!nameElement)
    {
        GP_ERROR("Missing 'compoundname' child node of 'compounddef' node for XML document '%s'.", _file);
        return "";
    }
    const char* text = nameElement->GetText();
    if (!text)
    {
        GP_ERROR("The 'compoundname' node is empty for XML document '%s'.", _file);
        return "";
    }

    return string(text);
}

void Generator::run(string inDir, string outDir, string* bindingTag, string* includeAfx, string* bindingNS)
{
	if (bindingTag)
	{
		__exportTag = *bindingTag;
		__globalfile = "lua_" + __exportTag + "_global";
		__allbindingfile = "lua_" + __exportTag + "_bindings";
		__globalfunc = "luaex_" + __exportTag + "global_open";
		__allbindingfunc = "luaex_" + __exportTag + "_register";
		__regfunc = "luaex_" + __exportTag + "reg_types";
	}
	if (includeAfx)
		__includeAfx = *includeAfx;
    // Set the output directory.
    _outDir = outDir;

    // Get a list of all .cpp and .h files in the output directory so
    // we can delete them before generating new bindings.
    vector<string> oldBindingsFiles;
    getFileList(outDir, oldBindingsFiles, isGeneratedBindingFile);

    // Get a list of the Doxygen XML files that specify a namespace.
    // Note: we must do this before adding the normal files so that
    // when we process the files sequentially, we process the namespaces
    // before the normal files (so that namespaces can be removed
    // properly from class/struct names, etc.)
    vector<string> files;
    getFileList(inDir, files, isNamespaceFile);

    // Add all of the normal Doxygen XML files we want to use to generate Lua bindings.
    if (!getFileList(inDir, files, isWantedFileNormal) || files.empty())
    {
        GP_ERROR("Failed to get a valid list of files to generate bindings from using directory '%s'.", inDir.c_str());
        return;
    }

    // Go through each file and build the data needed to generate the bindings.
    for (vector<string>::iterator iter = files.begin(); iter != files.end(); iter++)
    {
        tinyxml2::XMLDocument doc;
        _file = iter->c_str();
        doc.LoadFile(iter->c_str());

        int ret = doc.ErrorID();
        if (ret != 0)
        {
            GP_ERROR("Failed to load XML document '%s' properly with error ID %d.", iter->c_str(), ret);
            continue;
        }

        XMLElement* rootNode = doc.FirstChildElement("doxygen");
        if (!rootNode)
        {
            GP_ERROR("Missing root 'doxygen' node for XML document '%s'.", iter->c_str());
            continue;
        }
        XMLElement* node = rootNode->FirstChildElement("compounddef");
        if (!node)
        {
            GP_ERROR("Missing 'compounddef' node for XML document '%s'.", iter->c_str());
            continue;
        }

        const char* kind = node->Attribute("kind");
        if (kind && strcmp(kind, "namespace") == 0)
        {
            string name = getCompoundName(node);
            cout << "Parsing namespace " << name << "...\n";
            getNamespace(node, name);
        }
        else if ( (kind && (strcmp(kind, "class") == 0 || strcmp(kind, "struct") == 0) ) && strcmp(node->Attribute("prot"), "public") == 0)
        {
            string name = getCompoundName(node);
            cout << "Parsing class/struct " << name << "...\n";
            getClass(node, name);
        }
        else if (kind && strcmp(kind, "file") == 0)
        {
            string name = getCompoundName(node);
            cout << "Parsing file " << name << "...\n";
            if (name.find(".h") != name.npos)
            {
                getFunctions(node);
                getEnums(node, "", name);
				getTypedefs(node, "", name);
				getStructs(node, "", name);
            }
            else if (name.find(".cpp") != name.npos)
            {
                //getIncludes(node, name);
            }
        }
    }

    // Resolve all inheritance.
    resolveInheritance();

    // Resolve all unrecognized parameter and return value types.
    resolveTypes();

    // Resolve all inherited include files.
    resolveInheritedIncludes();

    // Generate the script bindings.
    generateBindings(bindingNS);

    // Print out all warnings (unsupported types, function name-Lua keyword clashes, etc.)
    if (__warnings.size() > 0)
    {
        cout << "\nWarnings:\n";
        for (set<string>::iterator iter = __warnings.begin(); iter!= __warnings.end(); iter++)
        {
            GP_WARN("%s", iter->c_str());
        }
    }

    // Print warnings for the use of unsupported C++ features.
    if (__printTemplateWarning)				
        GP_WARN("Detected the use of template parameters; this feature of C++ is not supported.");
    if (__printVarargWarning)
        GP_WARN("Detected the use of variable argument lists; this feature of C++ is not supported.");
    if (__printOperatorWarning)
        GP_WARN("Detected the use of operator overloading; this feature of C++ is not supported.");
	if (__printMulPointWarning)
		GP_WARN("Detected the use of point's point; this feature of C++ is not supported.");

    // Delete files that are no longer needed and print warnings for them
    for (unsigned int i = 0; i < oldBindingsFiles.size(); i++)
    {
        if (std::find(generatedFiles.begin(), generatedFiles.end(), oldBindingsFiles[i]) == generatedFiles.end())
        {
            GP_WARN("Deleting unused file: %s", oldBindingsFiles[i].c_str());
            remove(oldBindingsFiles[i].c_str());
        }
    }
}

Generator::Generator()
{
}

Generator::~Generator()
{
}

void Generator::getFunctions(XMLElement* fileNode, string ns)
{
    // Process all public, non-static functions and variables that are defined in this file.
    XMLElement* node = NULL;
    const char* kind = NULL;
    const char* isStatic = NULL;
    for (node = fileNode->FirstChildElement("sectiondef"); node != NULL; node = node->NextSiblingElement("sectiondef"))
    {
        kind = node->Attribute("kind");
        if (kind && strcmp(kind, "func") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                isStatic = e->Attribute("static");
                if (e->FirstChildElement("templateparamlist"))
                {
                    __printTemplateWarning = true;
                }
                else if (isExport(e) &&
                    (!isStatic || strcmp(isStatic, "yes") != 0))
                {
                    FunctionBinding b;
                    b.type = FunctionBinding::GLOBAL_FUNCTION;
                    b.name = getName(e);
					b.ns = ns;
					b.include = getIncludeFlag(e);

                    if (!isReservedKeyword(b.name))
                    {
                        // Ignore operators.
                        if (b.name.find("operator") != 0)
                        {
                            b.returnParam = getParam(e);

                            getParams(e, b);
                            getCreateFlag(e, b);
                            _functions[b.getFunctionName()].push_back(b);

                            // Add the function to the set of namespace members if it is part of a namespace.
                            if (ns.size() > 0)
                                _namespaces[ns].insert(b.name);
                        }
                        else
                        {
                            __printOperatorWarning = true;
                        }
                    }
                    else
                    {
                        __warnings.insert(string("Function name '") + b.name + string("' is a reserved Lua keyword; binding '") + b.getFunctionName() + string("' was not generated."));
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        /*else if (kind && strcmp(kind, "var") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                isStatic = e->Attribute("static");
                if (isExport(e) &&
                    (!isStatic || strcmp(isStatic, "yes") != 0))
                {
                    FunctionBinding b;
                    if (getIsConstType(e))
                        b.type = FunctionBinding::GLOBAL_CONSTANT;
                    else
                        b.type = FunctionBinding::GLOBAL_VARIABLE;

                    b.name = getName(e);
                    if (!isReservedKeyword(b.name))
                    {
                        b.returnParam = getParam(e, true);
                        _functions[b.getFunctionName()].push_back(b);
                        
                        // Add the variable to the set of namespace members if it is part of a namespace.
                        if (ns.size() > 0)
                            _namespaces[ns].insert(b.name);
                    }
                    else
                    {
                        __warnings.insert(string("Function name '") + b.name + string("' is a reserved Lua keyword; binding '") + b.getFunctionName() + string("' was not generated."));
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }*/
    }
}

void Generator::getNamespace(XMLElement* nsNode, const string& name)
{
    // Skip the 'std' namespace.
    if (name == "std")
        return;

    // Create the set of all classes that are a part of this namespace.
    XMLElement* e = nsNode->FirstChildElement("innerclass");
    while (e)
    {
        string classname = e->GetText();
        size_t i = classname.find("::");
        if (i != classname.npos)
            classname = classname.substr(i + 2);
        _namespaces[name].insert(classname);
        e = e->NextSiblingElement("innerclass");
    }

    // Parse all functions that are directly in this namespace.
    getFunctions(nsNode, name);

    // Parse all enums that are directly in this namespace.
    getEnums(nsNode, name);

    // Parse all typedefs.
    getTypedefs(nsNode, name);
}

void Generator::getClass(XMLElement* classNode, const string& name)
{
    // Get the ref id for the class.
    string refId = classNode->Attribute("id");
	string flag = getScriptFlag(classNode);

	bool __export = false;
	bool __api = false;
	bool __script = false;
	if (flag == __exportTag)
	{
		__export = true;
	}
	else if (flag == Interface_TAG)
	{
		__api = true;
	}
	else if (!flag.empty())
	{
		__script = true;
	}

    // Create the class binding object that we will store the function bindings in (name -> binding).
    ClassBinding classBinding(name, refId, __export, __script);

    // Store the mapping between the ref id and the class's fully qualified name.
    Generator::getInstance()->setIdentifier(refId, classBinding.classname);

    // Check if we should ignore this class.
    if (!__api && !__export && !__script)
        return;

    // Get the include header for the original class declaration.
    XMLElement* includeElement = classNode->FirstChildElement("includes");
    if (includeElement)
    {
        const char* text = includeElement->GetText();
        if (!text)
        {
            GP_ERROR("The 'includes' node is empty (a class must have an include file) for XML document '%s'.", _file);
            return;
        }
        classBinding.include = text;
    }
    else
    {
        // See if we are an inner class. If so, attempt to get the include from the root parent class.
        size_t index = classBinding.classname.find("::");
        if (index != classBinding.classname.npos)
        {
            string parentClassName = classBinding.classname.substr(0, index);
            if (_classes.find(parentClassName) != _classes.end())
                classBinding.include = _classes[parentClassName].include;
        }
        else
        {
            // Attempt to guess the name of the header.
            classBinding.include = classBinding.classname + ".h";
        }
    }

    // Track whether the class has any pure virtual functions.
    bool hasPureVirtual = false;

    // Process all public members that are defined in this class.
    XMLElement* node = NULL;
    const char* kind = NULL;
    for (node = classNode->FirstChildElement("sectiondef"); node != NULL; node = node->NextSiblingElement("sectiondef"))
    {
        kind = node->Attribute("kind");
        if (!kind)
        {
            GP_ERROR("Section definition is missing required 'kind' attribute in XML document  '%s'.", _file);
            return;
        }

        // Process public static functions.
        if (strcmp(kind, "public-static-func") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (e->FirstChildElement("templateparamlist"))
                {
                    __printTemplateWarning = true;
                }
                else if (isExport(e))
                {
                    FunctionBinding b(classBinding.classname, classBinding.uniquename);
                    b.type = FunctionBinding::STATIC_FUNCTION;

                    b.name = getName(e);
                    if (!isReservedKeyword(b.name))
                    {
                        b.returnParam = getParam(e, false, b.classname);
                        
                        getParams(e, b);
                        getCreateFlag(e, b);
                        classBinding.bindings[b.getFunctionName()].push_back(b);
                    }
                    else
                    {
                        __warnings.insert(string("Function name '") + b.name + string("' is a reserved Lua keyword; binding '") + b.getFunctionName() + string("' was not generated."));
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        // Process public member functions.
        else if (strcmp(kind, "public-func") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (e->FirstChildElement("templateparamlist"))
                {
                    __printTemplateWarning = true;
                }
                else
                {
                    if (strcmp(e->Attribute("virt"), "pure-virtual") == 0)
                        hasPureVirtual = true;

                    FunctionBinding b(classBinding.classname, classBinding.uniquename);
                    b.type = FunctionBinding::MEMBER_FUNCTION;

                    b.name = getName(e);

                    if (!isReservedKeyword(b.name))
                    {
                        // Ignore operators.
                        if (b.name.find("operator") != 0)
                        {
                            b.returnParam = getParam(e, false, b.classname);

                            getParams(e, b);

							if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
							{
								b.returnParam.info = refId;
								b.own = true;
								if (b.paramTypes.size() == 0)
								{
									FunctionBinding::Param p(FunctionBinding::Param::TYPE_TABLE);
									p.hasDefaultValue = true;
									b.paramTypes.push_back(p);
								}

								classBinding.inaccessibleConstructor = true;
							}
							else if (b.returnParam.type == FunctionBinding::Param::TYPE_DESTRUCTOR)
							{
								classBinding.inaccessibleDestructor = true;
							}

                            getCreateFlag(e, b);
							if (isExport(e))
								classBinding.bindings[b.getFunctionName()].push_back(b);

							//if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR &&
							//	b.getMinParamCount() == 0)
							//{
							//	FunctionBinding::Param p(FunctionBinding::Param::TYPE_TABLE);
							//	b.paramTypes.push_back(p);
							//	classBinding.bindings[b.getFunctionName()].push_back(b);
							//}
                        }
                        else
                        {
                            __printOperatorWarning = true;
                        }
                    }
                    else
                    {
                        __warnings.insert(string("Function name '") + b.name + string("' is a reserved Lua keyword; binding '") + b.getFunctionName() + string("' was not generated."));
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        // Process public static variables.
        else if (strcmp(kind, "public-static-attrib") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (isExport(e))
                {
                    FunctionBinding b(classBinding.classname, classBinding.uniquename);
                    if (getIsConstType(e))
                        b.type = FunctionBinding::STATIC_CONSTANT;
                    else
                        b.type = FunctionBinding::STATIC_VARIABLE;

                    b.name = getName(e);
                    if (!isReservedKeyword(b.name))
                    {
                        b.returnParam = getParam(e, true, b.classname);
                        classBinding.bindings[b.getFunctionName()].push_back(b);
                    }
                    else
                    {
                        __warnings.insert(string("Function name '") + b.name + string("' is a reserved Lua keyword; binding '") + b.getFunctionName() + string("' was not generated."));
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        // Process public member variables.
		else if (strcmp(kind, "public-attrib") == 0)
		{
			XMLElement* e = node->FirstChildElement("memberdef");
			while (e)
			{
				if (e->FirstChildElement("templateparamlist"))
				{
					__printTemplateWarning = true;
				}
				else if (isExport(e))
				{
					FunctionBinding b(classBinding.classname, classBinding.uniquename);
					if (getIsConstType(e))
						b.type = FunctionBinding::MEMBER_CONSTANT;
					else
						b.type = FunctionBinding::MEMBER_VARIABLE;

					b.name = getName(e);
					if (!isReservedKeyword(b.name))
					{
						b.returnParam = getParam(e, true, b.classname);
						classBinding.bindings[b.getFunctionName()].push_back(b);
					}
					else
					{
						__warnings.insert(string("Function name '") + b.name + string("' is a reserved Lua keyword; binding '") + b.getFunctionName() + string("' was not generated."));
					}
				}
				e = e->NextSiblingElement("memberdef");
			}
		}
        else if (strcmp(kind, "protected-func") == 0 ||
                 strcmp(kind, "private-func") == 0)
        {
            // Check for inaccessible constructor/destructor
            // and create a list of all other inaccessible functions
            // (we do this so that classes that override a public function
            // and change its scope to protected or private work properly
            // from Lua-i.e. no binding is generated for such functions).
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (strcmp(e->Attribute("virt"), "pure-virtual") == 0)
                    hasPureVirtual = true;

                FunctionBinding::Param t = getParam(e);
                if (t.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
                {
                    classBinding.inaccessibleConstructor = true;
                }
                else if (t.type == FunctionBinding::Param::TYPE_DESTRUCTOR)
                {
                    classBinding.inaccessibleDestructor = true;
                }
                else
                {
                    FunctionBinding b(classBinding.classname, classBinding.uniquename);
                    b.type = FunctionBinding::MEMBER_FUNCTION;

                    b.name = getName(e);
                
                    if (!isReservedKeyword(b.name))
                    {
                        // Ignore operators.
                        if (b.name.find("operator") != 0)
                        {
                            b.returnParam = getParam(e, false, b.classname);
                            if (b.returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
                            {
                                b.returnParam.info = refId;
                                b.own = true;
                            }

                            getParams(e, b);
                            getCreateFlag(e, b);
                            classBinding.hidden[b.getFunctionName()].push_back(b);
                        }
                        else
                        {
                            __printOperatorWarning = true;
                        }
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        else if (strcmp(kind, "protected-static-func") == 0 ||
                 strcmp(kind, "private-static-func") == 0)
        {
            // Create a list of all other inaccessible functions
            // (we do this so that classes that override a public function
            // and change its scope to protected or private work properly
            // from Lua-i.e. no binding is generated for such functions).

            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (e->FirstChildElement("templateparamlist"))
                {
                    __printTemplateWarning = true;
                }
                else if (isExport(e))
                {
                    FunctionBinding b(classBinding.classname, classBinding.uniquename);
                    b.type = FunctionBinding::STATIC_FUNCTION;

                    b.name = getName(e);
                    if (!isReservedKeyword(b.name))
                    {
                        b.returnParam = getParam(e, false, b.classname);
                        
                        getParams(e, b);
                        getCreateFlag(e, b);
                        classBinding.hidden[b.getFunctionName()].push_back(b);
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        // Process public enums and typedefs.
        else if (strcmp(kind, "public-type") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (isExport(e))
                {
                    if (strcmp(e->Attribute("kind"), "enum") == 0)
                    {
                        getEnum(e, &classBinding, classBinding.ns, classBinding.include);
                    }
                    else if (strcmp(e->Attribute("kind"), "typedef") == 0)
                    {
                        getTypedef(e, classBinding.classname, classBinding.ns);
                    }
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
        else if (strcmp(kind, "user-defined") == 0)
        {
			XMLElement* e = node->FirstChildElement("header");
			if (e)
			{
				const char* kind = e->GetText();
				if (strcmp(kind, "Callbacks") == 0)
				{
					e = node->FirstChildElement("memberdef");
					while (e)
					{
						if (e->FirstChildElement("templateparamlist"))
						{
							__printTemplateWarning = true;
						}
						else if (getName(e).compare("DECLARE_CALLBACK") == 0)
						{
							FunctionBinding b(classBinding.classname, classBinding.uniquename);
							b.type = FunctionBinding::MEMBER_CALLBACK;

							XMLElement* param = e->FirstChildElement("param");
							if (param)
							{
								b.returnParam = getParam(param, false, b.classname);
							}

							param = param->NextSiblingElement("param");
							if (param)
							{
								b.name = param->FirstChildElement("type")->FirstChild()->Value();
							}

							param = param->NextSiblingElement("param");
							while (param)
							{
								FunctionBinding::Param p = getParam(param, false, b.classname);
								if (p.type != FunctionBinding::Param::TYPE_VARARGS && p.type != FunctionBinding::Param::TYPE_VOID)
									b.paramTypes.push_back(p);
								param = param->NextSiblingElement("param");
							}
							classBinding.callbacks[b.name] = b;
						}
						e = e->NextSiblingElement("memberdef");
					}
				}
			}
        }
		else
		{
		}
    }

    // If the class has a pure-virtual method, we mark the constructor as inaccessible
    // and remove any constructor bindings that are in the list.
    if (hasPureVirtual)
    {
        classBinding.inaccessibleConstructor = true;
        map<string, vector<FunctionBinding> >::iterator iter = classBinding.bindings.begin();
        for (; iter != classBinding.bindings.end(); iter++)
        {
            if (iter->second[0].returnParam.type == FunctionBinding::Param::TYPE_CONSTRUCTOR)
            {
                classBinding.bindings.erase(iter);
                break;
            }
        }
    }

	node = classNode->FirstChildElement("basecompoundref");
	while (node)
	{
		if (node->Attribute("refid"))
			classBinding.base.push_back(node->Attribute("refid"));
		node = node->NextSiblingElement("basecompoundref");
	}

    // Store the class's derived class(es)' ref id(s) if it has any.
    node = classNode->FirstChildElement("derivedcompoundref");
    while (node)
    {
        classBinding.derived.push_back(node->Attribute("refid"));
        node = node->NextSiblingElement("derivedcompoundref");
    }

    // If the class has no base classes and it has derived classes, add it to the list of top-level classes.
    if (__api || (classBinding.base.size() == 0 && classBinding.derived.size() > 0))
        _topLevelBaseClasses.push_back(classBinding.classname);

    // Insert the class binding into the global table.
    _classes[classBinding.classname] = classBinding;
}

void Generator::getEnums(XMLElement* fileNode, string ns, string include)
{
    XMLElement* node = NULL;
    const char* kind = NULL;
    for (node = fileNode->FirstChildElement("sectiondef"); node != NULL; node = node->NextSiblingElement("sectiondef"))
    {
        kind = node->Attribute("kind");
        if (!kind)
        {
            GP_ERROR("Section definition is missing required 'kind' attribute in XML document  '%s'.", _file);
            return;
        }

        // Process the enums.
        if (strcmp(kind, "enum") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (isExport(e) && strcmp(e->Attribute("kind"), "enum") == 0)
                {
					string userInclude = getIncludeFlag(e);
					getEnum(e, NULL, ns, userInclude.empty() ? include : userInclude);
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
    }
}

void Generator::getEnum(XMLElement* e, ClassBinding* classbinding, string ns, string include)
{
    // Get the enumeration's name.
	string enumname;
	if (classbinding != NULL)
		enumname += classbinding->classname;
	if (enumname.size() > 0)
		enumname += string("::");
	enumname += getName(e);

    // Get the ref id for the enum.
    string refId = e->Attribute("id");

    // Store the ref id to name mapping.
    Generator::getInstance()->setIdentifier(refId, enumname);
    
    // Add the enum to the enum list if it's not there 
    // (we implicitly create it when we set its scope path).
    if (_enums.find(enumname) == _enums.end())
	{
		_enums[enumname].name = getName(e);
        _enums[enumname].scopePath = getScopePath(enumname, ns);
	}

	if (classbinding != NULL)
	{
		_enums[enumname].isGlobal = false;
		classbinding->_enums.push_back(enumname);
	}
	else
		_enums[enumname].isGlobal = true;

    // Set the include file for the enum.
    if (include.size() > 0)
        _enums[enumname].include = include;
    else
    {
        XMLElement* location = e->FirstChildElement("location");
        if (location)
            _enums[enumname].include = location->Attribute("file");
    }

    for (e = e->FirstChildElement("enumvalue"); e; e = e->NextSiblingElement("enumvalue"))
    {
        _enums[enumname].values.push_back(getName(e));
    }

    // Add the enum to the set of namespace members if it is part of a namespace.
    if (ns.size() > 0)
        _namespaces[ns].insert(enumname);
}

void Generator::getTypedefs(XMLElement* fileNode, string ns, string include)
{
    XMLElement* node = NULL;
    const char* kind = NULL;
    for (node = fileNode->FirstChildElement("sectiondef"); node != NULL; node = node->NextSiblingElement("sectiondef"))
    {
        kind = node->Attribute("kind");
        if (!kind)
        {
            GP_ERROR("Section definition is missing required 'kind' attribute in XML document  '%s'.", _file);
            return;
        }

        // Process the enums.
        if (strcmp(kind, "typedef") == 0)
        {
            XMLElement* e = node->FirstChildElement("memberdef");
            while (e)
            {
                if (/*isExport(e) &&*/ strcmp(e->Attribute("kind"), "typedef") == 0)
                {
                    getTypedef(e, "", ns);
                }
                e = e->NextSiblingElement("memberdef");
            }
        }
    }
}


void Generator::getStructs( XMLElement* fileNode, string ns /*= ""*/, string include )
{
	XMLElement* node = NULL;
	const char* kind = NULL;
	for (node = fileNode->FirstChildElement("sectiondef"); node != NULL; node = node->NextSiblingElement("sectiondef"))
	{
		kind = node->Attribute("kind");
		if (!kind)
		{
			GP_ERROR("Section definition is missing required 'kind' attribute in XML document  '%s'.", _file);
			return;
		}

		// Process the enums.
		if (strcmp(kind, "user-defined") == 0 && strcmp(node->FirstChildElement("header")->GetText(), "Structs") == 0)
		{
			XMLElement* e = node->FirstChildElement("memberdef");
			while (e)
			{
				if (strcmp(e->Attribute("kind"), "function") == 0 && getName(e).compare("DECLARE_CONTYPE") == 0)
				{
					getStruct(e, include, ns);
				}
				e = e->NextSiblingElement("memberdef");
			}
		}
	}
}

void Generator::getTypedef(XMLElement* e, string classname, string ns)
{
    TypedefBinding t;

    // Get the typedef's name.
    string tname = classname;
    if (tname.size() > 0)
        tname += string("::");
    tname += getName(e);

    // Get the typedef's id and store the mapping from ref id to name.
    string refId = e->Attribute("id");
    setIdentifier(refId, tname);

    //FunctionBinding::Param p = getParam(e);
    //if (p.info.size() > 0)
    //    t.refId = p.info;
	t.p = getParam(e);

    _typedefs[tname] = t;

    // Add the typedef to the set of namespace members if it is part of a namespace.
    if (ns.size() > 0)
        _namespaces[ns].insert(tname);
}

void Generator::getStruct( XMLElement* e, string include, string ns )
{
	XMLElement* param = e->FirstChildElement("param");
	if (param)
	{
		//param = param->NextSiblingElement("param");
		//if (param)
		{
			FunctionBinding::Param type = getParam(param);
			_structs.insert(make_pair(type.info, include));
		}
	}
}

static inline bool getScriptFlagHelper(XMLElement* e, string& flag)
{
    XMLElement* x = e->FirstChildElement("xmlonly");
    if (x)
    {
        flag = x->GetText();
        return true;
    }

    x = e->FirstChild() ? e->FirstChild()->ToElement() : NULL;
    while (x)
    {
        if (getScriptFlagHelper(x, flag))
            return true;

        x = x->NextSibling() ? x->NextSibling()->ToElement() : NULL;
    }
    return false;
}

string Generator::getScriptFlag(XMLElement* e)
{
    string flag;

    XMLElement* n = e->FirstChildElement("detaileddescription");
    if (n)
        getScriptFlagHelper(n, flag);
    
    return flag;
}

static inline bool getIncludeFlagHelper(XMLElement* e, string& flag)
{
	XMLElement* x = e->FirstChildElement("include");
	if (x)
	{
		flag = x->GetText();
		return true;
	}

	x = e->FirstChild() ? e->FirstChild()->ToElement() : NULL;
	while (x)
	{
		if (getIncludeFlagHelper(x, flag))
			return true;

		x = x->NextSibling() ? x->NextSibling()->ToElement() : NULL;
	}
	return false;
}

string Generator::getIncludeFlag( XMLElement* e )
{
	string flag;

	XMLElement* n = e->FirstChildElement("detaileddescription");
	if (n)
		getIncludeFlagHelper(n, flag);

	return flag;
}

void Generator::getCreateFlag(XMLElement* e, FunctionBinding& b)
{
    if (getScriptFlag(e) == "create")
        b.own = true;
}

string Generator::getName(XMLElement* e)
{
    // Get the name for the binding.
    XMLElement* nameElement = e->FirstChildElement("name");
    if (!nameElement)
    {
        GP_ERROR("Missing 'name' child node in XML document '%s'.", _file);
        return string();
    }
    return nameElement->GetText();
}

bool Generator::getIsConstType(XMLElement* e)
{
    XMLElement* typeElement = e->FirstChildElement("type");
    if (!typeElement)
    {
        GP_ERROR("Missing 'type' node in XML document '%s'.", _file);
        return false;
    }

    XMLNode* textNode = typeElement->FirstChild();
    if (textNode)
    {
        XMLText* text = textNode->ToText();
        if (text)
        {
            string type = text->Value();
            return type.find("const") != type.npos;
        }
    }
    return false;
}

FunctionBinding::Param Generator::getParam(XMLElement* e, bool isVariable, string classname)
{
    // Get the type for the current element (either the return type or a parameter type).
    XMLElement* typeElement = e->FirstChildElement("type");
    if (!typeElement)
    {
        GP_ERROR("Missing 'type' node in XML document '%s'.", _file);
    }

    FunctionBinding::Param p;
    XMLNode* textNode = typeElement->FirstChild();
    if (!textNode)
    {
        // Get the name of the current element to see if it is a destructor.
        string name = getName(e);
        if (name.find("~") == 0)
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_DESTRUCTOR);
        }
        else if (name.compare(classname) == 0)
        {
            // Assume it's a constructor.
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_CONSTRUCTOR, FunctionBinding::Param::KIND_POINTER, name);
        }
    }
    else
    {
        // Get the type string without const or reference qualifiers (and trim whitespace).
        string refId = "";
        string typeStr = "";
        int levelsOfIndirection = 0;
		bool outParam = false;
        FunctionBinding::Param::Kind kind;
        {
            // Attempt to process the type as reference (i.e. class, struct, enum, typedef, etc.) type.
            XMLNode* node = textNode;
            while (node)
            {
                if (strcmp(node->Value(), "ref") == 0)
                {
                    refId = node->ToElement()->Attribute("refid");
                    typeStr += node->ToElement()->GetText();
                }
                else if (node->ToText())
                {
                    typeStr += node->ToText()->Value();
                }

                node = node->NextSibling();
            }

            typeStr = stripTypeQualifiers(typeStr, kind, levelsOfIndirection, outParam);
        }

        if (typeStr == "void")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_VOID, kind);
        }
        else if (typeStr == "bool")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_BOOL, kind);
        }
        else if (typeStr == "char" && kind != FunctionBinding::Param::KIND_POINTER)
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_CHAR, kind);
        }
        else if (typeStr == "short")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_SHORT, kind);
        }
        else if (typeStr == "int")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_INT, kind);
        }
        else if (typeStr == "long")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_LONG, kind);
        }
        else if (typeStr == "unsigned char")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_UCHAR, kind);
        }
        else if (typeStr == "unsigned short")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_USHORT, kind);
        }
        else if (typeStr == "unsigned int")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_UINT, kind);
        }
		/*else if (typeStr == "uint64")
		{
			p = FunctionBinding::Param(FunctionBinding::Param::TYPE_UINT64, kind);
		}*/
        else if (typeStr == "unsigned long")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_ULONG, kind);
        }
        else if (typeStr == "float")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_FLOAT, kind);
        }
        else if (typeStr == "double")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_DOUBLE, kind);
        }
        else if (typeStr == "String" || typeStr == "std::string")
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_STRING, kind, "string");
        }
        else if (typeStr == "char" && kind == FunctionBinding::Param::KIND_POINTER)
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_STRING, kind, "char*");
        }
        else if (_enums.find(typeStr) != _enums.end() ||
            (classname.size() > 0 && _enums.find(classname + string("::") + typeStr) != _enums.end()))
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_ENUM, kind, refId);
        }
        else if (typeStr == "...")
        {
            __printVarargWarning = true;
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_VARARGS);
        }
		else if (typeStr == "RefHandle")
		{
			p = FunctionBinding::Param(FunctionBinding::Param::TYPE_REF, kind);
		}
		else if (typeStr == "FuncHandle")
		{
			p = FunctionBinding::Param(FunctionBinding::Param::TYPE_FUNCTION, kind);
		}
        else
        {
            p = FunctionBinding::Param(FunctionBinding::Param::TYPE_UNRECOGNIZED, kind, (refId.size() > 0) ? refId : typeStr);
        }

		if (levelsOfIndirection > 1)
		{
			__printMulPointWarning = true;
		}

        p.levelsOfIndirection = levelsOfIndirection;
		p.outParam = outParam;

        // Check if the type is a pointer declared with square brackets (i.e. float x[4]).
        XMLElement* arrayElement = NULL;
        if ((arrayElement = e->FirstChildElement("array")) != NULL ||
            (isVariable && (arrayElement = e->FirstChildElement("argsstring")) != NULL))
        {
            const char* text = arrayElement->GetText();
            string arrayString = (text ? text : "");
            string::size_type i = arrayString.find("[");
            string::size_type k = arrayString.find("]");
            if (i != arrayString.npos && k != arrayString.npos)
            {
                p.kind = FunctionBinding::Param::KIND_POINTER;
                p.levelsOfIndirection = 1;
                if (i != k - 1)
                    p.info = arrayString.substr(i + 1, k - (i + 1));
            }
        }

		XMLElement* nameElement = NULL;
		if ((arrayElement = e->FirstChildElement("declname")) != NULL)
		{
			p.name = arrayElement->FirstChild()->Value();
		}
    }

    // Get the default value for the parameter if it has one.
    XMLElement* defaultValueElement = e->FirstChildElement("defval");
    if (defaultValueElement)
    {
        p.hasDefaultValue = true;
    }

    return p;
}

void Generator::getParams(XMLElement* e, FunctionBinding& b)
{
    XMLElement* param = e->FirstChildElement("param");
    while (param)
    {
        FunctionBinding::Param p = getParam(param, false, b.classname);
        if (p.type != FunctionBinding::Param::TYPE_VARARGS && p.type != FunctionBinding::Param::TYPE_VOID)
            b.paramTypes.push_back(p);
        param = param->NextSiblingElement("param");
    }
}

void Generator::resolveMembers(const ClassBinding& c)
{
    vector<ClassBinding*> derived;

    // Go through the derived classes' bindings and update them.
    ClassBinding* cb = NULL;
    map<string, vector<FunctionBinding> >::const_iterator iter;
    for (unsigned int i = 0; i < c.derived.size(); i++)
    {
        // If the class is not in the map of classes, then
        // it was marked to be ignored, so skip over it.
        string derivedClassName = getIdentifier(c.derived[i]);
        if (_classes.find(derivedClassName) == _classes.end())
            continue;

        cb = &_classes[derivedClassName];

		if (cb->base[0].compare(c.refId) == 0)
		{
			derived.push_back(cb);
			continue;
		}

        // Go through this class' bindings and add them to the current
        // derived class if they are not already there (excluding constructors and destructors).
        for (iter = c.bindings.begin(); iter != c.bindings.end(); iter++)
        {
            FunctionBinding b(cb->classname, cb->uniquename);
            b.type = iter->second[0].type;
            b.name = iter->second[0].name;

            map<string, vector<FunctionBinding> >::iterator findIter = cb->bindings.find(b.getFunctionName());
            map<string, vector<FunctionBinding> >::iterator hiddenIter = cb->hidden.find(b.getFunctionName());
            if (findIter == cb->bindings.end() && hiddenIter == cb->hidden.end())
            {
                for (unsigned int i = 0, iCount = iter->second.size(); i < iCount; i++)
                {
                    if (iter->second[i].returnParam.type != FunctionBinding::Param::TYPE_CONSTRUCTOR &&
                        iter->second[i].returnParam.type != FunctionBinding::Param::TYPE_DESTRUCTOR)
                    {
                        // Copy the binding, but set its function name to 
                        // empty so that it is regenerated correctly.
                        b = iter->second[i];
                        b.functionName = "";
                        b.classname = cb->classname;
                        b.uniquename = cb->uniquename;

                        cb->bindings[b.getFunctionName()].push_back(b);
                    }
                }
            }
            else
            {
                bool addBinding;
                for (unsigned int i = 0, iCount = iter->second.size(); i < iCount; i++)
                {
                    string name = iter->second[i].name;

                    addBinding = true;
                    if (findIter != cb->bindings.end())
                    {
                        for (unsigned int j = 0, jCount = findIter->second.size(); j < jCount; j++)
                        {
                            if (FunctionBinding::signaturesMatch(iter->second[i], findIter->second[j]))
                            {
                                addBinding = false;
                                break;
                            }
                        }

                        // To call the base function, we have to qualify the call since
                        // the derived class has a function with the same name and different parameters.
                        if (addBinding)
                            name = iter->second[i].classname + string("::") + iter->second[i].name;
                    }
                    if (hiddenIter != cb->hidden.end())
                    {
                        for (unsigned int j = 0, jCount = hiddenIter->second.size(); j < jCount; j++)
                        {
                            if (FunctionBinding::signaturesMatch(iter->second[i], hiddenIter->second[j]))
                            {
                                addBinding = false;
                                break;
                            }
                        }
                    }

                    if (addBinding)
                    {
                        FunctionBinding b = iter->second[i];
                        b.name = name;
                        b.functionName = findIter->first;
                        b.classname = cb->classname;
                        b.uniquename = getUniqueName(cb->classname);

                        if (findIter != cb->bindings.end())
                            findIter->second.push_back(b);
                        else
                            cb->bindings[b.getFunctionName()].push_back(b);
                    }
                }
            }
        }

        derived.push_back(cb);
    }

    // Go through the derived classes' bindings and resolve the members for their derived classes.
    for (unsigned int i = 0; i < derived.size(); i++)
    {
        resolveMembers(*derived[i]);
    }
}

void Generator::resolveInheritance()
{
    // Go through the class inheritance tree and update each class's bindings list 
    // to include the bindings from the base class that haven't been overridden.
    for (unsigned int i = 0; i < _topLevelBaseClasses.size(); i++)
    {
        resolveMembers(_classes[_topLevelBaseClasses[i]]);
    }
}

void Generator::resolveIncludes(const ClassBinding& c)
{
    vector<ClassBinding*> derived;

    // Go through the derived classes' bindings and update them.
    ClassBinding* cb = NULL;
    map<string, vector<FunctionBinding> >::const_iterator iter;
    for (unsigned int i = 0; i < c.derived.size(); i++)
    {
        // If the class is not in the map of classes, then
        // it was marked to be ignored, so skip over it.
        string derivedClassName = getIdentifier(c.derived[i]);
        if (_classes.find(derivedClassName) == _classes.end())
            continue;

        cb = &_classes[derivedClassName];

        // Add all include files (uniquely) from the base class to the derived class.
        map<string, set<string> >::iterator baseIncludes = _includes.find(c.include);
        if (baseIncludes != _includes.end() && baseIncludes->second.size() > 0)
        {
            set<string>::iterator iter = baseIncludes->second.begin();
            set<string>& derivedIncludes = _includes[cb->include];
            for (; iter != baseIncludes->second.end(); iter++)
            {
                derivedIncludes.insert(*iter);
            }
        }
        
        derived.push_back(cb);
    }

    // Go through the derived classes' bindings and resolve the members for their derived classes.
    for (unsigned int i = 0; i < derived.size(); i++)
    {
        resolveIncludes(*derived[i]);
    }
}

void Generator::resolveInheritedIncludes()
{
    // Go through the class inheritance tree and update each class's 
    // list of includes to include the includes from the base class
    for (unsigned int i = 0; i < _topLevelBaseClasses.size(); i++)
    {
        resolveIncludes(_classes[_topLevelBaseClasses[i]]);
    }
}

void Generator::resolveType(FunctionBinding::Param* param, string functionName, string header)
{
    string name = getIdentifier(param->info);

    while (param->type == FunctionBinding::Param::TYPE_UNRECOGNIZED)
    {
        map<string, TypedefBinding>::iterator typedefIter = _typedefs.find(name);
        if (typedefIter != _typedefs.end() && /*typedefIter->second.refId.size() > 0*/typedefIter->second.p.type != FunctionBinding::Param::TYPE_UNRECOGNIZED)
        {
            param->type = typedefIter->second.p.type;
            param->info = typedefIter->second.refId;
        }
        else
        {
            map<string, ClassBinding>::iterator classIter = _classes.find(name);
            if (classIter != _classes.end())
            {
                param->type = FunctionBinding::Param::TYPE_OBJECT;
            }
            else
            {
				map<string,string>::iterator structIter = _structs.find(param->info);
				if (structIter != _structs.end())
				{
					param->type = FunctionBinding::Param::TYPE_STRUCT;
				}
				else
				{
					map<string, EnumBinding>::iterator enumIter = _enums.find(name);
					if (enumIter != _enums.end())
					{
						param->type = FunctionBinding::Param::TYPE_ENUM;
					}
					else
					{
						param->type = FunctionBinding::Param::TYPE_OBJECT;
						__warnings.insert(string("Unrecognized C++ type: ") + functionName + string(" -- ") + (name.size() > 0 ? name : param->info));
					}
				}
            }
        }
    }
}

void Generator::resolveTypes()
{
    // Go through all member functions and attempt to resolve unrecognized types.
    for (map<string, ClassBinding>::iterator iter = _classes.begin(); iter != _classes.end(); iter++)
    {
		set<string>& includes = _includes[iter->second.include];
        map<string, vector<FunctionBinding> >::iterator functionNameIter = iter->second.bindings.begin();
        for (; functionNameIter != iter->second.bindings.end(); functionNameIter++)
        {
            vector<FunctionBinding>::iterator functionIter = functionNameIter->second.begin();
            for (; functionIter != functionNameIter->second.end(); functionIter++)
            {
                resolveType(&functionIter->returnParam, iter->first + string("::") + functionIter->name, iter->second.include);

				if (functionIter->returnParam.type == FunctionBinding::Param::TYPE_OBJECT)
				{
					std::string className = getTypeName(functionIter->returnParam);
					if (_classes.find(className) != _classes.end())
						includes.insert(_classes[className].include);
				}
				else if (functionIter->returnParam.type == FunctionBinding::Param::TYPE_STRUCT)
				{
					if (_structs.find(functionIter->returnParam.info) != _structs.end())
						includes.insert(_structs[functionIter->returnParam.info]);
				}

                vector<FunctionBinding::Param>::iterator paramIter = functionIter->paramTypes.begin();
                for (; paramIter != functionIter->paramTypes.end(); paramIter++)
                {
                    resolveType(&(*paramIter), iter->first + string("::") + functionIter->name, iter->second.include);
					if (paramIter->type == FunctionBinding::Param::TYPE_OBJECT)
					{
						std::string className = getTypeName(*paramIter);
						if (_classes.find(className) != _classes.end())
							includes.insert(_classes[className].include);
					}
					else if (paramIter->type == FunctionBinding::Param::TYPE_STRUCT)
					{
						if (_structs.find(paramIter->info) != _structs.end())
							includes.insert(_structs[paramIter->info]);
					}
                }
            }
        }

		map<string,FunctionBinding>::iterator callbackIter = iter->second.callbacks.begin();
		for (; callbackIter != iter->second.callbacks.end(); callbackIter++)
		{
			resolveType(&callbackIter->second.returnParam, iter->first + string("::") + callbackIter->second.name, iter->second.include);

			vector<FunctionBinding::Param>::iterator paramIter = callbackIter->second.paramTypes.begin();
			for (; paramIter != callbackIter->second.paramTypes.end(); paramIter++)
			{
				resolveType(&(*paramIter), iter->first + string("::") + callbackIter->second.name, iter->second.include);

				if (paramIter->type == FunctionBinding::Param::TYPE_OBJECT)
				{
					std::string className = getTypeName(*paramIter);
					includes.insert(_classes[className].include);
				}
				else if (paramIter->type == FunctionBinding::Param::TYPE_STRUCT)
				{
					string name = getIdentifier(paramIter->info);
					includes.insert(_structs[name]);
				}
			}
		}
    }

    // Go through all non-member functions and attempt to resolve unrecognized types.
    string globalHeader = __globalfile + string(".h");
	set<string>& includes = _includes[globalHeader];
    for (map<string, vector<FunctionBinding> >::iterator iter = _functions.begin(); iter != _functions.end(); iter++)
    {
        vector<FunctionBinding>::iterator functionIter = iter->second.begin();
        for (; functionIter != iter->second.end(); functionIter++)
        {
			if (functionIter->include.size() > 0 && includes.find(functionIter->include) == includes.end())
				includes.insert(functionIter->include);
            resolveType(&functionIter->returnParam, functionIter->name, globalHeader);

            vector<FunctionBinding::Param>::iterator paramIter = functionIter->paramTypes.begin();
            for (; paramIter != functionIter->paramTypes.end(); paramIter++)
            {
                resolveType(&(*paramIter), functionIter->name, globalHeader);
            }
        }
    }

	for (map<string, EnumBinding>::iterator iter = _enums.begin(); iter != _enums.end(); iter++)
	{
		if (iter->second.include.size() > 0 && includes.find(iter->second.include) == includes.end())
			includes.insert(iter->second.include);
	}
}

void Generator::generateBindings(string* bindingNS)
{
    bool generatingGameplay = false;
    if (bindingNS && (*bindingNS == "gameplay"))
        generatingGameplay = true;

    string luaAllHStr = _outDir + __allbindingfile + string(".h");
    ostringstream luaAllH;
    string includeGuard = __allbindingfile + string("_H_");
    transform(includeGuard.begin(), includeGuard.end(), includeGuard.begin(), ::toupper);
    luaAllH << "#ifndef " << includeGuard << "\n";
    luaAllH << "#define " << includeGuard << "\n\n";
	luaAllH << "class LuaState;\n";
	//luaAllH << "#include \"LuaEx.h\"\n\n";
    
    string luaAllCppStr = _outDir + __allbindingfile + string(".cpp");
    ostringstream luaAllCpp;
	if (!__includeAfx.empty())
		luaAllCpp << "#include \"" << __includeAfx <<"\"\n";
	//luaAllCpp << "#include \"LuaEx.h\"\n";
    luaAllCpp << "#include \"" << __allbindingfile << ".h\"\n";
	luaAllCpp << "#include \"luaState.h\"\n\n";
    if (bindingNS)
    {
        luaAllCpp << "namespace " << *bindingNS << "\n";
        luaAllCpp << "{\n\n";
    }

	if (_classes.size() > 0)
	{
		map<string, ClassBinding>::iterator iter = _classes.begin();
		for (; iter != _classes.end(); iter++)
		{
			if (generatingGameplay || (!generatingGameplay && _namespaces["gameplay"].find(iter->second.classname) == _namespaces["gameplay"].end()))
			{
				if (iter->second._isExport)
				{
					luaAllCpp << "extern void luaex_" << iter->second.uniquename << "_open(LuaState* L);\n";
				}
			}
		}
	}
	luaAllCpp << "extern void " << __globalfunc << "(LuaState* L);\n";

	luaAllCpp << "\nvoid " << __regfunc << "(LuaState* L)\n";
	luaAllCpp << "{\n";

	if (_classes.size() > 0)
	{
		set<string> subClass;
		map<string, ClassBinding>::iterator iter = _classes.begin();
		for (; iter != _classes.end(); iter++)
		{
			if (generatingGameplay || (!generatingGameplay && _namespaces["gameplay"].find(iter->second.classname) == _namespaces["gameplay"].end()))
			{
				if (iter->second._isExport)
				{
					luaAllCpp << "    L->moduleUserType(\"" << iter->second.uniquename << "\");\n";
				}
			}
		}
	}

	luaAllCpp << "}\n\n";

    luaAllCpp << "\nvoid " << __allbindingfunc << "(LuaState* L)\n";
    luaAllCpp << "{\n";
	luaAllCpp << "    L->moduleOpen();\n";
	luaAllCpp << "    " << __regfunc << "(L);\n";
	luaAllCpp << "    L->moduleBegin(NULL);\n";

    // Write out all the class bindings.
    if (_classes.size() > 0)
    {
        map<string, ClassBinding>::iterator iter = _classes.begin();
        for (; iter != _classes.end(); iter++)
        {
            if (generatingGameplay || (!generatingGameplay && _namespaces["gameplay"].find(iter->second.classname) == _namespaces["gameplay"].end()))
            {
				if (iter->second._isExport)
				{
					cout << "Generating bindings for '" << iter->first << "'...\n";
					iter->second.write(_outDir, _includes[iter->second.include], bindingNS);
					//luaAllCpp << "	  luaex_" << iter->second.uniquename << "_open(L);\n";
				}
            }
        }

		map<string, ClassBinding> copyClasses = _classes;
		while (copyClasses.size() > 0)
		{
			map<string, ClassBinding>::iterator iter = copyClasses.begin();
			while(iter != copyClasses.end())
			{
				if (generatingGameplay || (!generatingGameplay && _namespaces["gameplay"].find(iter->second.classname) == _namespaces["gameplay"].end()))
				{
					if (iter->second._isExport)
					{
						if ((iter->second.base.size() == 0) || copyClasses.find(getIdentifier(iter->second.base[0])) == copyClasses.end() )
						{
							luaAllCpp << "	  luaex_" << iter->second.uniquename << "_open(L);\n";
							copyClasses.erase(iter++);
						}
						else
						{
							iter++;
						}
					}
					else
					{
						copyClasses.erase(iter++);
					}
				}
				else
				{
					copyClasses.erase(iter++);
				}
			}
		}
    }
	luaAllCpp << "	  "<< __globalfunc << "(L);\n";

    // Go through all the classes and if they have any derived classes, add them to the list of base classes.
    vector<string> baseClasses;
    for (map<string, ClassBinding>::iterator iter = _classes.begin(); iter != _classes.end(); iter++)
    {
        if (iter->second.derived.size() > 0)
            baseClasses.push_back(iter->first);
    }

    // Write out the global bindings file.
    cout << "Generating global bindings...\n";
    //if (baseClasses.size() > 0 || _functions.size() > 0 || _enums.size() > 0)
    {
        // Calculate if there are global function bindings to write out.
        bool generateGlobalFunctionBindings = false;
        if (_functions.size() > 0)
        {
            for (map<string, vector<FunctionBinding> >::iterator iter = _functions.begin(); iter != _functions.end(); iter++)
            {
                if (generatingGameplay || (!generatingGameplay &&  _namespaces["gameplay"].find(iter->second[0].name) == _namespaces["gameplay"].end()))
                {
                    generateGlobalFunctionBindings = true;
                    break;
                }
            }
        }

        // Calculate if there are enum bindings to write out.
        bool generateEnumBindings = false;
        if (_enums.size() > 0)
        {
            for (map<string, EnumBinding>::iterator iter = _enums.begin(); iter != _enums.end(); iter++)
            {
                if ((generatingGameplay || !generatingGameplay &&  _namespaces["gameplay"].find(iter->first) == _namespaces["gameplay"].end()) && iter->second.isGlobal)
                {
                    generateEnumBindings = true;
                    break;
                }
            }
        }

        // Write out the header file.

        // Write out the implementation.
        {
            string path = _outDir + __globalfile + string(".cpp");
            ostringstream global;
			if (!__includeAfx.empty())
				global << "#include \"" << __includeAfx <<"\"\n";
			global << "#include \"luaState.h\"\n";
            //global << "#include \"LuaEx.h\"\n";
            map<string, set<string> >::iterator iter = _includes.find(__globalfile + string(".h"));
            if (iter != _includes.end())
            {
                set<string>::iterator includeIter = iter->second.begin();
                for (; includeIter != iter->second.end(); includeIter++)
                {
                    global << "#include \"" << *includeIter << "\"\n";
                }
            }
            global << "\n";

			if (generateGlobalFunctionBindings)
			{
				for (map<string, vector<FunctionBinding> >::iterator iter = _functions.begin(); iter != _functions.end(); iter++)
				{
					if (generatingGameplay || (!generatingGameplay &&  _namespaces["gameplay"].find(iter->second[0].name) == _namespaces["gameplay"].end()))
					{
						if (iter->second[0].ns.length() > 0 && (!bindingNS || (*bindingNS != iter->second[0].ns)))
						{
							global << "using namespace " << iter->second[0].ns << ";\n\n";
						}
					}
				}
			}

            if (bindingNS)
            {
                global << "namespace " << *bindingNS << "\n";
                global << "{\n\n";
            }
            
            // Write out the binding functions.
            if (generateGlobalFunctionBindings)
            {
                for (map<string, vector<FunctionBinding> >::iterator iter = _functions.begin(); iter != _functions.end(); iter++)
                {
                    if (generatingGameplay || (!generatingGameplay &&  _namespaces["gameplay"].find(iter->second[0].name) == _namespaces["gameplay"].end()))
                        FunctionBinding::write(global, iter->second);
                }
            }

			// Write out the function used to register all global bindings with Lua.
			global << "void " << __globalfunc << "(LuaState* L)\n";
			global << "{\n";

			if (generateGlobalFunctionBindings)
			{
				// Bind the non-member functions.
				for (map<string, vector<FunctionBinding> >::iterator iter = _functions.begin(); iter != _functions.end(); iter++)
				{
					if (generatingGameplay || (!generatingGameplay &&  _namespaces["gameplay"].find(iter->second[0].name) == _namespaces["gameplay"].end()))
					{
						UINT len = iter->second[0].name.length();
						char* name = new char[ len + 1 ];
						strncpy(name, iter->second[0].name.c_str(), len);
						name[ len ] = 0;
						*name = toupper(*name);

						global << "    L->moduleFunction(\"" << name << "\", " << iter->second[0].getFunctionName() << ");\n";
					}
				}
			}

			// Register all enums.
			if (generateEnumBindings)
			{
				for (map<string, EnumBinding>::iterator iter = _enums.begin(); iter != _enums.end(); iter++)
				{
					if (generatingGameplay || (!generatingGameplay &&  _namespaces["gameplay"].find(iter->first) == _namespaces["gameplay"].end() && iter->second.isGlobal))
					{
						writeEnum(global, iter->second, 2);
					}
				}
			}
			global << "}\n\n";

            if (bindingNS)
                global << "}\n";

            writeFile(path, global.str());
        }
    }

	luaAllCpp << "    L->moduleEnd();\n";
    luaAllCpp << "}\n\n";
    if (bindingNS)
        luaAllCpp << "}\n\n";

    writeFile(luaAllCppStr, luaAllCpp.str());

    if (bindingNS)
    {
        luaAllH << "\nnamespace " << *bindingNS << "\n";
        luaAllH << "{\n\n";
    }
    luaAllH << "void " << __allbindingfunc << "(LuaState* L);\n\n";
    if (bindingNS)
        luaAllH<< "}\n\n";
    luaAllH << "#endif\n";

    writeFile(luaAllHStr, luaAllH.str());
}

void Generator::getAllDerived(set<string>& derived, string classname)
{
    for (unsigned int i = 0, count = _classes[classname].derived.size(); i < count; i++)
    {
        // If the derived class is not in the ref ID table, then it
        // is a hidden (protected, private, etc.) class, so don't include it.
        if (_refIds.find(_classes[classname].derived[i]) != _refIds.end())
        {
            string derivedClassName = getIdentifier(_classes[classname].derived[i]);
            derived.insert(derivedClassName);
            getAllDerived(derived, derivedClassName);
        }
    }
}

void Generator::getIncludes(XMLElement* e, string filename)
{
    filename.replace(filename.find(".cpp"), 4, ".h");
    for (e = e->FirstChildElement("includes"); e; e = e->NextSiblingElement("includes"))
    {
        if (e->Attribute("refid"))
            _includes[filename].insert(e->GetText());
    }
}

bool Generator::isClassBindingFromRef( string refId )
{
	string className = getIdentifier(refId);
	map<string, ClassBinding>::iterator iter = _classes.find(className);
	if (iter != _classes.end() && (iter->second._isScript|| iter->second._isExport))
	{
		return true;
	}
	return false;
}

bool Generator::isExport( XMLElement* e)
{
	return getScriptFlag(e) == __exportTag;
}


void Generator::writeEnum( ostream& o, EnumBinding& binding, int indentLevel )
{
	indent(o, indentLevel);
	o << "L->moduleDefine(\"" << binding.name << "\");\n";
	indent(o, indentLevel);
	o << "L->moduleBegin(\"" << binding.name << "\");\n";
	for (unsigned int i = 0; i < binding.values.size(); i++)
	{
		indent(o, indentLevel+1);
		o << "L->moduleConstant(\"" << binding.values[i] << "\", ";
		for (vector<string>::iterator iter2 = binding.scopePath.begin(); iter2 != binding.scopePath.end(); iter2++)
		{
			o << *iter2 << "::";
		}
		o << binding.values[i] << ");\n";
	}
	indent(o, indentLevel);
	o << "L->moduleEnd();\n";
}

void Generator::writeEnum( ostream& o, string enumName, int indentLevel )
{
	if (_enums.find(enumName) != _enums.end())
	{
		writeEnum(o, _enums[enumName], indentLevel);
	}
}

string Generator::getIncludeAfx()
{
	return __includeAfx;
}

// ----------------------------------------------------------------------------
// Utility functions

static string trim(const string& str)
{
    string s = str;
    while (isspace(s[0]))
    {
        s.erase(s.begin());
    }
    while (isspace(s[s.size() - 1]))
    {
        s.erase(s.begin() + s.size() - 1);
    }
    return s;
}

static string stripTypeQualifiers(const string& typeStr, FunctionBinding::Param::Kind& kind, int& levelsOfIndirection, bool& outParam)
{
    levelsOfIndirection = 0;

    string type = typeStr;
    kind = FunctionBinding::Param::KIND_VALUE;

	// Check if the type is autoPtr.
	string::size_type i = type.find("ref_ptr<");
	if (i != type.npos)
	{
		kind = FunctionBinding::Param::KIND_REFPTR;

		type.erase(type.begin() + i, type.begin() + i + 8);
		type.erase(type.begin() + type.rfind(">"));
	}
	else
	{
		// Check if the type is a reference.
		i = type.find("&");
		if (i != type.npos)
		{
			kind = FunctionBinding::Param::KIND_REFERENCE;
			type.erase(type.begin() + i);
		}
	}

    // Check if the type is a pointer.
    while ((i = type.find("*")) != std::string::npos)
    {
        kind = FunctionBinding::Param::KIND_POINTER;
        type.erase(type.begin() + i);
        ++levelsOfIndirection;
    }

    // Ignore const qualifiers.
    i = type.find("const ");
    if (i != type.npos)
    {
        type.erase(type.begin() + i, type.begin() + i + 6);
    }

	i = type.find("OUT ");
	if (i != type.npos)
	{
		type.erase(type.begin() + i, type.begin() + i + 4);
		outParam = true;
	}

    return trim(type);
}

static inline bool isWantedFileNormal(const string& s)
{
    if (s.find(".xml") == s.size() - 4)
    {
        if (s.find("class") == 0 || s.find("struct") == 0 || 
            (s.find("_") == 0 && (s.find("h.xml") != s.npos || s.find("cpp.xml") != s.npos)))
            return true;
    }
    return false;
}

static inline bool isNamespaceFile(const string& s)
{
    if (s.find(".xml") == s.size() - 4)
    {
        if (s.find("namespace") == 0 || (!isWantedFileNormal(s) && s.find("h.xml") != s.npos))
            return true;
    }
    return false;
}

static inline bool isGeneratedBindingFile(const string& s)
{
    return ( (s.find(".cpp") == s.size() - 4) || (s.find(".h") == s.size() - 2) ) && (s.compare("lua_UserDefine.cpp") != 0);
}

static bool getFileList(string directory, vector<string>& files, bool (*isWantedFile)(const string& s))
{
#ifdef WIN32
    string path = directory + string("/*");

    // Convert char to wchar.
    basic_string<TCHAR> wPath;
    wPath.assign(path.begin(), path.end());

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(wPath.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        return false;
    }
    do
    {
        // Add to the list if this is not a directory and it passes the test function.
        if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            // Convert wchar to char.
            basic_string<TCHAR> wfilename(FindFileData.cFileName);
            string filename;
            filename.assign(wfilename.begin(), wfilename.end());

            if (isWantedFile(filename))
            {
                filename = string(directory) + filename;
                files.push_back(filename);
            }
        }
    } while (FindNextFile(hFind, &FindFileData) != 0);

    FindClose(hFind);
    return true;
#else
    string path = directory + string("/.");
    struct dirent* dp;
    DIR* dir = opendir(path.c_str());
    if (!dir)
    {
        return false;
    }
    while ((dp = readdir(dir)) != NULL)
    {
        string filepath(path);
        filepath.append("/");
        filepath.append(dp->d_name);

        struct stat buf;
        if (!stat(filepath.c_str(), &buf))
        {
            // Add to the list if this is not a directory and it passes the test function.
            if (!S_ISDIR(buf.st_mode))
            {
                string filename = dp->d_name;
                if (isWantedFile(filename))
                {
                    filename = string(directory) + filename;
                    files.push_back(filename);
                }
            }
        }
    }
    closedir(dir);
    return true;
#endif
}

bool isReservedKeyword(string name)
{
    static set<string> keywords;
    if (keywords.size() == 0)
    {
        keywords.insert("and");
        keywords.insert("break");
        keywords.insert("do");
        keywords.insert("else");
        keywords.insert("elseif");
        keywords.insert("end");
        keywords.insert("false");
        keywords.insert("for");
        keywords.insert("function");
        keywords.insert("if");
        keywords.insert("in");
        keywords.insert("local");
        keywords.insert("nil");
        keywords.insert("not");
        keywords.insert("or");
        keywords.insert("repeat");
        keywords.insert("return");
        keywords.insert("then");
        keywords.insert("true");
        keywords.insert("until");
        keywords.insert("while");
    };

    return keywords.find(name) != keywords.end();
}

static inline void indent(ostream& o, int indentLevel)
{
	for (int k = 0; k < indentLevel; k++)
		o << "  ";
}
// ----------------------------------------------------------------------------
