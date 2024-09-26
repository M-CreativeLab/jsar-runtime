#pragma once

#include <napi.h>
#include "common/utility.hpp"
#include "./node.hpp"

namespace dombinding
{
  template <typename ObjectType, typename NodeType>
  NodeBase<ObjectType, NodeType>::NodeBase(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ObjectType>(info),
        jsThreadId(std::this_thread::get_id())
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsExternal())
    {
      NodeContainer<NodeType> *container = info[0].As<Napi::External<NodeContainer<NodeType>>>().Data();
      ResetNode(info, container->node);
    }
    else
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    Napi::Function callbackFn = Napi::Function::New(env, &NodeBase<ObjectType, NodeType>::OnEventListenerCallback);
    listenerCallback = Napi::Persistent(callbackFn);

    Napi::Object callbackObject = callbackFn.ToObject();
    Napi::Function onEventListenerCallback = callbackObject
                                                 .Get("bind")
                                                 .As<Napi::Function>()
                                                 .Call(callbackObject, {info.This()})
                                                 .As<Napi::Function>();
    threadSafeListenerCallback = Napi::ThreadSafeFunction::New(env, onEventListenerCallback, "EventListenerCallback", 0, 2);
  }

  template <typename ObjectType, typename NodeType>
  NodeBase<ObjectType, NodeType>::~NodeBase()
  {
    listenerCallback.Reset();
    threadSafeListenerCallback.Release();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::IsConnectedGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->connected);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::ChildNodesGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto childNodes = node->getChildNodes();
    Napi::Array jsChildNodes = Napi::Array::New(env, childNodes.size());
    for (size_t i = 0; i < childNodes.size(); i++)
      jsChildNodes.Set(i, Node::NewInstance(env, childNodes[i]));
    return jsChildNodes;
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::FirstChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    shared_ptr<dom::Node> firstChildNode = node->getFirstChild();
    if (firstChildNode != nullptr)
      return Node::NewInstance(env, firstChildNode);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::LastChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    shared_ptr<dom::Node> lastChildNode = node->getLastChild();
    if (lastChildNode != nullptr)
      return Node::NewInstance(env, lastChildNode);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::TextContentGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (node->nodeType == dom::NodeType::DOCUMENT_NODE || node->nodeType == dom::NodeType::DOCUMENT_TYPE_NODE)
      return env.Null();
    else
      return Napi::String::New(env, node->getTextContent());
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::AppendChild(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::CloneNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::CompareDocumentPosition(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::Contains(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::GetRootNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::HasChildNodes(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::InsertBefore(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::AddEventListener(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "2 arguments required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto typeString = info[0].ToString().Utf8Value();
    auto listenerValue = info[1];

    if (!listenerValue.IsFunction())
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "The listener provided is not a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    dom::DOMEventType eventType;
    try
    {
      eventType = dom::StringToEventType(typeString);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsThisRef = make_shared<Napi::ObjectReference>(Napi::Persistent(info.This().As<Napi::Object>()));
    auto listenerRef = make_shared<Napi::FunctionReference>(Napi::Persistent(listenerValue.As<Napi::Function>()));
    auto listenerCallback = [this, jsThisRef, listenerRef](dom::DOMEventType type, dom::Event &event)
    {
      if (this->jsThreadId != std::this_thread::get_id())
      {
        /**
         * When the caller thread is not the same as the JavaScript thread, we need to call tsfn.
         */
        shared_ptr<dom::Event> eventRef = make_shared<dom::Event>(event);
        threadSafeListenerCallback.NonBlockingCall(
            [eventRef, listenerRef](Napi::Env env, Napi::Function jsCallback)
            {
              Napi::HandleScope scope(env);
              jsCallback.Call({Napi::Number::New(env, static_cast<int>(eventRef->type)),
                               listenerRef->Value()});
            });
        return;
      }
      else
      {
        /**
         * When the caller thread is the same as the JavaScript thread, we can call the listener directly.
         */
        Napi::Env env = jsThisRef->Env();
        Napi::HandleScope scope(env);

        assert(!this->listenerCallback.IsEmpty());
        this->listenerCallback.Call(jsThisRef->Value(), {Napi::Number::New(env, static_cast<int>(type)),
                                                         listenerRef->Value()});
      }
    };

    auto nativeListener = node->addEventListener(eventType, listenerCallback);
    listenerRefToNativeIdMap.insert({listenerRef, nativeListener->id});
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::RemoveEventListener(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      auto msg = "Failed to execute 'removeEventListener' on 'EventTarget': "
                 "2 arguments required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto typeString = info[0].ToString().Utf8Value();
    auto listenerValue = info[1];

    if (!listenerValue.IsFunction())
    {
      auto msg = "Failed to execute 'removeEventListener' on 'EventTarget': "
                 "The listener provided is not a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    dom::DOMEventType eventType;
    try
    {
      eventType = dom::StringToEventType(typeString);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    for (auto it = listenerRefToNativeIdMap.begin(); it != listenerRefToNativeIdMap.end();)
    {
      auto listenerRef = it->first;
      if (listenerRef->Value() == listenerValue)
      {
        uint32_t listenerId = it->second;
        node->removeEventListener(eventType, listenerId);
        it = listenerRefToNativeIdMap.erase(it);
        break;
      }
      else
      {
        ++it;
      }
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::DispatchEvent(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': "
                 "1 argument required, but only 0 present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto eventObject = info[0].ToObject();
    if (!eventObject.InstanceOf(env.Global().Get("Event").As<Napi::Function>()))
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': "
                 "parameter 1 is not of type 'Event'.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto eventTypeString = eventObject.Get("type").ToString().Utf8Value();
    dom::DOMEventType eventType;
    try
    {
      eventType = dom::StringToEventType(eventTypeString);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    /**
     * Dispatch the event
     */
    node->dispatchEvent(eventType, nullptr);

    /**
     * Call `on${eventType}` if it is a function
     */
    auto jsThis = info.This().As<Napi::Object>();
    auto onEventName = "on" + ToLowerCase(eventTypeString);
    if (jsThis.Has(onEventName))
    {
      auto onEventValue = jsThis.Get(onEventName);
      if (onEventValue.IsFunction())
        onEventValue.As<Napi::Function>().Call(jsThis, {eventObject});
    }
    return Napi::Boolean::New(env, true);
  }

  template <typename ObjectType, typename NodeType>
  void NodeBase<ObjectType, NodeType>::ResetNode(const Napi::CallbackInfo &info, shared_ptr<NodeType> nodeToSet)
  {
    node = nodeToSet;
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      // Update the static properties which are not be changed
      auto jsThis = info.This().As<Napi::Object>();
      jsThis.Set("baseURI", Napi::String::New(env, node->baseURI));
      jsThis.Set("nodeName", Napi::String::New(env, node->nodeName));
      jsThis.Set("isConnected", Napi::Boolean::New(env, node->connected));
      jsThis.Set("nodeType", Napi::Number::New(env, static_cast<int>(node->nodeType)));
    }
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::OnEventListenerCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function eventConstructor;
    if (env.Global().Get("Event").IsFunction())
      eventConstructor = env.Global().Get("Event").As<Napi::Function>();

    if (eventConstructor.IsEmpty())
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "The event constructor is not found.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 2)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "1 argument required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsFunction())
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "The listener provided is not a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    string eventTypeStr;
    try
    {
      dom::DOMEventType type = static_cast<dom::DOMEventType>(info[0].ToNumber().Int32Value());
      eventTypeStr = dom::EventTypeToString(type);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object eventObject = eventConstructor.New({Napi::String::New(env, eventTypeStr)});
    info[1].As<Napi::Function>().Call(info.This(), {eventObject});
    return env.Undefined();
  }
}
