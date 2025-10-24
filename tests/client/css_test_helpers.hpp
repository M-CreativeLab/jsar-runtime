#include <memory>
#include <client/dom/browsing_context.hpp>
#include <client/html/html_element.hpp>
#include <client/cssom/style_traits.hpp>

using namespace endor;
using namespace endor::client_cssom::values;

namespace tests
{
  // Helper function to create a minimal context for testing
  inline static computed::Context CreateComputedContext()
  {
    static auto browsingContext = make_shared<dom::BrowsingContext>();
    static auto document = dom::Document::Make("text/html", dom::DocumentType::kHTML, browsingContext);
    static auto element = make_shared<dom::HTMLElement>("div", document);
    return computed::Context::From(element);
  }
}
