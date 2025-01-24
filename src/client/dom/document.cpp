#include <iostream>
#include <client/per_process.hpp>
#include <client/builtin_scene/ecs-inl.hpp>

#include "./element.hpp"
#include "./text.hpp"
#include "./document-inl.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;

  enum class TreverseOrder
  {
    PreOrder, // Pre-order traversal: root -> left -> right
    PostOrder // Post-order traversal: left -> right -> root
  };

  Document::Document(string contentType, DocumentType documentType,
                     shared_ptr<BrowsingContext> browsingContext,
                     bool autoConnect)
      : Node(NodeType::DOCUMENT_NODE, "#document", nullopt),
        contentType(contentType),
        documentType(documentType),
        scene(TrClientContextPerProcess::Get()->builtinScene),
        browsingContext(browsingContext),
        autoConnect(autoConnect),
        defaultView_(TrClientContextPerProcess::Get()->window)
  {
    assert(scene != nullptr);
    assert(browsingContext != nullptr);
    assert(defaultView_.lock() != nullptr);
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

  std::shared_ptr<browser::Window> Document::defaultView()
  {
    auto ref = defaultView_.lock();
    assert(ref != nullptr);
    return ref;
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
    // Connect the window and document before opening this document.
    {
      auto window = TrClientContextPerProcess::Get()->window;
      assert(window != nullptr);
      window->configureDocument(getPtr<Document>());
      defaultView_ = window;
    }

    // Start connecting the document's children automatically if the flag is set.
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
    const std::string name() const override { return "dom.RenderHTMLDocument"; }
    void onExecute() override
    {
      assert(document_ != nullptr);
      auto body = document_->body();
      auto scene = document_->scene;
      if (scene == nullptr || body == nullptr)
        return;

      // Step 1: Compute the elements' styles.
      // TODO: use `computeStyle` for stylesheets.
      {
        auto adoptStyleForElement = [this](shared_ptr<HTMLElement> element)
        {
          const auto computedStyle = document_->defaultView()->getComputedStyle(element);
          element->adoptStyle(computedStyle);
          return true;
        };
        auto adoptStyleForText = [](shared_ptr<Text> textNode)
        {
          textNode->adoptStyle(*textNode->style_);
        };
        traverseElementOrTextNode(body, adoptStyleForElement, adoptStyleForText, TreverseOrder::PreOrder);
      }

      // Step 2: Compute the layout of all the elements only if the layout is dirty.
      auto rootLayoutNode = body->layoutNode();
      if (rootLayoutNode != nullptr && rootLayoutNode->isDirty())
        rootLayoutNode->computeLayout(targetWidth(), targetHeight());

      // Step 3: Call the renderElement method of each element to draw the element.
      {
        auto renderElement = [scene](shared_ptr<HTMLElement> element)
        { return element->renderElement(*scene); };
        auto renderText = [scene](shared_ptr<Text> textNode)
        { textNode->renderText(*scene); };
        traverseElementOrTextNode(body, renderElement, renderText, TreverseOrder::PreOrder);
      }
    }

  private:
    /**
     * Traverse `HTMLElement` or `Text` children from a root node.
     *
     * @param elementOrTextNode The root element or text node.
     * @param elementCallback The callback function for the element node.
     * @param textNodeCallback The callback function for the text node.
     * @param order The traverse order.
     */
    void traverseElementOrTextNode(shared_ptr<Node> elementOrTextNode,
                                   function<bool(shared_ptr<HTMLElement>)> elementCallback,
                                   function<void(shared_ptr<Text>)> textNodeCallback,
                                   TreverseOrder order)
    {
      if (TR_UNLIKELY(elementOrTextNode == nullptr) || !elementOrTextNode->connected)
        return;

      if (elementOrTextNode->nodeType == NodeType::TEXT_NODE)
      {
        auto textNode = dynamic_pointer_cast<Text>(elementOrTextNode);
        if (textNode != nullptr)
          textNodeCallback(textNode);
        return;
      }

      if (elementOrTextNode->nodeType == NodeType::ELEMENT_NODE)
      {
        auto element = dynamic_pointer_cast<HTMLElement>(elementOrTextNode);
        if (element == nullptr)
          return;

        bool shouldContinue = true;
        if (order == TreverseOrder::PreOrder)
        {
          if (!elementCallback(element)) // If the element callback returns false, stop traversing in pre-order.
            shouldContinue = false;
        }

        if (shouldContinue)
        {
          for (auto childNode : element->childNodes)
            traverseElementOrTextNode(childNode, elementCallback, textNodeCallback, order);
          if (order == TreverseOrder::PostOrder)
            elementCallback(element);
        }
      }
    }
    // The target width to render the document.
    inline float targetWidth() const
    {
      std::shared_ptr<browser::Window> window = document_->defaultView();
      assert(window != nullptr);
      return window->innerWidth();
    }
    // The target height to render the document.
    inline float targetHeight() const
    {
      std::shared_ptr<browser::Window> window = document_->defaultView();
      assert(window != nullptr);
      return window->innerHeight();
    }

  private:
    HTMLDocument *document_ = nullptr;
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
