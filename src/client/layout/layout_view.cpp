#include <client/browser/window.hpp>
#include <client/dom/document-inl.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/values/computed/context.hpp>
#include <client/html/all_html_elements.hpp>

#include "./layout_view.hpp"
#include "./layout_object.hpp"
#include "./layout_none.hpp"
#include "./layout_inline.hpp"
#include "./layout_block.hpp"
#include "./layout_flexible_box.hpp"
#include "./layout_grid.hpp"
#include "./layout_image.hpp"

namespace client_layout
{
  using namespace std;
  using namespace browser;

  shared_ptr<LayoutView> LayoutView::Make(shared_ptr<dom::Document> document, const Window &window)
  {
    auto view = make_shared<LayoutView>(document, window);
    assert(view != nullptr && "Failed to create the LayoutView for the document.");

    view->setDisplay(DisplayType::Block());
    view->createEntity();

    auto context = client_cssom::values::computed::Context::From(document);
    client_cssom::CSSStyleDeclaration initialStyle;
    initialStyle.setProperty("width", "100%");
    initialStyle.setProperty("height", "100%");

    view->setStyle(client_cssom::ComputedStyle(initialStyle, context));
    return view;
  }

  LayoutView::LayoutView(shared_ptr<dom::Document> document, const Window &window)
      : LayoutBlockFlow(document),
        viewport(window.innerWidth(), window.innerHeight(), window.innerDepth()),
        taffy_node_allocator_(make_shared<crates::layout2::Allocator>()),
        hit_test_count_(0),
        hit_test_cache_hits_(0),
        hit_test_cache_(make_unique<HitTestCache>())
  {
  }

  bool LayoutView::computeLayout(const ConstraintSpace &avilableSpace)
  {
    function<void(LayoutObject &, const LayoutObject &)> traverseChildNode =
        [&traverseChildNode](LayoutObject &object, const LayoutObject &parent)
    {
      if (object.isNone())
        return;
      object.didComputeLayoutOnce(parent.fragment());

      // traverse the children of the block or inline object.
      if (object.isLayoutBlock())
      {
        auto block = static_pointer_cast<LayoutBlock>(object.shared_from_this());
        for (shared_ptr<LayoutObject> child : block->childrenRef())
          traverseChildNode(*child, *block); // Just traverse the block's children but don't compute layout.
      }
      else if (object.isLayoutInline())
      {
        auto inlineObject = static_pointer_cast<LayoutInline>(object.shared_from_this());
        for (shared_ptr<LayoutObject> child : inlineObject->childrenRef())
          traverseChildNode(*child, *inlineObject); // Just traverse the inline's children but don't compute layout.
      }
    };

    // TODO(yorkie): support the lifecycle `willComputeLayout`?

    // Use taffy to compute the layout.
    bool r = LayoutBlockFlow::computeLayout(avilableSpace);

    // Traverse the children of the view and call `didComputeLayout` for each child.
    // This lifecycle `didComputeLayout` is used to setup for the next layout computation such as setting the content
    // size for text and replaced elements.
    for (shared_ptr<LayoutObject> child : childrenRef())
      traverseChildNode(*child, *this);

    return r;
  }

  bool LayoutView::hitTest(const HitTestRay &ray, HitTestResult &r)
  {
    // TODO(yorkie): support the update of the lifecycle, style and layout for the hit test.
    return hitTestNoLifecycleUpdate(ray, r);
  }

  bool LayoutView::hitTestNoLifecycleUpdate(const HitTestRay &hitTestRay, HitTestResult &r)
  {
    hit_test_count_ += 1;

    bool hit = false;
    HitTestResult cachedResult = r;
    if (hit_test_cache_->lookupCachedResult(hitTestRay, cachedResult))
    {
      hit_test_cache_hits_ += 1;
      r = cachedResult;
    }
    else
    {
      hit = hitTestAllPhases(r, hitTestRay, glm::vec3(0.0f));
      if (hit)
        hit_test_cache_->addCachedResult(hitTestRay, r);
    }
    return hit;
  }

  void LayoutView::clearHitTestCache()
  {
    hit_test_cache_->clear();
  }

  size_t LayoutView::computeMinimumWidth()
  {
    // TODO(yorkie): support the minimum width for the view.
    return 0;
  }

