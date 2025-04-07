#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <client/per_process.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/builtin_scene/text.hpp>
#include <client/dom/text.hpp>

#include "./layout_text.hpp"
#include "./formatting_contexts-inl.hpp"

namespace client_layout
{
  using namespace std;
  using namespace builtin_scene;
  using namespace skia::textlayout;

  LayoutText::LayoutText(shared_ptr<dom::Text> textNode)
      : LayoutObject(textNode)
  {
  }

  shared_ptr<dom::Text> LayoutText::textNode() const
  {
    return static_pointer_cast<dom::Text>(node());
  }

  string LayoutText::plainText() const
  {
    // TODO(yorkie): support offset such as <div>foo<b>!</b>bar</div>, it should created as 2 `LayoutText` objects.
    return textNode()->data();
  }

  const ConstraintSpace LayoutText::adjustSpace(const ConstraintSpace &inputSpace) const
  {
    float maxWidth = inputSpace.width();
    if (TR_UNLIKELY(maxWidth <= 0))
      return inputSpace;
    if (TR_UNLIKELY(!hasSceneComponent<WebContent>()))
      return inputSpace;

    string textContent = plainText();
    shared_ptr<WebContent> webContentComponent = getSceneComponent<WebContent>();
    assert(webContentComponent != nullptr && "The web content must be set.");

    if (textContent.size() == 0)
      return inputSpace;

    auto paragraphStyle = webContentComponent->paragraphStyle();
    auto paragraphBuilder = ParagraphBuilder::make(paragraphStyle,
                                                   TrClientContextPerProcess::Get()->getFontCacheManager());
    paragraphBuilder->pushStyle(paragraphStyle.getTextStyle());
    paragraphBuilder->addText(textContent.c_str(), textContent.size());
    paragraphBuilder->pop();

    auto paragraph = paragraphBuilder->Build();
    paragraph->layout(maxWidth > 0
                          ? maxWidth
                          : numeric_limits<float>::infinity());

    // Use longest line width and height as the constraint space.
    return ConstraintSpace(paragraph->getLongestLine(),
                           paragraph->getHeight());
  }

  bool LayoutText::computeLayout(const ConstraintSpace &avilableSpace)
  {
    // adjust the space size by the text content.
    ConstraintSpace adjustedSpace = adjustSpace(avilableSpace);

    // Update the layout size.
    formattingContext().setContentSize(adjustedSpace.width(),
                                       adjustedSpace.height());

    // Execute the layout computation.
    return LayoutObject::computeLayout(adjustedSpace);
  }

  // Remove the leading and trailing whitespaces, and \n, \r, \t characters.
  string processTextContent(const string &text)
  {
    string result(text);
    size_t start = result.find_first_not_of(" \t\n\r");
    if (start == string::npos || start >= result.length())
      start = 0;
    size_t end = result.find_last_not_of(" \t\n\r");
    if (end == string::npos || end >= result.length())
      end = result.length();
    return result.substr(start, end - start + 1);
  }

  void LayoutText::entityDidCreated(ecs::EntityId entity)
  {
    LayoutObject::entityDidCreated(entity);

    auto appendText = [this, &entity](Scene &scene)
    {
      string textContent = plainText();
      // Remove the leading and trailing whitespaces, and \n, \r, \t characters.
      textContent = processTextContent(textContent);
      scene.addComponent(entity, Text2d(textContent));
    };
    useSceneWithCallback(appendText);
  }

  void LayoutText::entityWillBeDestroyed(builtin_scene::ecs::EntityId entity)
  {
    auto removeText = [&entity](Scene &scene)
    {
      scene.removeComponent<Text2d>(entity);
    };
    useSceneWithCallback(removeText);

    LayoutObject::entityWillBeDestroyed(entity);
  }
}
