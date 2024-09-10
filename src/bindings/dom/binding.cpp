#include "binding.hpp"
#include "./node.hpp"
#include "./element.hpp"
#include "./all_html_elements.hpp"
#include "./audio.hpp"
#include "./document.hpp"
#include "./dom_parser.hpp"
#include "./rendering_context.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      dombinding::Node::Init(env);
      dombinding::Element::Init(env);
      dombinding::HTMLElement::Init(env);
      {
#define XX(tagNameStr, className) dombinding::className::Init(env);
        TYPED_ELEMENT_MAP(XX)
#undef XX
        Audio::Init(env, exports);
      }
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);

      dombinding::DOMParser::Init(env);
      dombinding::DocumentRenderingContext::Init(env, exports);
      return exports;
    }
  }
}
