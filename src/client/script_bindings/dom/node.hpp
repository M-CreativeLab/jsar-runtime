#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/node.hpp>
#include <client/script_bindings/event_target.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace dom_bindings
    {
      class Node;
      using NodeBase = scripting_base::ObjectWrap<Node, dom::Node, EventTarget>;

      /**
     * Node wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Node objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Node interface including properties like nodeType,
     * nodeName, parentNode and methods like appendChild, removeChild, etc.
     */
      class Node : public NodeBase
      {
        using NodeBase::ObjectWrap;

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

      private:
        // Property getters
        void BaseURIGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void NodeNameGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void NodeTypeGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void NodeValueGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void NodeValueSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void IsConnectedGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ParentNodeGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ParentElementGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ChildNodesGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void FirstChildGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void LastChildGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void NextSiblingGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void PreviousSiblingGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void OwnerDocumentGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TextContentGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TextContentSetter(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Methods
        void AppendChild(const v8::FunctionCallbackInfo<v8::Value> &args);
        void RemoveChild(const v8::FunctionCallbackInfo<v8::Value> &args);
        void InsertBefore(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ReplaceChild(const v8::FunctionCallbackInfo<v8::Value> &args);
        void CloneNode(const v8::FunctionCallbackInfo<v8::Value> &args);
        void HasChildNodes(const v8::FunctionCallbackInfo<v8::Value> &args);
        void Contains(const v8::FunctionCallbackInfo<v8::Value> &args);
        void IsDefaultNamespace(const v8::FunctionCallbackInfo<v8::Value> &args);
        void IsEqualNode(const v8::FunctionCallbackInfo<v8::Value> &args);
        void IsSameNode(const v8::FunctionCallbackInfo<v8::Value> &args);
        void Normalize(const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor
