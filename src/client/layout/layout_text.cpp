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
      , plain_text_(nullopt)
      , transformed_text_(nullopt)
  {
  }

  shared_ptr<dom::Text> LayoutText::textNode() const
  {
    return static_pointer_cast<dom::Text>(node());
  }

  // Format text according to CSS white-space property.
  string formatText(const string &text, const client_cssom::values::computed::WhiteSpace &whiteSpace)
  {
    if (text.empty())
      return "";

    string formatted = text;

    // Handle space and tab collapsing
    if (whiteSpace.shouldCollapseSpaces())
    {
      // Replace sequences of spaces/tabs with single space
      string result;
      bool lastWasSpace = false;
      for (char c : formatted)
      {
        if (c == ' ' || c == '\t')
        {
          if (!lastWasSpace)
          {
            result += ' ';
            lastWasSpace = true;
          }
        }
        else
        {
          result += c;
          lastWasSpace = false;
        }
      }
      formatted = result;
    }

    // Handle newline collapsing
    if (whiteSpace.shouldCollapseNewlines())
    {
      // Replace newlines with spaces (which may then be collapsed)
      string result;
      for (char c : formatted)
      {
        if (c == '\n' || c == '\r')
        {
          result += ' ';
        }
        else
        {
          result += c;
        }
      }
      formatted = result;
    }

    // Handle leading/trailing whitespace for collapsing modes
    if (whiteSpace.shouldCollapseSpaces())
    {
      size_t start = formatted.find_first_not_of(" \t");
      if (start == string::npos)
      {
        return ""; // Entire string is whitespace
      }

      size_t end = formatted.find_last_not_of(" \t");
      formatted = formatted.substr(start, end - start + 1);
    }

    return formatted;
  }

  string LayoutText::plainText() const
  {
    if (!plain_text_.has_value())
    {
      auto computedStyle = style();
      if (computedStyle.has_value())
      {
        plain_text_ = formatText(textNode()->data(), computedStyle->whiteSpace());
      }
      else
      {
        // Fallback to normal white-space behavior
        client_cssom::values::computed::WhiteSpace normalWhiteSpace;
        plain_text_ = formatText(textNode()->data(), normalWhiteSpace);
      }
    }
    return plain_text_.value_or("");
  }

  string LayoutText::transformedText() const
  {
    if (!transformed_text_.has_value())
      transformed_text_ = transformAndSecureText(plainText());
    return transformed_text_.value_or("");
  }

  const ConstraintSpace LayoutText::adjustSpace(const ConstraintSpace &inputSpace) const
  {
    float maxWidth = inputSpace.width();
    if (TR_UNLIKELY(maxWidth < 0))
      return inputSpace;
    if (TR_UNLIKELY(!hasSceneComponent<WebContent>()))
      return inputSpace;

    string textContent = transformedText();
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
                        ? maxWidth + 1.0f // Add a small margin to avoid rounding issues
                        : numeric_limits<float>::infinity());

    // Use longest line width and height as the constraint space.
    return ConstraintSpace(paragraph->getLongestLine(),
                           paragraph->getHeight());
  }

  void LayoutText::textDidChange()
  {
    auto computedStyle = style();
    if (computedStyle.has_value())
    {
      plain_text_ = formatText(textNode()->data(), computedStyle->whiteSpace());
    }
    else
    {
      // Fallback to normal white-space behavior
      client_cssom::values::computed::WhiteSpace normalWhiteSpace;
      plain_text_ = formatText(textNode()->data(), normalWhiteSpace);
    }
    transformed_text_ = transformAndSecureText(plainText());
    is_text_content_dirty_ = true;

    formattingContext().setIsEmpty(isEmptyText());
    adjustTextContentSize(parent()->fragment());

    auto updateText = [this](Scene &scene)
    {
      bool shouldUpdateContent = false;

      auto textComponent = scene.getComponent<Text2d>(entity());
      if (textComponent != nullptr)
      {
        textComponent->content = transformedText();
        shouldUpdateContent = true;
      }

      if (shouldUpdateContent)
      {
        auto &webContentComponent = scene.getComponentChecked<WebContent>(entity());
        webContentComponent.setContentDirty(true);
      }
    };
    useSceneWithCallback(updateText);
  }

  void LayoutText::entityDidCreate(ecs::EntityId entity)
  {
    LayoutObject::entityDidCreate(entity);

    auto computedStyle = style();
    if (computedStyle.has_value())
    {
      plain_text_ = formatText(textNode()->data(), computedStyle->whiteSpace());
    }
    else
    {
      // Fallback to normal white-space behavior
      client_cssom::values::computed::WhiteSpace normalWhiteSpace;
      plain_text_ = formatText(textNode()->data(), normalWhiteSpace);
    }
    transformed_text_ = transformAndSecureText(plainText());
    is_text_content_dirty_ = true;

    auto appendText = [this, &entity](Scene &scene)
    {
      scene.addComponent(entity, Text2d(transformedText()));
      formattingContext().setIsEmpty(isEmptyText());
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

  void LayoutText::formattingContextDidSet(FormattingContext &context)
  {
    LayoutObject::formattingContextDidSet(context);
    context.setContentSizeEnabled(true);
  }

  void LayoutText::styleWillChange(client_cssom::ComputedStyle &new_style)
  {
    LayoutObject::styleWillChange(new_style);

    // TODO(yorkie): implement StyleDifference to check the changed properties to avoid the repeated update.
    if (new_style.hasProperty("text-transform") ||
        new_style.hasProperty("-webkit-text-security") ||
        new_style.hasProperty("white-space"))
    {
      // Re-format the plain text if white-space property changed
      if (new_style.hasProperty("white-space"))
      {
        plain_text_ = formatText(textNode()->data(), new_style.whiteSpace());
      }

      transformed_text_ = transformAndSecureText(plainText());
      is_text_content_dirty_ = true;

      auto textComponent = getSceneComponent<Text2d>();
      if (textComponent != nullptr)
        textComponent->content = transformedText();
    }

    // Update the text content if the text is empty.
    formattingContext().setIsEmpty(isEmptyText());
  }

  void LayoutText::styleDidChange()
  {
    LayoutObject::styleDidChange();

    updateLayer(containingScrollContainer(), false);
  }

  void LayoutText::didComputeLayoutOnce(const ConstraintSpace &avilableSpace)
  {
    adjustTextContentSize(avilableSpace);
  }

  void LayoutText::adjustTextContentSize(const ConstraintSpace &space)
  {
    if (last_space_.has_value() &&
        space == last_space_.value() &&
        !is_text_content_dirty_)
    {
      // No need to adjust if the space is not changed or the text content is not dirty.
      return;
    }

    ConstraintSpace adjustedSpace = adjustSpace(space);
    formattingContext().setContentSize(adjustedSpace.width(),
                                       adjustedSpace.height());
    last_space_ = adjustedSpace;
    is_text_content_dirty_ = false;
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
    const auto &textNode = dom::Node::AsChecked<const dom::Text>(node());
    if (textNode.hasAdoptedStyle())
    {
      const auto &textStyle = textNode.adoptedStyleRef();

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
    }
    return original;
  }
}
