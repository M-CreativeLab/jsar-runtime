#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_string_map.hpp>

namespace script_bindings
{
  class DOMStringMap;
  using DOMStringMapBase = scripting_base::ObjectWrap<DOMStringMap, dom::DOMStringMap>;

  /**
   * DOMStringMap wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::DOMStringMap objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM DOMStringMap interface for dataset attributes.
   */
  class DOMStringMap : public DOMStringMapBase
  {
    using DOMStringMapBase::ObjectWrap;

  public:
    /**
     * The name of the DOMStringMap class for V8.
     */
    static std::string Name()
    {
      return "DOMStringMap";
    }

    /**
     * Configure the V8 function template with DOMStringMap properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 DOMStringMap instance from a native dom::DOMStringMap.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::DOMStringMap> nativeStringMap);

    /**
     * Initialize the DOMStringMap class and register it with V8.
     */
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
    static void NamedPropertyGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void NamedPropertySetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void NamedPropertyQuery(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Integer> &info);
    static void NamedPropertyDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean> &info);
    static void NamedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);
  };
}