#include "binding.hpp"
#include "../../client/nn/webnn_context.hpp"

namespace transmute {
namespace nn {

Napi::Object WebNNBinding::Init(Napi::Env env, Napi::Object exports) {
  // Export the createContext function
  exports.Set(Napi::String::New(env, "createContext"), 
              Napi::Function::New(env, CreateContext));
  
  return exports;
}

Napi::Value WebNNBinding::CreateContext(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  
  try {
    // Parse options from JavaScript (if provided)
    WebNNContextOptions options;
    
    if (info.Length() > 0 && info[0].IsObject()) {
      Napi::Object opts = info[0].As<Napi::Object>();
      
      if (opts.Has("deviceType")) {
        std::string deviceType = opts.Get("deviceType").As<Napi::String>().Utf8Value();
        if (deviceType == "cpu") {
          options.deviceType = WebNNDeviceType::CPU;
        } else if (deviceType == "gpu") {
          options.deviceType = WebNNDeviceType::GPU;
        } else if (deviceType == "npu") {
          options.deviceType = WebNNDeviceType::NPU;
        }
      }
      
      if (opts.Has("powerPreference")) {
        std::string powerPref = opts.Get("powerPreference").As<Napi::String>().Utf8Value();
        if (powerPref == "high-performance") {
          options.powerPreference = WebNNPowerPreference::HighPerformance;
        } else if (powerPref == "low-power") {
          options.powerPreference = WebNNPowerPreference::LowPower;
        }
      }
    }
    
    // Create WebNN context
    auto context = WebNNContext::Create(options);
    
    // For now, return a simple object indicating success
    // In a full implementation, you would wrap the context in a JavaScript class
    Napi::Object result = Napi::Object::New(env);
    result.Set("status", Napi::String::New(env, "success"));
    result.Set("deviceType", Napi::String::New(env, "cpu")); // TODO: return actual device type
    
    return result;
    
  } catch (const std::exception& e) {
    Napi::TypeError::New(env, e.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

} // namespace nn
} // namespace transmute