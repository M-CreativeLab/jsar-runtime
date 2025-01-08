#pragma once

#include <napi.h>
#include "./css_style_declaration.hpp"

namespace bindings
{
  namespace cssom
  {
    inline Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      cssombinding::CSSStyleDeclaration::Init(env);
      return exports;
    }
  }
}
