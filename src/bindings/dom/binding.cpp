#include "binding.hpp"
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
      dombinding::Node::Init(env);
      dombinding::Element::Init(env);
      dombinding::HTMLElement::Init(env);
      dombinding::HTMLMediaElement::Init(env);
      {
#define XX(tagNameStr, className) dombinding::className::Init(env);
        TYPED_ELEMENT_MAP(XX)
#undef XX
      }
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);

      dombinding::DOMParser::Init(env);
      dombinding::BrowsingContext::Init(env, exports);
      dombinding::WorkerContext::Init(env, exports);
      return exports;
    }
  }
}
