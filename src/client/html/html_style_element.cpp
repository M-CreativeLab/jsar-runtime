#include <client/cssom/css_stylesheet.hpp>
#include <client/dom/document.hpp>
#include "./html_style_element.hpp"

namespace dom
{
  using namespace std;

  void HTMLStyleElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);
    renderable = false;

    if (hasAttribute("blocking"))
      blocking = getAttribute("blocking") != "false";
    if (hasAttribute("disabled"))
      disabled = getAttribute("disabled") != "false";
  }

  void HTMLStyleElement::connectedCallback()
  {
    client_cssom::CSSStyleSheetInit init{baseURI, disabled};
    auto sheet = make_shared<client_cssom::CSSStyleSheet>(init);
    string cssText = textContent();
    if (!cssText.empty())
    {
      if (blocking)
        sheet->replaceSync(cssText);
      else
        sheet->replace(cssText);
    }

    // Update the sheet
    sheet_ = sheet;
    getOwnerDocumentChecked().appendStyleSheet(sheet);
  }
}
