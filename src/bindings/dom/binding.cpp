#include "binding.hpp"
#include "./event.hpp"
#include "./node.hpp"
#include "./element.hpp"
#include "./all_html_elements.hpp"
#include "./document.hpp"
#include "./dom_parser.hpp"
#include "./browsing_context.hpp"
#include "./worker_context.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      // Add events
      dombinding::InitEvents(env);

      // Add nodes & elements
      dombinding::Node::Init(env);
      dombinding::Element::Init(env);
      dombinding::HTMLElement::Init(env);
      dombinding::HTMLMediaElement::Init(env);
#define XX(tagNameStr, className) dombinding::className::Init(env);
      TYPED_ELEMENT_MAP(XX)
#undef XX

      // Add documents
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);

      // Add others
      dombinding::DOMParser::Init(env);
      dombinding::BrowsingContext::Init(env, exports);
      dombinding::WorkerContext::Init(env, exports);
      return exports;
    }
  }
}
