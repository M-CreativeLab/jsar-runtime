#include <client/dom/document-inl.hpp>
#include <client/dom/all_html_elements.hpp>

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

  shared_ptr<LayoutView> LayoutView::Make(shared_ptr<dom::Document> document)
  {
    auto view = make_shared<LayoutView>(document);
    assert(view != nullptr && "Failed to create the LayoutView for the document.");

    view->setDisplay(DisplayType::Block());
    view->createEntity();
    return view;
  }

  LayoutView::LayoutView(shared_ptr<dom::Document> document)
      : LayoutBlockFlow(document),
        taffyNodeAllocator_(make_shared<crates::layout2::Allocator>())
  {
  }

  size_t LayoutView::computeMinimumWidth()
  {
    return 0;
  }

  void LayoutView::debugPrint(const string &message, LayoutView::DebugOptions options) const
  {
    if (TR_UNLIKELY(options.disabled))
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
      }
      else if (object.isLayoutBlock() && options.showBlockRecursively)
      {
        auto &block = dynamic_cast<const LayoutBlock &>(object);
        depth++;
        for (const auto &child : block.childrenRef())
          printObject(child);
        depth--;
      }
    };

    // Print the view tree.
    cout << "LayoutView (" << message << ")" << endl;
    for (const auto &child : childrenRef())
      printObject(child);

    if (options.printFormattingContext)
      debugPrintFormattingContext();
  }

  shared_ptr<LayoutBoxModelObject> LayoutView::createBox(const string &displayStr, shared_ptr<dom::Element> element,
                                                         shared_ptr<LayoutBlock> parentBlock)
  {
    shared_ptr<LayoutBoxModelObject> boxObject = makeBox(displayStr, element);
    if (parentBlock != nullptr)
      parentBlock->addChild(boxObject);
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
    auto textObject = makeText(textNode);
    parentBox->addChild(textObject);

    assert(textObject->parent() != nullptr && "Inserted text object must have a parent box.");
    textObject->createEntity();
    return textObject;
  }

  shared_ptr<LayoutBoxModelObject> LayoutView::replaceBox(shared_ptr<LayoutBoxModelObject> oldBox,
                                                          const string &newDisplayStr)
  {
    auto element = dom::Node::As<dom::Element>(oldBox->node());
    auto newBox = makeBox(newDisplayStr, element);

    // Move the children from the old box to the new box.
    if (newBox->isLayoutBlock() && oldBox->isLayoutBlock())
    {
      auto newBlock = dynamic_pointer_cast<LayoutBlock>(newBox);
      auto oldBlock = dynamic_pointer_cast<LayoutBlock>(oldBox);
      assert(newBlock != nullptr && oldBlock != nullptr && "The new and old boxes must be block boxes.");
      newBlock->useChildrenFrom(oldBlock);
    }

    auto parent = oldBox->parent();
    parent->replaceChild(newBox, oldBox);
    newBox->useEntity(oldBox);
    return newBox;
  }

  void LayoutView::removeObject(shared_ptr<LayoutObject> object)
  {
    assert(object->parent() != nullptr && "The object to be removed must have a parent.");
    auto parent = object->parent();
    parent->removeChild(object);
  }

  shared_ptr<LayoutBoxModelObject> LayoutView::makeBox(const string &displayStr, shared_ptr<dom::Element> element)
  {
    shared_ptr<LayoutBoxModelObject> boxObject = nullptr;
    DisplayType display = DisplayType::Make(displayStr);

    // Create LayoutImage for the cases: <img>, <picture> and content: url().
    // TODO(yorkie): support <picture> element and content case.
    if (dom::Node::Is<dom::HTMLImageElement>(element))
      boxObject = make_shared<LayoutImage>(element);

    // Skip the box creation for the display type of "none".
    if (display.isNone())
      boxObject = make_shared<LayoutNone>(element);

    // Create a box object via the display type for non-replaced elements.
    if (boxObject == nullptr)
    {
      // Non replaced elements to create box objects.
      if (display.outside == DisplayOutside::kBlock)
      {
        if (display.inside == DisplayInside::kFlow ||
            display.inside == DisplayInside::kFlowRoot)
          boxObject = make_shared<LayoutBlockFlow>(element);
        else if (display.inside == DisplayInside::kFlex)
          boxObject = make_shared<LayoutFlexibleBox>(element);
        else if (display.inside == DisplayInside::kGrid)
          boxObject = make_shared<LayoutGrid>(element);
      }
      else if (display.outside == DisplayOutside::kInline)
      {
        boxObject = make_shared<LayoutInline>(element);
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

  shared_ptr<LayoutText> LayoutView::makeText(shared_ptr<dom::Text> textNode)
  {
    auto textObject = make_shared<LayoutText>(textNode);
    textObject->setFormattingContext(DisplayType::Inline());
    return textObject;
  }
}
