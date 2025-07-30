#pragma once

#include "../../client/nn/webnn_context.hpp"
#include <napi.h>

namespace transmute {
namespace nn {

// WebNN API binding for Node.js/N-API
class WebNNBinding {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  
  // Factory method to create WebNN context
  static Napi::Value CreateContext(const Napi::CallbackInfo& info);
  
private:
  WebNNBinding() = default;
};

} // namespace nn
} // namespace transmute