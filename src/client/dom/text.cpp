#include <memory>
#include "./text.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;
  using namespace builtin_scene;

  // Create a text node from a document and a value.
  xml_node createTextNode(shared_ptr<xml_document> doc, const string &value = "")
  {
    xml_node textNode(node_pcdata, doc.get());
    if (value != "" && !textNode.set_value(value.c_str()))
      return xml_node();
    return textNode;
  }

  Text::Text(xml_node node, shared_ptr<Document> ownerDocument)
      : CharacterData(node, ownerDocument),
        SceneObject(getOwnerDocumentReferenceAs<HTMLDocument>(), nodeName),
        content2d_(nullptr)
  {
    client_cssom::CSSStyleDeclaration defaultStyle;
    defaultStyle.setProperty("width", "100%");
    defaultStyle.setProperty("height", "100%");
    style_ = make_shared<client_cssom::CSSStyleDeclaration>(defaultStyle.cssText());
  }

  Text::Text(shared_ptr<Document> ownerDocument)
      : Text(createTextNode(ownerDocument->docInternal), ownerDocument)
  {
  }

  Text::Text(const string value, shared_ptr<Document> ownerDocument)
      : Text(createTextNode(ownerDocument->docInternal, value), ownerDocument)
  {
  }

  Text::Text(Text &other)
      : CharacterData(other),
        SceneObject(other),
        content2d_(std::move(other.content2d_)),
        style_(other.style_)
  {
  }

  const string &Text::wholeText() const
  {
    return data_;
  }

  unique_ptr<Text> Text::splitText(size_t offset)
  {
    if (offset > data_.size())
      throw out_of_range("Offset is out of range");

    string first = data_.substr(0, offset);
    string second = data_.substr(offset);

    data_ = first; // Update the current text node's data
    return make_unique<Text>(second, getOwnerDocumentReference());
  }

  void Text::connect()
  {
    CharacterData::connect();
    SceneObject::connectedCallback(*this); // Create the entity

    // Initialize the mesh
    auto initTextMesh = [this](Scene &scene)
    {
      auto meshes = scene.getResource<Meshes>();
      scene.addComponent(entity_.value(),
                         Mesh3d(meshes->add(MeshBuilder::CreateBox(1.0f, 1.0f, 0.001f))));
    };
    useScene(initTextMesh);

    // Initialize the Content2d and connect it.
    content2d_ = make_unique<Content2d>(shared_from_this());
    content2d_->onNodeConnected();

    // Append the text
    auto appendText = [this](Scene &scene)
    {
      assert(entity_.has_value());
      scene.addComponent(entity_.value(), Text2d(data()));
    };
    useScene(appendText);
  }
}
