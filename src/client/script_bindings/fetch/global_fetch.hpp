#pragma once

#include <node/v8.h>
#include <client/dom/document.hpp>

namespace endor
{
  namespace script_bindings
  {
    class GlobalFetch
    {
    public:
      static v8::Local<v8::Value> Fetch(std::shared_ptr<dom::Document> document,
                                        const v8::FunctionCallbackInfo<v8::Value> &args);
      static v8::Local<v8::Value> FetchLater(std::shared_ptr<dom::Document> document,
                                             const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
} // namespace endor
