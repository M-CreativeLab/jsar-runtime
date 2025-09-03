#include <iostream>
#include <crates/bindings.hpp>
#include <client/per_process.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/html/html_meta_element.hpp>
#include <client/cssom/selectors/css_selector_parser.hpp>
#include <client/cssom/selectors/matching.hpp>

#include "./node_list-inl.hpp"
#include "./element.hpp"
#include "./text.hpp"
#include "./comment.hpp"
#include "./document-inl.hpp"
#include "./document_renderer.hpp"
#include "./browsing_context.hpp"

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
      cerr << "Warning: The URL is not a valid HTTP or file URL, using about:blank instead." << endl;
      cerr << "The URL: " << url << endl;
    }

    if (loadSource)
      browsingContext->fetchTextSourceResource(url, [this](const string &source)
                                               { setSource(source); });
    else
      setSource("<html><head></head><body></body></html>");
  }

  // This class is used to trace the source code of the document and print the line containing the offset with a
  // caret (^) indicating the position.
  class SourceTrace
  {
  public:
    SourceTrace(const string &source, size_t offset)
        : source(source)
        , offset(offset)
    {
    }

    friend ostream &operator<<(ostream &os, const SourceTrace &trace)
    {
      auto &source = trace.source;
      auto &offset = trace.offset;

      // Print the first line
      // TODO(yorkie): support filename?
      os << "(source):";

      if (source.empty())
      {
        // Empty source string, nothing to print.
        os << offset << endl
           << "^" << endl;
        return os;
      }

      // 1. Search for the line containing the offset.
      size_t line_num = 0;
      size_t line_start = 0;
      size_t current = 0;
      bool found_line = false;
      size_t line_length = 0;

      while (current <= source.length())
      {
        size_t line_end = source.find('\n', current);
        if (line_end == string::npos)
          line_end = source.length();
        else
          line_num++;

        line_length = line_end - current;
        if (offset >= current && offset <= line_end)
        {
          line_start = current;
          found_line = true;
          break;
        }

        if (line_end == source.length())
          break; // String ends, no more lines to check.

        current = line_end + 1;
      }

      // 2. Extract the line containing the offset.
      string line;
      if (found_line)
      {
        line = source.substr(line_start, line_length);
      }
      else
      {
        // If the offset is not found, we take the last line of the source.
        size_t last_line_start = source.find_last_of('\n');
        if (last_line_start == string::npos)
        {
          line = source;
        }
        else
        {
          line = source.substr(last_line_start + 1);
        }
      }

      // 3. Calculate the display column position.
      size_t line_offset = (found_line) ? (offset - line_start) : line.length();
      size_t display_col = 0;

      // Calculate the display column position based on tabs and spaces.
      for (size_t i = 0; i < line_offset && i < line.length(); ++i)
      {
        if (line[i] == '\t')
          display_col = display_col + (8 - (display_col % 8));
        else
          ++display_col;
      }

      os << line_num << ":" << (display_col + 1) << endl
         << line << endl
         << string(display_col, ' ') << '^';
      return os;
    }

  public:
    string_view source;
    size_t offset;
  };

  void Document::setSource(const string &source, bool isFragment)
  {
    string inputText(source);
    if (documentType == DocumentType::kHTML)
      fixSource(inputText); // Fix the source string if it's an HTML document.

    // Parse the XML document.
    auto flag = pugi::parse_default | pugi::parse_ws_pcdata | pugi::parse_comments;
    if (isFragment)
      flag |= pugi::parse_fragment;
    if (documentType == DocumentType::kHTML)
      flag |= pugi::parse_unquoted_attributes;

    auto r = doc_internal_->load_string(inputText.c_str(), flag);
    if (r.status != pugi::xml_parse_status::status_ok) [[unlikely]]
    {
      cerr << SourceTrace(inputText, r.offset) << endl
           << "SyntaxError: " << r.description() << endl
           << string(4, ' ') << "at Document::setSource()" << endl
           << endl;
    }

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

  std::shared_ptr<Comment> Document::createComment(const string &data)
  {
    return make_shared<Comment>(data, getPtr<Document>());
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
    auto s = client_cssom::selectors::CSSelectorParser::parseSelectors(selectors);
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
    auto s = client_cssom::selectors::CSSelectorParser::parseSelectors(selectors);
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
    onStyleSheetsDidChange();
  }

  void Document::write(const std::string &markup)
  {
    // TODO(yorkie): implement the write logic.
    // This method is used to write the markup into the document.
    cerr << "Document::write() is not implemented yet." << endl
         << "Markup: " << markup << endl
         << "This method is used to write the markup into the document." << endl;
  }

  void Document::writeln(const std::string &markup)
  {
    // Write the markup followed by a newline
    write(markup + "\n");
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

      // Check if this is a viewport meta element and apply it
      auto meta_element = std::dynamic_pointer_cast<HTMLMetaElement>(element);
      if (meta_element && meta_element->isViewportMeta())
      {
        onViewportMetaChanged(meta_element);
      }
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

        if (Node::Is<Comment>(node))
        {
          auto &comment = Node::AsChecked<Comment>(node);
          s.append("<!--");
          s.append(comment.data());
          s.append("-->");
          isElementNode = false;
          return;
        }

        // TODO: Support ProcessingInstruction and DocumentType.
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

  void HTMLDocument::invalidateDocumentCache()
  {
    styleCache().invalidateCache();
    dirty_root_text_or_element_ = documentElement();
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

  bool HTMLDocument::simulateScrollWithOffset(float offsetX, float offsetY)
  {
    auto layoutBox = layoutView();
    assert(layoutBox != nullptr && "The layout box is not set.");
    if (offsetX == 0 && offsetY == 0)
      return false;

    bool scrolled = layoutBox->scrollBy(glm::vec3(offsetX, offsetY, 0));
    if (scrolled)
    {
      // TODO(yorkie): no need to invalidate the document cache on scroll, will optimize later that only invalidate
      // the cache for the affected elements.
      invalidateDocumentCache();

      // Throttle scroll events for better performance
      if (!shouldThrottleScrollEvent())
      {
        // Dispatch the scroll event.
        last_scroll_event_time_ = std::chrono::steady_clock::now();
        dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));
      }
    }
    return scrolled;
  }

  bool HTMLDocument::shouldThrottleScrollEvent() const
  {
    auto now = std::chrono::steady_clock::now();
    return (now - last_scroll_event_time_) < scroll_throttle_duration_;
  }

  void Document::onViewportMetaChanged(std::shared_ptr<dom::HTMLMetaElement> meta_element)
  {
    if (!meta_element || !meta_element->isViewportMeta())
      return;

    auto viewport_meta = meta_element->parseViewportMeta();
    if (!viewport_meta)
      return;

    // Apply viewport settings to the window
    auto window = defaultView();
    if (window)
    {
      window->applyViewportMeta(*viewport_meta);
    }
  }
}