  void LayoutView::debugPrint(const string &message, LayoutView::DebugOptions options) const
  {
    if (TR_LIKELY(options.disabled))
      return;

    int depth = 1;
    function<void(const LayoutObject &)> printObject =
        [&options, &depth, &printObject](const LayoutObject &object)
    {
      string prefixSpaces = "";
      for (int i = 0; i < depth; i++)
        prefixSpaces += "  "; // use 2 spaces as the indentation.
      cout << prefixSpaces << object.debugName() << endl;

      if (object.isText())
      {
        // Print the text line
        auto &text = dynamic_cast<const LayoutText &>(object);
        string debugText = text.plainText();
        if (debugText.length() == 0)
          debugText = "(empty)";
        else if (debugText.length() > 25)
          debugText = "{\"" + debugText.substr(0, 25) + "\" ...}";
        else
          debugText = "\"" + debugText + "\"";

        if (options.showTextContent)
          cout << prefixSpaces << "  text run: " << debugText << endl;
        return;
      }

      if (options.showBlockRecursively)
      {
        shared_ptr<client_layout::LayoutObjectChildList> children = nullptr;
        if (object.isLayoutBlock())
        {
          auto &block = dynamic_cast<const LayoutBlock &>(object);
          children = block.children();
        }
        else if (object.isLayoutInline())
        {
          auto &inlineObject = dynamic_cast<const LayoutInline &>(object);
          children = inlineObject.children();
        }

        if (children != nullptr)
        {
          const auto &childrenRef = *children;
          depth++;
          for (shared_ptr<const LayoutObject> child : childrenRef)
            printObject(*child);
          depth--;
        }
      }
    };

    // Print the view tree.
    cout << "LayoutView (" << message << ")" << endl;
    for (shared_ptr<const LayoutObject> child : childrenRef())
      printObject(*child);

    if (options.printFormattingContext)
      debugPrintFormattingContext();
  }

  shared_ptr<LayoutBoxModelObject> LayoutView::createBox(const string &displayStr, shared_ptr<dom::Element> element,
                                                         shared_ptr<LayoutBlock> parentBlock,
                                                         shared_ptr<LayoutObject> beforeObject)
  {
    shared_ptr<LayoutBoxModelObject> boxObject = makeBox(displayStr, element);
    if (parentBlock != nullptr)
      parentBlock->addChild(boxObject, beforeObject);
    else
      addChild(boxObject);

    assert(boxObject->parent() != nullptr && "Inserted box must have a parent box.");
    boxObject->createEntity();
    return boxObject;
  }

  shared_ptr<LayoutText> LayoutView::createText(shared_ptr<dom::Text> textNode,
                                                shared_ptr<LayoutBoxModelObject> parentBox)
  {
    assert(parentBox != nullptr && "The parent box must be set for the text object.");
    shared_ptr<LayoutText> textObject = makeText(textNode);
    parentBox->addChild(textObject);

    assert(textObject->parent() != nullptr && "Inserted text object must have a parent box.");
    textObject->createEntity();
    return textObject;
  }

  void LayoutView::removeObject(shared_ptr<LayoutObject> object)
  {
    if (object == nullptr || object->parent() == nullptr)
      return;
    auto parent = object->parent();
    parent->removeChild(object);
  }

  unique_ptr<LayoutBoxModelObject> LayoutView::makeBox(const string &displayStr, shared_ptr<dom::Element> element)
  {
    unique_ptr<LayoutBoxModelObject> boxObject = nullptr;
    DisplayType display = DisplayType::Make(displayStr);

    // Create LayoutImage for the cases: <img>, <picture> and content: url().
    // TODO(yorkie): support <picture> element and content case.
    if (dom::Node::Is<dom::HTMLImageElement>(element))
      boxObject = make_unique<LayoutImage>(element);

    // Skip the box creation for the display type of "none".
    if (display.isNone())
      boxObject = make_unique<LayoutNone>(element);

    // Create a box object via the display type for non-replaced elements.
    if (boxObject == nullptr)
    {
      // Non replaced elements to create box objects.
      if (display.outside == DisplayOutside::kBlock)
      {
        if (display.inside == DisplayInside::kFlow ||
            display.inside == DisplayInside::kFlowRoot)
          boxObject = make_unique<LayoutBlockFlow>(element);
        else if (display.inside == DisplayInside::kFlex)
          boxObject = make_unique<LayoutFlexibleBox>(element);
        else if (display.inside == DisplayInside::kGrid)
          boxObject = make_unique<LayoutGrid>(element);
      }
      else if (display.outside == DisplayOutside::kInline)
      {
        boxObject = make_unique<LayoutInline>(element);
      }
      else
      {
        assert(false && "Unsupported display type: ruby.");
      }
    }

    assert(boxObject != nullptr && "Failed to make a box for the display type.");
    boxObject->setDisplay(display); // Update display to create the inner formatting context.
    return boxObject;
  }

  unique_ptr<LayoutText> LayoutView::makeText(shared_ptr<dom::Text> textNode)
  {
    auto textObject = make_unique<LayoutText>(textNode);
    textObject->setFormattingContext(DisplayType::Inline());
    return textObject;
  }
}
