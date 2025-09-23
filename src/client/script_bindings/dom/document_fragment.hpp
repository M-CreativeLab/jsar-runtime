#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/document_fragment.hpp>
#include "./node.hpp"

namespace script_bindings
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
    /**
     * The name of the DocumentFragment class for V8.
     */
    static std::string Name()
    {
      return "DocumentFragment";
    }

    /**
     * Configure the V8 function template with DocumentFragment properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 DocumentFragment instance from a native dom::DocumentFragment.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::DocumentFragment> nativeFragment);

    /**
     * Initialize the DocumentFragment class and register it with V8.
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

  public:
    DocumentFragment(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // DOM Level 4 query methods
    static void QuerySelector(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void QuerySelectorAll(const v8::FunctionCallbackInfo<v8::Value> &info);
    
    // DOM Level 4 search methods
    static void GetElementById(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void GetElementsByTagName(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void GetElementsByClassName(const v8::FunctionCallbackInfo<v8::Value> &info);

    // Property getters
    static void ChildElementCountGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void FirstElementChildGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void LastElementChildGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}