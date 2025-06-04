#include <crates/bindings.hpp>
#include <client/cssom/css_stylesheet.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>

#include "./html_link_element.hpp"

namespace dom
{
  using namespace std;
  using namespace crates;

  void HTMLLinkElement::createdCallback()
  {
    HTMLElement::createdCallback();
    renderable = false;

    if (hasAttribute("as"))
      asType_ = HTMLLinkElement::InitContentType(getAttribute("as"));
    if (hasAttribute("rel"))
      relType_ = HTMLLinkElement::InitRelType(getAttribute("rel"));

    if (hasAttribute("blocking"))
      blocking_ = getBoolAttribute("blocking");
    if (hasAttribute("disabled"))
      disabled_ = getBoolAttribute("disabled");
  }

  void HTMLLinkElement::connectedCallback()
  {
    HTMLElement::connectedCallback();

    shared_ptr<BrowsingContext> browsingContext = ownerDocument->lock()->browsingContext;
    if (!disabled())
    {
      string src = href();
      auto resourceUrl = UrlHelper::CreateUrlStringWithPath(baseURI, src);
      if (resourceUrl == "")
      {
        cerr << "Failed to parse the URL: " << src << endl;
      }
      else
      {
        browsingContext->fetchTextSourceResource(resourceUrl, [this](const string &source)
                                                 { onResourceLoaded(source); });
      }
    }
  }

  void HTMLLinkElement::disconnectedCallback()
  {
    HTMLElement::disconnectedCallback();

    auto ownerDocument = getOwnerDocumentReference();
    if (styleSheet_ && ownerDocument != nullptr)
    {
      auto &styleSheets = ownerDocument->stylesheets_;
      styleSheets.erase(std::remove(styleSheets.begin(), styleSheets.end(), styleSheet_), styleSheets.end());
      styleSheet_.reset();

      // Invalidate the cache after removing the stylesheet
      // TODO: Just remove the cache for this sheet?
      ownerDocument->style_cache_.invalidateCache();
    }
  }

  bool HTMLLinkElement::getBoolAttribute(const std::string &name)
  {
    auto value = getAttribute(name);
    return value == "true" || value == name;
  }

  void HTMLLinkElement::onResourceLoaded(const string &source)
  {
    if (rel() == RelType::kStylesheet)
      loadStyleSheet(source);
    // TODO: support other types of resources
  }

  void HTMLLinkElement::loadStyleSheet(const string &cssText)
  {
    client_cssom::CSSStyleSheetInit init{baseURI, disabled()};
    auto sheet = make_shared<client_cssom::CSSStyleSheet>(init);
    if (blocking())
      sheet->replaceSync(cssText);
    else
      sheet->replace(cssText);

    // Update the sheet
    styleSheet_ = sheet;
    auto &document = getOwnerDocumentChecked();
    document.stylesheets_.push_back(sheet);
    document.style_cache_.invalidateCache();
  }
}
