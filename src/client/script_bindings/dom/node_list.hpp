#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/node_list.hpp>

namespace script_bindings
{
  class NodeList;
  using NodeListBase = scripting_base::ObjectWrap<NodeList, dom::NodeList>;

  /**
   * NodeList wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::NodeList objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM NodeList interface for collections of DOM nodes.
   */
  class NodeList : public NodeListBase
  {
    using NodeListBase::ObjectWrap;

  public:
    /**
     * The name of the NodeList class for V8.
     */
    static std::string Name()
    {
      return "NodeList";
    }

    /**
     * Configure the V8 function template with NodeList properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 NodeList instance from a native dom::NodeList.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::NodeList> nativeNodeList);

    /**
     * Initialize the NodeList class and register it with V8.
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

  public:
    NodeList(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Core methods
    static void Item(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Entries(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void ForEach(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Keys(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Values(const v8::FunctionCallbackInfo<v8::Value> &info);

    // Property getters
    static void LengthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Indexed property handlers for array-like access
    static void IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void IndexedPropertyQuery(uint32_t index, const v8::PropertyCallbackInfo<v8::Integer> &info);
    static void IndexedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);
  };
}