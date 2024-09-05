#include "binding.hpp"
#include "./node.hpp"
#include "./element.hpp"
#include "./html_element.hpp"
#include "./html_html_element.hpp"
#include "./html_head_element.hpp"
#include "./html_body_element.hpp"
#include "./html_meta_element.hpp"
#include "./audio.hpp"
#include "./dom_parser.hpp"
#include "./document.hpp"

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
        dombinding::HTMLHtmlElement::Init(env);
        dombinding::HTMLHeadElement::Init(env);
        dombinding::HTMLBodyElement::Init(env);
        dombinding::HTMLMetaElement::Init(env);
      }
      dombinding::DOMParser::Init(env);
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);
      Audio::Init(env, exports);
      return exports;
    }
  }
}
