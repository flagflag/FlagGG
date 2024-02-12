#include "Container/ArrayPtr.h"
#include "Core/Context.h"
#include "Core/Profiler.h"
#include "Core/ObjectFactory.h"
#include "Log.h"
#include "IOFrame/Buffer/StringBuffer.h"
//#include "../IO/VectorBuffer.h"
#include "Resource/ResourceCache.h"
#include "Resource/XMLFile.h"

#include <PugiXml/pugixml.hpp>

namespace FlagGG
{

REGISTER_TYPE_FACTORY(XMLFile);

/// XML writer for pugixml.
class XMLWriter : public pugi::xml_writer
{
public:
    /// Construct.
    explicit XMLWriter(IOFrame::Buffer::IOBuffer* stream) :
        stream_(stream),
        success_(true)
    {
    }

    /// Write bytes to output.
    void write(const void* data, size_t size) override
    {
        if (stream_->WriteStream(data, (unsigned)size) != size)
            success_ = false;
    }

    /// Destination serializer.
    IOFrame::Buffer::IOBuffer* stream_;
    /// Success flag.
    bool success_;
};

XMLFile::XMLFile() :
    Resource(),
    document_(new pugi::xml_document())
{
}

XMLFile::~XMLFile() = default;

bool XMLFile::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
    unsigned dataSize = stream->GetSize();
    if (!dataSize)
    {
        FLAGGG_LOG_ERROR("Zero sized XML data.");
        return false;
    }

    SharedArrayPtr<char> buffer(new char[dataSize]);
    if (stream->ReadStream(buffer.Get(), dataSize) != dataSize)
        return false;

    if (!document_->load_buffer(buffer.Get(), dataSize))
    {
        FLAGGG_LOG_ERROR("Could not parse XML data.");
        document_->reset();
        return false;
    }

    // Note: this probably does not reflect internal data structure size accurately
    SetMemoryUse(dataSize);
    return true;
}

bool XMLFile::BeginSave(IOFrame::Buffer::IOBuffer* stream)
{
    return Save(stream, "\t");
}

bool XMLFile::Save(IOFrame::Buffer::IOBuffer* stream, const String& indentation) const
{
    XMLWriter writer(stream);
    document_->save(writer, indentation.CString());
    return writer.success_;
}

XMLElement XMLFile::CreateRoot(const String& name)
{
    document_->reset();
    pugi::xml_node root = document_->append_child(name.CString());
    return XMLElement(this, root.internal_object());
}

XMLElement XMLFile::GetOrCreateRoot(const String& name)
{
    XMLElement root = GetRoot(name);
    if (root.NotNull())
        return root;
    root = GetRoot();
    if (root.NotNull())
        FLAGGG_LOG_WARN("XMLFile already has root " + root.GetName() + ", deleting it and creating root " + name);
    return CreateRoot(name);
}

bool XMLFile::FromString(const String& source)
{
    if (source.Empty())
        return false;

    IOFrame::Buffer::StringBuffer buffer(source.CString(), source.Length());
    return BeginLoad(&buffer) && EndLoad();
}

XMLElement XMLFile::GetRoot(const String& name)
{
    pugi::xml_node root = document_->first_child();
    if (root.empty())
        return XMLElement();

    if (!name.Empty() && name != root.name())
        return XMLElement();
    else
        return XMLElement(this, root.internal_object());
}

String XMLFile::ToString(const String& indentation) const
{
    // TODO:
    //VectorBuffer dest;
    //XMLWriter writer(dest);
    //document_->save(writer, indentation.CString());
    //return String((const char*)dest.GetData(), dest.GetSize());
    return String::EMPTY;
}

void XMLFile::Patch(XMLFile* patchFile)
{
    Patch(patchFile->GetRoot());
}

void XMLFile::Patch(const XMLElement& patchElement)
{
    pugi::xml_node root = pugi::xml_node(patchElement.GetNode());

    for (auto& patch : root)
    {
        pugi::xml_attribute sel = patch.attribute("sel");
        if (sel.empty())
        {
            FLAGGG_LOG_ERROR("XML Patch failed due to node not having a sel attribute.");
            continue;
        }

        // Only select a single node at a time, they can use xpath to select specific ones in multiple otherwise the node set becomes invalid due to changes
        pugi::xpath_node original = document_->select_single_node(sel.value());
        if (!original)
        {
            FLAGGG_LOG_STD_ERROR("XML Patch failed with bad select: %s.", sel.value());
            continue;
        }

        if (strcmp(patch.name(), "add") == 0)
            PatchAdd(patch, original);
        else if (strcmp(patch.name(), "replace") == 0)
            PatchReplace(patch, original);
        else if (strcmp(patch.name(), "remove") == 0)
            PatchRemove(original);
        else
            FLAGGG_LOG_ERROR("XMLFiles used for patching should only use 'add', 'replace' or 'remove' elements.");
    }
}

