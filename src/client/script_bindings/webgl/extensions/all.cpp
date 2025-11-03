#include <client/script_bindings/webgl/extensions/all.hpp>

// Supported Extensions
#include "./OES_element_index_uint.hpp"
#include "./OES_standard_derivatives.hpp"
#include "./OES_texture_float_linear.hpp"
#include "./OVR_multiview2.hpp"

using namespace std;
using namespace v8;

#define SUPPORTED_EXTENSIONS_MAP(XX) \
  XX(OES_element_index_uint)         \
  XX(OES_standard_derivatives)       \
  XX(OES_texture_float_linear)       \
  XX(OVR_multiview2)

namespace endor
{
  namespace script_bindings::webgl_bindings
  {
    unordered_map<string, Global<Value>> WebGLExtensions::NameToExtensionMap_;
    void WebGLExtensions::Initialize(v8::Isolate *isolate)
    {
#define INIT_EXTENSION(NAME) extensions::NAME::Initialize(isolate);

      SUPPORTED_EXTENSIONS_MAP(INIT_EXTENSION)
#undef INIT_EXTENSION
    }

    bool WebGLExtensions::HasExtension(const string &name)
    {
#define CHECK_EXTENSION(NAME)           \
  if (name == extensions::NAME::Name()) \
    return true;

      SUPPORTED_EXTENSIONS_MAP(CHECK_EXTENSION)
#undef CHECK_EXTENSION

      // Not found return false
      return false;
    }

    Local<Value> WebGLExtensions::GetExtension(Isolate *isolate, const string &name)
    {
      EscapableHandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Value> extensionValue = Null(isolate);
      if (NameToExtensionMap_.find(name) != NameToExtensionMap_.end())
      {
        extensionValue = NameToExtensionMap_[name].Get(isolate);
      }
      else
      {
#define CHECK_AND_CREATE_EXTENSION(NAME) \
  if (name == extensions::NAME::Name())  \
    extensionValue = extensions::NAME::NewInstance(isolate, nullptr);

        SUPPORTED_EXTENSIONS_MAP(CHECK_AND_CREATE_EXTENSION)
#undef CHECK_AND_CREATE_EXTENSION

        if (!extensionValue->IsNull())
        {
          NameToExtensionMap_[name].Reset(isolate, extensionValue);
        }
      }
      return scope.Escape(extensionValue);
    }
  }
} // namespace endor
