#include <iostream>
#include <client/per_process.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <crates/bindings.hpp>

#include "./node_list-inl.hpp"
#include "./element.hpp"
#include "./text.hpp"
#include "./document-inl.hpp"
#include "./document_renderer.hpp"
#include "./browsing_context.hpp"
#include "../cssom/selectors/matching.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;

  shared_ptr<Document> Document::Make(string contentType, DocumentType documentType, shared_ptr<BrowsingContext> browsingContext, bool autoConnect)
  {
    return make_shared<Document>(contentType, documentType, browsingContext, autoConnect);
  }

  Document::Document(string contentType, DocumentType documentType, shared_ptr<BrowsingContext> browsingContext, bool autoConnect)
      : Node(NodeType::DOCUMENT_NODE, "#document", nullopt)
      , contentType(contentType)
      , documentType(documentType)
      , scene(TrClientContextPerProcess::Get()->builtinScene)
      , browsingContext(browsingContext)
      , auto_connect_(autoConnect)
      , default_view_(TrClientContextPerProcess::Get()->window)
      , timeline_(make_shared<DocumentTimeline>())
  {
    assert(browsingContext != nullptr);
    assert(default_view_.lock() != nullptr);
    doc_internal_ = make_shared<pugi::xml_document>();
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

  void Document::setSource(const string &source, bool isFragment)
  {
    string inputText(source);
    if (documentType == DocumentType::kHTML)
      fixSource(inputText); // Fix the source string if it's an HTML document.

    // Parse the XML document.
    auto flag = pugi::parse_default | pugi::parse_ws_pcdata | pugi::parse_comments;
    if (isFragment)
      flag |= pugi::parse_fragment;

    auto r = doc_internal_->load_string(inputText.c_str(), flag);
    if (r.status != pugi::xml_parse_status::status_ok)
      throw runtime_error("Failed to parse XML document: " + std::string(r.description()));

    resetFrom(doc_internal_, getPtr<Document>());
    is_source_loaded_ = true;

    //
    // Update fields after the document are parsed.
    //

    // Find the head and body elements.
    for (auto childNode : documentElement()->childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
      {
        if (childNode->nodeName == "head")
          head_element_ = dynamic_pointer_cast<HTMLHeadElement>(childNode);
        else if (childNode->nodeName == "body")
          body_element_ = dynamic_pointer_cast<HTMLBodyElement>(childNode);
      }
    }

    // Clear list and maps.
    all_elements_list_.clear();
    element_map_by_id_.clear();

    // Update the element list and maps.
    auto initElementsCache = [this](shared_ptr<Node> childNode)
    {
      onNodeAdded(childNode, true, false);
      return true;
    };
    iterateChildNodes(initElementsCache);

    if (should_open_)
      openInternal();
  }

  void Document::open()
  {
    should_open_ = true;
    if (is_source_loaded_)
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

  std::shared_ptr<Node> Document::importNode(const std::shared_ptr<Node> node, bool deep)
  {
    if (Node::Is<Document>(node))
      throw runtime_error("The node is a document node, which cannot be imported.");

    auto importedNode = node->cloneNode(deep);
    importedNode->updateFieldsFromDocument(getPtr<Document>());
    return importedNode;
  }

  shared_ptr<Element> Document::getElementById(const string &id)
  {
    auto it = element_map_by_id_.find(id);
    if (it != element_map_by_id_.end())
      return it->second;
    else
      return nullptr;
  }

  std::vector<shared_ptr<Element>> Document::getElementsByClassName(const string &className)
  {
    std::vector<shared_ptr<Element>> elements;
    for (auto element : all_elements_list_)
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
    for (auto element : all_elements_list_)
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
    for (auto element : all_elements_list_)
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
    for (const auto &element : all_elements_list_)
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
    for (auto element : all_elements_list_)
    {
      if (!Node::Is<HTMLElement>(element))
        continue;
      if (client_cssom::selectors::matchesSelectorList(selectorList, Node::As<HTMLElement>(element)))
        elements.push_back(element);
    }
    return elements;
  }

  void Document::appendStyleSheet(shared_ptr<client_cssom::CSSStyleSheet> sheet)
  {
    stylesheets_.push_back(sheet);
    style_cache_.invalidateCache();
    onStyleSheetsDidChange();
  }

  void Document::onNodeAdded(const std::shared_ptr<Node> node, bool fast_insert, bool recursive)
  {
    if (TR_UNLIKELY(node == nullptr))
      return;

    if (node->isElement())
    {
      shared_ptr<Element> element = Node::As<Element>(node);

      // Fast inserting elements into the list without checking for duplicates.
      // FIXME(yorkie): At initialization, we can ensure that the elements are unique.
      if (fast_insert)
      {
        all_elements_list_.push_back(element);
      }
      else
      {
        // Check if element is already in the list to avoid duplicates
        auto it = std::find(all_elements_list_.begin(), all_elements_list_.end(), element);
        if (it == all_elements_list_.end())
        {
          // If not, add it to the all elements list
          all_elements_list_.push_back(element);
        }
      }

      // Add the element to the element map by id
      if (!element->id.empty())
        element_map_by_id_[element->id] = element;
    }

    if (recursive)
    {
      // Recursively add child nodes
      for (auto childNode : node->childNodes)
        onNodeAdded(childNode, fast_insert, true);
    }
  }

  void Document::onNodeRemoved(const shared_ptr<Node> node, bool recursive)
  {
    if (TR_UNLIKELY(node == nullptr))
      return;

    if (node->isElement())
    {
      shared_ptr<Element> element = Node::As<Element>(node);

      // Remove the element from the all elements list
      auto it = std::remove(all_elements_list_.begin(), all_elements_list_.end(), element);
      if (it != all_elements_list_.end())
        all_elements_list_.erase(it, all_elements_list_.end());

      // Remove the element from the element map by id
      if (!element->id.empty())
        element_map_by_id_.erase(element->id);
    }

    if (recursive)
    {
      // Recursively remove child nodes
      for (auto childNode : node->childNodes)
        onNodeRemoved(childNode, true);
    }
  }

  void Document::openInternal()
  {
    // Connect the window and document before opening this document.
    {
      auto window = TrClientContextPerProcess::Get()->window;
      assert(window != nullptr);
      window->configureDocument(getPtr<Document>());
      default_view_ = window;
      onDocumentOpened();
    }

    // Start connecting the document's children automatically if the flag is set.
    if (auto_connect_)
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
    while ((pos = source.find(invalidComment, pos)) != string::npos)
    {
      source.replace(pos, invalidComment.size(), defaultComment);
      pos += defaultComment.length();
    }
    return source;
  }

  std::string Document::cookie() const
  {
    std::string cookies_str;
    for (const auto &cookie : cookies_)
    {
      if (!cookies_str.empty())
        cookies_str += "; ";
      cookies_str += cookie.first + "=" + cookie.second;
    }
    return cookies_str;
  }

  void Document::setCookie(const std::string &new_cookies)
  {
    // TODO(yorkie): implement the cookie parsing and setting logic.
    // See https://developer.mozilla.org/en-US/docs/Web/API/Document/cookie
  }

  shared_ptr<Element> Document::firstElementChild() const
  {
    for (auto childNode : childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        return Node::As<Element>(childNode);
    }
    return nullptr;
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
    document->setSource("<html>" + input + "</html>", true);

    auto htmlElement = document->documentElement();
    assert(htmlElement != nullptr && "The `documentElement` is not found.");
    return htmlElement->childNodes;
  }

  HTMLDocument::HTMLDocument(shared_ptr<BrowsingContext> browsingContext, bool autoConnect)
      : Document("text/html", DocumentType::kHTML, browsingContext, autoConnect)
      , layout_view_(nullptr)
  {
  }

  std::optional<builtin_scene::BoundingBox> HTMLDocument::visualBoundingBox() const
  {
    auto layoutBox = layoutView();
    if (layoutBox == nullptr)
      return nullopt;

    auto &viewport = layoutBox->viewport;
    builtin_scene::BoundingBox boundingBox(viewport.width(),
                                           viewport.height(),
                                           viewport.depth());
    return boundingBox;
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
    auto selfDocument = getPtr<HTMLDocument>();
    auto window = default_view_.lock();

    // Set the document cache to be invalid once the document is opened.
    invalidateDocumentCache();

    auto scene = TrClientContextPerProcess::Get()->builtinScene;
    if (scene != nullptr && window != nullptr)
    {
      // Create the layout view before starting the scene.
      layout_view_ = client_layout::LayoutView::Make(selfDocument, *window);

      // TODO: support resize the document scene.
      using namespace builtin_scene::ecs;
      // Configure the built-in scene for the HTML rendering before starting the scene.
      scene->addSystem(SchedulerLabel::kPreUpdate, System::Make<RenderHTMLDocument>(this));
      scene->start();
    }
  }

  void HTMLDocument::onStyleSheetsDidChange()
  {
    invalidateDocumentCache();
  }

  void HTMLDocument::simulateScrollWithOffset(float offsetX, float offsetY)
  {
    auto layoutBox = layoutView();
    assert(layoutBox != nullptr && "The layout box is not set.");
    if (offsetX == 0 && offsetY == 0)
      return;

    layoutBox->scrollBy(glm::vec3(offsetX, offsetY, 0));
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));
  }
}
