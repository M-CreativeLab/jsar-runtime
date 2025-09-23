#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  namespace dom_bindings
  {
    class DOMStringMap;
    using DOMStringMapBase = scripting_base::ObjectWrap<DOMStringMap>;

    class DOMStringMap : public DOMStringMapBase
    {
      using DOMStringMapBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "DOMStringMap";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate);
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      DOMStringMap(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Indexed property handlers for dynamic dataset access
      static void IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void IndexedPropertyQuery(uint32_t index, const v8::PropertyCallbackInfo<v8::Integer> &info);
      static void IndexedPropertyDeleter(uint32_t index, const v8::PropertyCallbackInfo<v8::Boolean> &info);
      static void IndexedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);

      // Named property handlers for dynamic dataset access
      static void NamedPropertyGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NamedPropertySetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NamedPropertyQuery(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Integer> &info);
      static void NamedPropertyDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean> &info);
      static void NamedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);
    };
  }
}
