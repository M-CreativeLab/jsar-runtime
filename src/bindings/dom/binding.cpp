#include "binding.hpp"
#include "./node.hpp"
#include "./element.hpp"
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
      dombinding::DOMParser::Init(env, exports);
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);
      Audio::Init(env, exports);
      return exports;
    }
  }
}
