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
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Node> handle);

    public:
      Node(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void BaseURIGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NodeNameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NodeTypeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NodeValueGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NodeValueSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void IsConnectedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ParentNodeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ParentElementGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ChildNodesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FirstChildGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void LastChildGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NextSiblingGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void PreviousSiblingGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void OwnerDocumentGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TextContentGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TextContentSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      void AppendChild(const v8::FunctionCallbackInfo<v8::Value> &info);
      void RemoveChild(const v8::FunctionCallbackInfo<v8::Value> &info);
      void InsertBefore(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ReplaceChild(const v8::FunctionCallbackInfo<v8::Value> &info);
      void CloneNode(const v8::FunctionCallbackInfo<v8::Value> &info);
      void HasChildNodes(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Contains(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IsDefaultNamespace(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IsEqualNode(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IsSameNode(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Normalize(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}