void XMLFile::PatchAdd(const pugi::xml_node& patch, pugi::xpath_node& original) const
{
    // If not a node, log an error
    if (original.attribute())
    {
        FLAGGG_LOG_STD_ERROR("XML Patch failed calling Add due to not selecting a node, %s attribute was selected.",
            original.attribute().name());
        return;
    }

    // If no type add node, if contains '@' treat as attribute
    pugi::xml_attribute type = patch.attribute("type");
    if (!type || strlen(type.value()) <= 0)
        AddNode(patch, original);
    else if (type.value()[0] == '@')
        AddAttribute(patch, original);
}

void XMLFile::PatchReplace(const pugi::xml_node& patch, pugi::xpath_node& original) const
{
    // If no attribute but node then its a node, otherwise its an attribute or null
    if (!original.attribute() && original.node())
    {
        pugi::xml_node parent = original.node().parent();

        parent.insert_copy_before(patch.first_child(), original.node());
        parent.remove_child(original.node());
    }
    else if (original.attribute())
    {
        original.attribute().set_value(patch.child_value());
    }
}

void XMLFile::PatchRemove(const pugi::xpath_node& original) const
{
    // If no attribute but node then its a node, otherwise its an attribute or null
    if (!original.attribute() && original.node())
    {
        pugi::xml_node parent = original.parent();
        parent.remove_child(original.node());
    }
    else if (original.attribute())
    {
        pugi::xml_node parent = original.parent();
        parent.remove_attribute(original.attribute());
    }
}

void XMLFile::AddNode(const pugi::xml_node& patch, const pugi::xpath_node& original) const
{
    // If pos is null, append or prepend add as a child, otherwise add before or after, the default is to append as a child
    pugi::xml_attribute pos = patch.attribute("pos");
    if (!pos || strlen(pos.value()) <= 0 || strcmp(pos.value(), "append") == 0)
    {
        pugi::xml_node::iterator start = patch.begin();
        pugi::xml_node::iterator end = patch.end();

        // There can not be two consecutive text nodes, so check to see if they need to be combined
        // If they have been we can skip the first node of the nodes to add
        if (CombineText(patch.first_child(), original.node().last_child(), false))
            start++;

        for (; start != end; start++)
            original.node().append_copy(*start);
    }
    else if (strcmp(pos.value(), "prepend") == 0)
    {
        pugi::xml_node::iterator start = patch.begin();
        pugi::xml_node::iterator end = patch.end();

        // There can not be two consecutive text nodes, so check to see if they need to be combined
        // If they have been we can skip the last node of the nodes to add
        if (CombineText(patch.last_child(), original.node().first_child(), true))
            end--;

        pugi::xml_node pos = original.node().first_child();
        for (; start != end; start++)
            original.node().insert_copy_before(*start, pos);
    }
    else if (strcmp(pos.value(), "before") == 0)
    {
        pugi::xml_node::iterator start = patch.begin();
        pugi::xml_node::iterator end = patch.end();

        // There can not be two consecutive text nodes, so check to see if they need to be combined
        // If they have been we can skip the first node of the nodes to add
        if (CombineText(patch.first_child(), original.node().previous_sibling(), false))
            start++;

        // There can not be two consecutive text nodes, so check to see if they need to be combined
        // If they have been we can skip the last node of the nodes to add
        if (CombineText(patch.last_child(), original.node(), true))
            end--;

        for (; start != end; start++)
            original.parent().insert_copy_before(*start, original.node());
    }
    else if (strcmp(pos.value(), "after") == 0)
    {
        pugi::xml_node::iterator start = patch.begin();
        pugi::xml_node::iterator end = patch.end();

        // There can not be two consecutive text nodes, so check to see if they need to be combined
        // If they have been we can skip the first node of the nodes to add
        if (CombineText(patch.first_child(), original.node(), false))
            start++;

        // There can not be two consecutive text nodes, so check to see if they need to be combined
        // If they have been we can skip the last node of the nodes to add
        if (CombineText(patch.last_child(), original.node().next_sibling(), true))
            end--;

        pugi::xml_node pos = original.node();
        for (; start != end; start++)
            pos = original.parent().insert_copy_after(*start, pos);
    }
}

void XMLFile::AddAttribute(const pugi::xml_node& patch, const pugi::xpath_node& original) const
{
    pugi::xml_attribute attribute = patch.attribute("type");

    if (!patch.first_child() && patch.first_child().type() != pugi::node_pcdata)
    {
        FLAGGG_LOG_STD_ERROR("XML Patch failed calling Add due to attempting to add non text to an attribute for %s.", attribute.value());
        return;
    }

    String name(attribute.value());
    name = name.Substring(1);

    pugi::xml_attribute newAttribute = original.node().append_attribute(name.CString());
    newAttribute.set_value(patch.child_value());
}

bool XMLFile::CombineText(const pugi::xml_node& patch, const pugi::xml_node& original, bool prepend) const
{
    if (!patch || !original)
        return false;

    if ((patch.type() == pugi::node_pcdata && original.type() == pugi::node_pcdata) ||
        (patch.type() == pugi::node_cdata && original.type() == pugi::node_cdata))
    {
        if (prepend)
            const_cast<pugi::xml_node&>(original).set_value(FlagGG::ToString("%s%s", patch.value(), original.value()).CString());
        else
            const_cast<pugi::xml_node&>(original).set_value(FlagGG::ToString("%s%s", original.value(), patch.value()).CString());

        return true;
    }

    return false;
}

}
