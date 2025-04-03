#include <client/builtin_scene/scene.hpp>
#include <client/builtin_scene/text.hpp>
#include <client/dom/text.hpp>

#include "./layout_text.hpp"

namespace client_layout
{
  using namespace std;
  using namespace builtin_scene;

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
    // TODO: support offset such as <div>foo<b>!</b>bar</div>, it should created as 2 `LayoutText` objects.
    return textNode()->data();
  }

  bool LayoutText::computeLayout(const ConstraintSpace &avilableSpace)
  {
    cout << "LayoutText::computeLayout: " << avilableSpace.width() << ", " << avilableSpace.height() << endl;
    return LayoutObject::computeLayout(avilableSpace);
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
