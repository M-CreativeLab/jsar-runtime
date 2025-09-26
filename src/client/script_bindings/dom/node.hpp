#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/node.hpp>
#include <client/script_bindings/event_target.hpp>

namespace script_bindings
{
  namespace dom_bindings
  {
    class Node;
    using NodeBase = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>;

    /**
     * Node wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Node objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Node interface including properties like nodeType,
     * nodeName, parentNode and methods like appendChild, removeChild, etc.
     */
    class Node : public NodeBase
    {

    public:
      /**
       * The name of the Node class for V8.
       */
      static std::string Name()
      {
        return "Node";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Node> nativeNode);

    public:
      Node(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void NodeNameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NodeTypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NodeValueGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NodeValueSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ParentNodeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FirstChildGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void LastChildGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NextSiblingGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void PreviousSiblingGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OwnerDocumentGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TextContentGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TextContentSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void AppendChild(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RemoveChild(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void InsertBefore(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ReplaceChild(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CloneNode(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void HasChildNodes(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Contains(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}