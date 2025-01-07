#include <iostream>
#include <client/per_process.hpp>
#include <client/builtin_scene/ecs-inl.hpp>

#include "./element.hpp"
#include "./document-inl.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;

  Document::Document(string contentType, DocumentType documentType,
                     shared_ptr<BrowsingContext> browsingContext,
                     bool autoConnect)
      : Node(NodeType::DOCUMENT_NODE, "#document", nullopt),
        contentType(contentType),
        documentType(documentType),
        scene(TrClientContextPerProcess::Get()->builtinScene),
        browsingContext(browsingContext),
        autoConnect(autoConnect)
  {
    assert(scene != nullptr);
    assert(browsingContext != nullptr);
    docInternal = make_shared<pugi::xml_document>();
  }

  Document::Document(Document &other)
      : Node(other),
        compatMode(other.compatMode),
        contentType(other.contentType),
        documentType(other.documentType),
        scene(other.scene),
        browsingContext(other.browsingContext),
        documentElement(other.documentElement),
        autoConnect(other.autoConnect),
        docInternal(other.docInternal)
  {
  }

  void Document::setUrl(const string &url)
  {
    if (browsingContext == nullptr)
      throw std::runtime_error("BrowsingContext is not set");

    bool loadSource = false;
    if (url.find_first_of("http:") == 0 ||
        url.find_first_of("https:") == 0 ||
        url.find_first_of("file:") == 0)
    {
      baseURI = url;
      loadSource = true;
    }
    else
    {
      baseURI = "about:blank";
      loadSource = false;
    }

    if (loadSource)
      browsingContext->fetchTextSourceResource(url, [this](const string &source)
                                               { setSource(source); });
    else
      setSource("<html><head></head><body></body></html>");
  }

  void Document::setSource(const string &source)
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse XML document: " + std::string(r.description()));

    resetFrom(docInternal, getPtr<Document>());
    isSourceLoaded = true;

    if (shouldOpen)
      openInternal();
  }

  void Document::open()
  {
    shouldOpen = true;
    if (isSourceLoaded)
      openInternal();
  }

  shared_ptr<Element> Document::getElementById(const string &id)
  {
    auto it = elementMapById.find(id);
    if (it != elementMapById.end())
      return it->second;
    else
      return nullptr;
  }

  std::vector<shared_ptr<Element>> Document::getElementsByClassName(const string &className)
  {
    std::vector<shared_ptr<Element>> elements;
    for (auto element : allElementsList)
    {
      if (element->hasAttribute("class"))
      {
        auto classes = element->getAttribute("class");
        if (classes.find(className) != std::string::npos)
          elements.push_back(element);
      }
    }
    return elements;
  }

  std::vector<shared_ptr<Element>> Document::getElementsByName(const string &name)
  {
    std::vector<shared_ptr<Element>> elements;
    for (auto element : allElementsList)
    {
      if (element->hasAttribute("name"))
      {
        auto elementName = element->getAttribute("name");
        if (elementName == name)
          elements.push_back(element);
      }
    }
    return elements;
  }

  std::vector<shared_ptr<Element>> Document::getElementsByTagName(const string &tagName)
  {
    std::vector<shared_ptr<Element>> elements;
    for (auto element : allElementsList)
    {
      if (element->is(tagName))
        elements.push_back(element);
    }
    return elements;
  }

  shared_ptr<HTMLHeadElement> Document::head()
  {
    return headElement;
  }

  shared_ptr<HTMLBodyElement> Document::body()
  {
    return bodyElement;
  }

  void Document::connect()
  {
    Node::connect();

    // When the document is connected, we need:
    // 1. Set the document element, head element, and body element.
    // 2. Update the element list and maps.

    for (auto childNode : childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
      {
        documentElement = std::dynamic_pointer_cast<Element>(childNode);
        break;
      }
    }

    if (documentElement != nullptr)
    {
      for (auto childNode : documentElement->childNodes)
      {
        if (childNode->nodeType == NodeType::ELEMENT_NODE)
        {
          if (childNode->nodeName == "head")
            headElement = std::dynamic_pointer_cast<HTMLHeadElement>(childNode);
          else if (childNode->nodeName == "body")
            bodyElement = std::dynamic_pointer_cast<HTMLBodyElement>(childNode);
        }
      }

      /**
       * Iterate all the child nodes of the document and update the element maps.
       */
      auto updateElementListAndMaps = [this](shared_ptr<Node> childNode)
      {
        if (childNode->nodeType == NodeType::ELEMENT_NODE)
        {
          auto element = std::dynamic_pointer_cast<Element>(childNode);
          allElementsList.push_back(element);
          if (!element->id.empty())
            elementMapById[element->id] = element;
        }
        return true;
      };

      allElementsList.clear();
      elementMapById.clear();
      iterateChildNodes(updateElementListAndMaps);
    }
  }

  void Document::openInternal()
  {
    if (autoConnect)
    {
      connect();
      load();
    }
  }

  XMLDocument::XMLDocument(shared_ptr<BrowsingContext> browsingContext, bool autoConnect)
      : Document("text/xml", DocumentType::kXML, browsingContext, autoConnect)
  {
  }

  // The HTML rendering ECS system, which is used to render the HTML document.
  class RenderHTMLDocument : public builtin_scene::ecs::System
  {
  public:
    RenderHTMLDocument(HTMLDocument *document)
        : builtin_scene::ecs::System(),
          document_(document)
    {
      assert(document_ != nullptr);
    }

  public:
    void onExecute()
    {
      assert(document_ != nullptr);
      auto body = document_->body();
      auto scene = document_->scene;
      if (scene == nullptr)
        return;

      // Step 1: Compute the elements' styles.
      // TODO: use `computeStyle` for stylesheets.
      iterateElementWithChildren(body, [](shared_ptr<HTMLElement> element)
                                 { element->adoptStyle(element->style); });

      // Step 2: Compute the layout of all the elements only if the layout is dirty.
      if (body->layoutNode_ != nullptr && body->layoutNode_->isDirty())
        body->layoutNode_->computeLayout(targetWidth_, targetHeight_);

      // Step 3: Call the renderElement method of each element to draw the element.
      iterateElementWithChildren(body, [scene](shared_ptr<HTMLElement> element)
                                 { element->renderElement(*scene); });
    }

  private:
    void iterateElementWithChildren(shared_ptr<HTMLElement> element, std::function<void(shared_ptr<HTMLElement>)> callback)
    {
      if (TR_UNLIKELY(element == nullptr))
        return;

      for (auto childNode : element->childNodes)
      {
        if (childNode->nodeType == NodeType::ELEMENT_NODE)
        {
          auto childElement = std::dynamic_pointer_cast<HTMLElement>(childNode);
          if (childElement != nullptr)
            iterateElementWithChildren(childElement, callback);
        }
      }
      callback(element);
    }

  private:
    HTMLDocument *document_ = nullptr;
    // TODO: support configurable target width and height.
    float targetWidth_ = 1440.0f;
    float targetHeight_ = 900.0f;
  };

  HTMLDocument::HTMLDocument(shared_ptr<BrowsingContext> browsingContext, bool autoConnect)
      : Document("text/html", DocumentType::kHTML, browsingContext, autoConnect),
        layoutAllocator_(make_shared<crates::layout::Allocator>())
  {
    {
      // Configure the built-in scene for the HTML rendering.
      using namespace builtin_scene::ecs;
      scene->addSystem(SchedulerLabel::kPreUpdate, System::Make<RenderHTMLDocument>(this));
    }
  }
}
