#include "./placeholders.hpp"
#include "common/command_buffers/webgl_placeholders.hpp"

namespace bindings
{
  namespace webgl
  {
    void InitPlaceholders(Napi::Env env, Napi::Object exports)
    {
      auto jsPlaceholdersObject = Napi::Object::New(env);
      jsPlaceholdersObject.Set("useRightHandedKey", Napi::String::New(env, WEBGL_PLACEHOLDERS_USE_RIGHTHANDED_KEY));
      jsPlaceholdersObject.Set("inverseMatrixKey", Napi::String::New(env, WEBGL_PLACEHOLDERS_INVERSE_MATRIX_KEY));
      jsPlaceholdersObject.Set("placeholderIdKey", Napi::String::New(env, WEBGL_PLACEHOLDERS_PLACEHOLDER_ID_KEY));

      auto jsIdsObject = Napi::Object::New(env);
#define XX(id, v) jsIdsObject.Set(#id, Napi::Number::New(env, v));
      PLACEHOLDER_ID_MAP(XX)
#undef XX
      jsPlaceholdersObject.Set("placeholderIds", jsIdsObject);
      exports.Set("placeholders", jsPlaceholdersObject);
    }
  }
}
