#include "./node.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    // static
    void Node::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add EventTarget methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "addEventListener").ToLocalChecked(),
                           FunctionTemplate::New(isolate, AddEventListener));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "removeEventListener").ToLocalChecked(),
                           FunctionTemplate::New(isolate, RemoveEventListener));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "dispatchEvent").ToLocalChecked(),
                           FunctionTemplate::New(isolate, DispatchEvent));

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nodeName").ToLocalChecked(),
                                    NodeNameGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nodeType").ToLocalChecked(),
                                    NodeTypeGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nodeValue").ToLocalChecked(),
                                    NodeValueGetter,
                                    NodeValueSetter);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "parentNode").ToLocalChecked(),
                                    ParentNodeGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "firstChild").ToLocalChecked(),
                                    FirstChildGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "lastChild").ToLocalChecked(),
                                    LastChildGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nextSibling").ToLocalChecked(),
                                    NextSiblingGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "previousSibling").ToLocalChecked(),
                                    PreviousSiblingGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "ownerDocument").ToLocalChecked(),
                                    OwnerDocumentGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "textContent").ToLocalChecked(),
                                    TextContentGetter,
                                    TextContentSetter);

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "appendChild").ToLocalChecked(),
                            FunctionTemplate::New(isolate, AppendChild));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "removeChild").ToLocalChecked(),
                            FunctionTemplate::New(isolate, RemoveChild));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "insertBefore").ToLocalChecked(),
                            FunctionTemplate::New(isolate, InsertBefore));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "replaceChild").ToLocalChecked(),
                            FunctionTemplate::New(isolate, ReplaceChild));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "cloneNode").ToLocalChecked(),
                            FunctionTemplate::New(isolate, CloneNode));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "hasChildNodes").ToLocalChecked(),
                            FunctionTemplate::New(isolate, HasChildNodes));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "contains").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Contains));
    }

    // static
    Local<Object> Node::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Node> nativeNode)
    {
      EscapableHandleScope scope(isolate);

      if (nativeNode == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      // TODO: Create appropriate subclass instances based on node type
      // For now, just create a basic Node wrapper
      return scope.Escape(scripting_base::ObjectWrap<Node, ::dom::Node>::NewInstance(isolate, nativeNode).As<Object>());
    }

    // static
    Local<Function> Node::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<Node, ::dom::Node>::Initialize(isolate);
    }

    Node::Node(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<Node, ::dom::Node>(isolate, args)
    {
    }

    // Property getters

    // static
    void Node::NodeNameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetUndefined();
        return;
      }

      string nodeName = node->inner()->nodeName;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, nodeName.c_str()).ToLocalChecked());
    }

    // static
    void Node::NodeTypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetUndefined();
        return;
      }

      int nodeType = static_cast<int>(node->inner()->nodeType);
      info.GetReturnValue().Set(Integer::New(isolate, nodeType));
    }

    // static
    void Node::NodeValueGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      string nodeValue = node->inner()->nodeValue;
      if (nodeValue.empty())
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, nodeValue.c_str()).ToLocalChecked());
      }
    }

    // static
    void Node::NodeValueSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      if (value->IsNull() || value->IsUndefined())
      {
        node->inner()->nodeValue = "";
      }
      else
      {
        String::Utf8Value utf8Value(isolate, value);
        node->inner()->nodeValue = string(*utf8Value);
      }
    }

    // static
    void Node::ParentNodeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto parentNode = node->inner()->parentNode.lock();
      if (parentNode == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> parentWrapper = Node::NewInstance(isolate, parentNode);
        info.GetReturnValue().Set(parentWrapper);
      }
    }

    // static
    void Node::FirstChildGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto firstChild = node->inner()->firstChild;
      if (firstChild == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> childWrapper = Node::NewInstance(isolate, firstChild);
        info.GetReturnValue().Set(childWrapper);
      }
    }

    // static
    void Node::LastChildGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto lastChild = node->inner()->lastChild;
      if (lastChild == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> childWrapper = Node::NewInstance(isolate, lastChild);
        info.GetReturnValue().Set(childWrapper);
      }
    }

    // static
    void Node::NextSiblingGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto nextSibling = node->inner()->nextSibling.lock();
      if (nextSibling == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> siblingWrapper = Node::NewInstance(isolate, nextSibling);
        info.GetReturnValue().Set(siblingWrapper);
      }
    }

    // static
    void Node::PreviousSiblingGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto previousSibling = node->inner()->previousSibling.lock();
      if (previousSibling == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> siblingWrapper = Node::NewInstance(isolate, previousSibling);
        info.GetReturnValue().Set(siblingWrapper);
      }
    }

    // static
    void Node::OwnerDocumentGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto ownerDocument = node->inner()->ownerDocument.lock();
      if (ownerDocument == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        // TODO: Create proper Document wrapper
        Local<Object> documentWrapper = Node::NewInstance(isolate, ownerDocument);
        info.GetReturnValue().Set(documentWrapper);
      }
    }

    // static
    void Node::TextContentGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      string textContent = node->inner()->textContent();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, textContent.c_str()).ToLocalChecked());
    }

    // static
    void Node::TextContentSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      node->inner()->setTextContent(string(*utf8Value));
    }

    // Methods

    // static
    void Node::AppendChild(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "appendChild requires 1 argument").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      if (!info[0]->IsObject())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
        return;
      }

      Node *childNode = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(Local<Object>::Cast(info[0]));
      if (childNode == nullptr || childNode->inner() == nullptr)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
        return;
      }

      auto result = node->inner()->appendChild(childNode->inner());
      if (result != nullptr)
      {
        Local<Object> resultWrapper = Node::NewInstance(isolate, result);
        info.GetReturnValue().Set(resultWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    // static
    void Node::RemoveChild(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "removeChild requires 1 argument").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      if (!info[0]->IsObject())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
        return;
      }

      Node *childNode = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(Local<Object>::Cast(info[0]));
      if (childNode == nullptr || childNode->inner() == nullptr)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
        return;
      }

      auto result = node->inner()->removeChild(childNode->inner());
      if (result != nullptr)
      {
        Local<Object> resultWrapper = Node::NewInstance(isolate, result);
        info.GetReturnValue().Set(resultWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    // static
    void Node::InsertBefore(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "insertBefore requires 2 arguments").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement insertBefore
      cout << "insertBefore called" << endl;
      info.GetReturnValue().Set(info[0]);
    }

    // static
    void Node::ReplaceChild(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "replaceChild requires 2 arguments").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement replaceChild
      cout << "replaceChild called" << endl;
      info.GetReturnValue().Set(info[1]);
    }

    // static
    void Node::CloneNode(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      bool deep = false;
      if (info.Length() > 0 && info[0]->IsBoolean())
      {
        deep = info[0]->BooleanValue(isolate);
      }

      // TODO: Implement cloneNode
      cout << "cloneNode called with deep=" << deep << endl;
      info.GetReturnValue().Set(info.This());
    }

    // static
    void Node::HasChildNodes(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      bool hasChildren = node->inner()->hasChildNodes();
      info.GetReturnValue().Set(Boolean::New(isolate, hasChildren));
    }

    // static
    void Node::Contains(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      if (!info[0]->IsObject())
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      Node *otherNode = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(Local<Object>::Cast(info[0]));
      if (otherNode == nullptr || otherNode->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      bool contains = node->inner()->contains(otherNode->inner());
      info.GetReturnValue().Set(Boolean::New(isolate, contains));
    }

    // EventTarget methods

    // static
    void Node::AddEventListener(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "addEventListener requires at least 2 arguments").ToLocalChecked()));
        return;
      }

      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be a string").ToLocalChecked()));
        return;
      }

      if (!info[1]->IsFunction())
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Second argument must be a function").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value eventType(isolate, info[0]);
      Local<Function> listener = Local<Function>::Cast(info[1]);

      // For now, store the listener. In a full implementation, this would register
      // with the DOM event system
      // TODO: Implement actual event listener registration with the DOM backend
      cout << "addEventListener called for event: " << *eventType << endl;
    }

    // static
    void Node::RemoveEventListener(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "removeEventListener requires at least 2 arguments").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value eventType(isolate, info[0]);
      // TODO: Implement actual event listener removal
      cout << "removeEventListener called for event: " << *eventType << endl;
    }

    // static
    void Node::DispatchEvent(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "dispatchEvent requires 1 argument").ToLocalChecked()));
        return;
      }

      if (!info[0]->IsObject())
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be an Event object").ToLocalChecked()));
        return;
      }

      Node *node = scripting_base::ObjectWrap<Node, ::dom::Node>::Unwrap(info.This());
      if (node == nullptr || node->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement actual event dispatching
      cout << "dispatchEvent called" << endl;
      info.GetReturnValue().Set(Boolean::New(isolate, true));
    }
  }
}