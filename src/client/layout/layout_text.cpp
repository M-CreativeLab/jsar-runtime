#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <common/utility.hpp>
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

  // Remove the leading and trailing whitespaces, and \n, \r, \t characters.
  string formatText(const string &text)
  {
    if (text.empty())
      return ""; // Early exit for empty string

    size_t start = text.find_first_not_of(" \t\n\r");
    if (start == string::npos)
    {
      return ""; // Entire string is whitespace
    }

    size_t end = text.find_last_not_of(" \t\n\r");
    // Since we already checked `start`, `end` cannot be npos here
    return text.substr(start, end - start + 1);
  }

  string LayoutText::plainText() const
  {
    // TODO(yorkie): support offset such as <div>foo<b>!</b>bar</div>, it should created as 2 `LayoutText` objects.
    // TODO(yorkie): support caching for `formatText` to avoid the repeated formatting.
    return formatText(textNode()->data());
  }

  const ConstraintSpace LayoutText::adjustSpace(const ConstraintSpace &inputSpace) const
  {
    float maxWidth = inputSpace.width();
    if (TR_UNLIKELY(maxWidth <= 0))
      return inputSpace;
    if (TR_UNLIKELY(!hasSceneComponent<WebContent>()))
      return inputSpace;

    string textContent = transformAndSecureText(plainText());
    shared_ptr<WebContent> webContentComponent = getSceneComponent<WebContent>();
    assert(webContentComponent != nullptr && "The web content must be set.");

    if (textContent.size() == 0)
      return ConstraintSpace::Zero();

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

  void LayoutText::entityDidCreate(ecs::EntityId entity)
  {
    LayoutObject::entityDidCreate(entity);

    auto appendText = [this, &entity](Scene &scene)
    {
      auto textContent = transformAndSecureText(plainText());
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

  void LayoutText::styleWillChange(client_cssom::CSSStyleDeclaration &newStyle)
  {
    LayoutObject::styleWillChange(newStyle);

    // TODO(yorkie): implement StyleDifference to check the changed properties to avoid the repeated update.
    if (newStyle.hasProperty("text-transform") ||
        newStyle.hasProperty("-webkit-text-security"))
    {
      auto textComponent = getSceneComponent<Text2d>();
      if (textComponent != nullptr)
          textComponent->content = transformAndSecureText(plainText());
    }

    // Ignore the `LayoutText` in layout tree if the text content is empty.
    if (isEmptyText())
      newStyle.setProperty("display", "none");
  }

  void LayoutText::didComputeLayoutOnce(const ConstraintSpace &avilableSpace)
  {
    // adjust the space size by the text content.
    ConstraintSpace adjustedSpace = adjustSpace(avilableSpace);

    // Update the content size of the formatting context.
    formattingContext().setContentSize(adjustedSpace.width(),
                                       adjustedSpace.height());
  }

  // TODO(yorkie): support offset mapping for the secure text.
  string secureText(const string &plain, char16_t mask)
  {
    string secured = plain;
    for (auto &c : secured)
      c = mask;
    return secured;
  }

  string LayoutText::transformAndSecureText(const string &original) const
  {
    auto &textStyle = dom::Node::As<dom::Text>(node())->adoptedStyle();
    if (textStyle.hasProperty("-webkit-text-security"))
    {
      auto textSecurity = textStyle.getPropertyValue("-webkit-text-security");
      if (textSecurity == "disc")
        return secureText(original, kBulletCharacter);
      else if (textSecurity == "circle")
        return secureText(original, kWhiteBulletCharacter);
      else if (textSecurity == "square")
        return secureText(original, kBlackSquareCharacter);
    }

    if (textStyle.hasProperty("text-transform"))
    {
      auto textTransform = textStyle.getPropertyValue("text-transform");
      if (textTransform == "uppercase")
        return ToUpperCase(original);
      else if (textTransform == "lowercase")
        return ToLowerCase(original);
      else if (textTransform == "capitalize")
        return ToCapitalize(original);
    }
    return original;
  }
}
