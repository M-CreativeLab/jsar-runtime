#include <iostream>
#include <client/per_process.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <crates/bindings.hpp>

#include "./element.hpp"
#include "./text.hpp"
#include "./document-inl.hpp"
#include "./browsing_context.hpp"
#include "../cssom/selectors/matching.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;

  enum class TreverseOrder
  {
    PreOrder, // Pre-order traversal: root -> left -> right
    PostOrder // Post-order traversal: left -> right -> root
  };

  shared_ptr<Document> Document::Make(string contentType, DocumentType documentType,
                                      shared_ptr<BrowsingContext> browsingContext,
                                      bool autoConnect)
  {
    return make_shared<Document>(contentType, documentType, browsingContext, autoConnect);
  }

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
    string inputText(source);
    if (documentType == DocumentType::kHTML)
      fixSource(inputText); // Fix the source string if it's an HTML document.

    auto r = docInternal->load_string(inputText.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw runtime_error("Failed to parse XML document: " + std::string(r.description()));

    resetFrom(docInternal, getPtr<Document>());
    isSourceLoaded = true;

    //
    // Update fields after the document are parsed.
    //

    // Find the head and body elements.
    for (auto childNode : documentElement()->childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
      {
        if (childNode->nodeName == "head")
          headElement = dynamic_pointer_cast<HTMLHeadElement>(childNode);
        else if (childNode->nodeName == "body")
          bodyElement = dynamic_pointer_cast<HTMLBodyElement>(childNode);
      }
    }

    // Update the element list and maps.
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

    if (shouldOpen)
      openInternal();
  }

  void Document::open()
  {
    shouldOpen = true;
    if (isSourceLoaded)
      openInternal();
  }

  std::shared_ptr<DocumentFragment> Document::createDocumentFragment()
  {
    return make_shared<DocumentFragment>(getPtr<Document>());
  }

  std::shared_ptr<Text> Document::createTextNode(const string &data)
  {
    return make_shared<Text>(data, getPtr<Document>());
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

  vector<shared_ptr<Element>> Document::getElementsByName(const string &name)
  {
    vector<shared_ptr<Element>> elements;
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

  vector<shared_ptr<Element>> Document::getElementsByTagName(const string &tagName)
  {
    vector<shared_ptr<Element>> elements;
    for (auto element : allElementsList)
    {
      if (element->is(tagName))
        elements.push_back(element);
    }
    return elements;
  }

  shared_ptr<Element> Document::querySelector(const string &selectors)
  {
    auto s = crates::css2::parsing::parseSelectors(selectors);
    if (s == nullopt)
      throw runtime_error("Failed to parse the CSS selectors: " + selectors);

    auto selectorList = s.value();
    for (auto element : allElementsList)
    {
      if (!Node::Is<HTMLElement>(element))
        continue;
      if (client_cssom::selectors::matchesSelectorList(selectorList, Node::As<HTMLElement>(element)))
        return element;
    }
    return nullptr;
  }

  NodeList<Element> Document::querySelectorAll(const string &selectors)
  {
    auto s = crates::css2::parsing::parseSelectors(selectors);
    if (s == nullopt)
      throw runtime_error("Failed to parse the CSS selectors: " + selectors);

    NodeList<Element> elements(false);
    auto selectorList = s.value();
    for (auto element : allElementsList)
    {
      if (!Node::Is<HTMLElement>(element))
        continue;
      if (client_cssom::selectors::matchesSelectorList(selectorList, Node::As<HTMLElement>(element)))
        elements.push_back(element);
    }
    return elements;
  }

  void Document::openInternal()
  {
    // Connect the window and document before opening this document.
    {
      auto window = TrClientContextPerProcess::Get()->window;
      assert(window != nullptr);
      window->configureDocument(getPtr<Document>());
      defaultView_ = window;
      onDocumentOpened();
    }

    // Start connecting the document's children automatically if the flag is set.
    if (autoConnect)
    {
      connect();
      load();
    }
  }

  string &Document::fixSource(string &source)
  {
    string invalidComment = "<!>";
    string defaultComment = "<!---->"; // replace the invalid comment with the default comment.

    size_t pos = 0;
    while ((pos = source.find(invalidComment, pos)) != std::string::npos)
    {
      source.replace(pos, invalidComment.size(), defaultComment);
      pos += defaultComment.length();
    }
    return source;
  }

  string XMLDocument::SerializeFragment(const shared_ptr<Node> node, bool wellFormed)
  {
    // TODO
    return "";
  }

  vector<shared_ptr<Node>> XMLDocument::ParseFragment(const shared_ptr<Element> contextElement,
                                                      const string &input)
  {
    throw runtime_error("The XMLDocument::ParseFragment method is not implemented yet.");
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

      // Step 1: Compute each element's styles.
      {
        auto adoptStyleForElement = [this](shared_ptr<HTMLElement> element)
        {
          const auto &computedStyle = document_->defaultView()->getComputedStyle(element);
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

  // This follows the fragment serializing algorithm steps.
  //
  // See https://html.spec.whatwg.org/multipage/dynamic-markup-insertion.html#fragment-serializing-algorithm-steps
  string HTMLDocument::SerializeFragment(const shared_ptr<Node> node, bool serializableShadowRoots)
  {
    // 1. If the node serializes as void, then return the empty string.
    if (node == nullptr)
      return "";

    // 2. Let `s` be a string, and initialize it to the empty string.
    string s("");

    // 3. If the node is a template element, then let the node instead be the template element's template contents (a DocumentFragment node).
    // TODO: <template> not supported yet.

    // 4. If current node is a shadow host?
    // TODO: Shadow DOM not supported yet.

    // 5. For each child node of the node, in tree order, run the following steps:
    {
      // Convert the node's depth to spaces.
      auto depthToSpaces = [](int depth) -> string
      {
        // Fast path for the common cases.
        if (depth <= 0)
          return "";
        if (depth == 1)
          return " ";
        if (depth == 2)
          return "  ";

        string spaces;
        for (int i = 0; i < depth; i++)
          spaces.append("  ");
        return spaces;
      };

      // Serialize the node(Element, Text, Comment, ProcessingInstruction, DocumentType).
      auto serializeNode = [&depthToSpaces](string &s, const shared_ptr<Node> node, bool &isElementNode)
      {
        if (TR_UNLIKELY(node == nullptr))
          return;

        // Append the prefix spaces via the depth of the node.
        s.append("\n");
        s.append(depthToSpaces(node->depth()));

        if (Node::Is<Element>(node))
        {
          auto &element = Node::AsChecked<Element>(node);
          s.append("<");
          s.append(element.localName);

          for (auto &attr : element.getAttributeNames())
          {
            string attrValue = element.getAttribute(attr);
            s.append(" ");
            s.append(attr);
            s.append("=\"");
            s.append(attrValue);
            s.append("\"");
          }
          isElementNode = true;
          return;
        }

        if (Node::Is<Text>(node))
        {
          auto &text = Node::AsChecked<Text>(node);
          s.append(text.data());
          isElementNode = false;
          return;
        }

        // TODO: Support Comment, ProcessingInstruction and DocumentType.
        isElementNode = false;
      };

      // Serialize the children of the node.
      function<void(string &, const shared_ptr<Node>)> serializeChildren =
          [&depthToSpaces, &serializeChildren, &serializeNode](string &s, const shared_ptr<Node> node)
      {
        for (auto childNode : node->childNodes)
        {
          bool isElementNode = false;
          serializeNode(s, childNode, isElementNode);

          if (childNode->hasChildNodes())
          {
            if (isElementNode)
              s.append(">");

            // Serialize the children of the child node.
            serializeChildren(s, childNode);

            // Close the element tag if it's an element node.
            if (isElementNode)
            {
              auto &element = Node::AsChecked<Element>(childNode);
              s.append("\n");
              s.append(depthToSpaces(element.depth()));
              s.append("</");
              s.append(element.localName);
              s.append(">");
            }
          }
          else
          {
            if (isElementNode)
              s.append(" />");
          }
        }
      };

      if (node->hasChildNodes())
        serializeChildren(s, node);
    }

    // 6. Return `s`.
    return s;
  }

  vector<shared_ptr<Node>> HTMLDocument::ParseFragment(const shared_ptr<Element> contextElement,
                                                       const string &input,
                                                       bool _allowDeclarativeShadowRoots)
  {
    auto contextDocument = contextElement->getOwnerDocumentReference();
    assert(contextDocument != nullptr);

    shared_ptr<Document> document = Document::Make(
        "text/html", DocumentType::kHTML, contextDocument->browsingContext, false);
    document->setSource("<html>" + input + "</html>");

    auto htmlElement = document->documentElement();
    assert(htmlElement != nullptr && "The `documentElement` is not found.");
    return htmlElement->childNodes;
  }

  HTMLDocument::HTMLDocument(shared_ptr<BrowsingContext> browsingContext, bool autoConnect)
      : Document("text/html", DocumentType::kHTML, browsingContext, autoConnect),
        layoutAllocator_(make_shared<crates::layout2::Allocator>())
  {
    if (scene != nullptr)
    {
      // Configure the built-in scene for the HTML rendering.
      using namespace builtin_scene::ecs;
      scene->addSystem(SchedulerLabel::kPreUpdate, System::Make<RenderHTMLDocument>(this));
    }
  }

  void HTMLDocument::afterLoadedCallback()
  {
    Document::afterLoadedCallback();

    // Dispatch the load event.
    dispatchEvent(DOMEventType::DOMContentLoaded);
    // TODO(Yorkie): wait for the pending resources to be loaded.
    dispatchEvent(DOMEventType::Load);
  }

  void HTMLDocument::onDocumentOpened()
  {
    auto scene = TrClientContextPerProcess::Get()->builtinScene;
    if (scene != nullptr)
      scene->start();
  }
}
