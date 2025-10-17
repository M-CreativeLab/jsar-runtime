#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/document_fragment.hpp>
#include "./node.hpp"

namespace script_bindings
{
  namespace dom_bindings
  {
    class DocumentFragment;
    using DocumentFragmentBase = scripting_base::ObjectWrap<DocumentFragment, dom::DocumentFragment, Node>;

    /**
     * DocumentFragment wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::DocumentFragment objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM DocumentFragment interface for creating lightweight document objects.
     */
    class DocumentFragment : public DocumentFragmentBase
    {
      using DocumentFragmentBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "DocumentFragment";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      DocumentFragment(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Methods
      void Append(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Prepend(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetElementById(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetElementsByTagName(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetElementsByClassName(const v8::FunctionCallbackInfo<v8::Value> &info);
      void QuerySelector(const v8::FunctionCallbackInfo<v8::Value> &info);
      void QuerySelectorAll(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Property getters
      void ChildElementCountGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ChildrenGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void FirstElementChildGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LastElementChildGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
