#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/node_list-inl.hpp>

namespace script_bindings
{
  namespace dom_bindings
  {
    class NodeList;
    using NodeListBase = scripting_base::ObjectWrap<NodeList, dom::NodeListApi>;

    class NodeList : public NodeListBase
    {
      using NodeListBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "NodeList";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      NodeList(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Core methods
      void Item(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Entries(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ForEach(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Keys(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Values(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Property getters
      void LengthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Indexed property handlers for array-like access
      static void IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void IndexedPropertyQuery(uint32_t index, const v8::PropertyCallbackInfo<v8::Integer> &info);
      static void IndexedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);
    };
  }
}