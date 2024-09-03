#include "binding.hpp"
#include "./audio.hpp"
#include "./dom_parser.hpp"
#include "./document.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      Audio::Init(env, exports);
      dombinding::DOMParser::Init(env, exports);
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);
      return exports;
    }
  }
}
