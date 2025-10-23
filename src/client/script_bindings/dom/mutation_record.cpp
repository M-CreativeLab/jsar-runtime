#include <iostream>
#include "./mutation_record.hpp"
#include "./node.hpp"
#include "./node_list.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings::dom_bindings
  {
    void MutationRecord::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->InstanceTemplate();

      // Add read-only properties
      InstanceReadonlyPropertyAccessor(isolate, prototype, "type", &MutationRecord::TypeGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "target", &MutationRecord::TargetGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "addedNodes", &MutationRecord::AddedNodesGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "removedNodes", &MutationRecord::RemovedNodesGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "previousSibling", &MutationRecord::PreviousSiblingGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "nextSibling", &MutationRecord::NextSiblingGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "attributeName", &MutationRecord::AttributeNameGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "attributeNamespace", &MutationRecord::AttributeNamespaceGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "oldValue", &MutationRecord::OldValueGetter);
    }

    MutationRecord::MutationRecord(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : MutationRecordBase(isolate, args)
    {
      // MutationRecord constructor
    }

    // Property getters
    void MutationRecord::TypeGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      switch (handle()->type)
      {
      case dom::MutationType::Attributes:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "attributes").ToLocalChecked());
        break;
      case dom::MutationType::CharacterData:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "characterData").ToLocalChecked());
        break;
      case dom::MutationType::ChildList:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "childList").ToLocalChecked());
        break;
      default:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "unknown").ToLocalChecked());
        break;
      }
    }

    void MutationRecord::TargetGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (handle()->hasTarget())
      {
        args.GetReturnValue().Set(Node::GetOrNewInstance(isolate,
                                                         handle()->getTarget()));
      }
      else
      {
        args.GetReturnValue().SetNull();
      }
    }

    void MutationRecord::AddedNodesGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      auto nodes = make_unique<dom::NodeList<dom::Node>>(handle()->addedNodes);
      args.GetReturnValue().Set(NodeList::NewInstance(isolate, move(nodes)));
    }

    void MutationRecord::RemovedNodesGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      auto nodes = make_unique<dom::NodeList<dom::Node>>(handle()->removedNodes);
      args.GetReturnValue().Set(NodeList::NewInstance(isolate, move(nodes)));
    }

    void MutationRecord::PreviousSiblingGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (handle()->previousSibling.expired())
      {
        args.GetReturnValue().SetNull();
      }
      else
      {
        args.GetReturnValue().Set(Node::GetOrNewInstance(isolate,
                                                         handle()->previousSibling.lock()));
      }
    }

    void MutationRecord::NextSiblingGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (handle()->nextSibling.expired())
      {
        args.GetReturnValue().SetNull();
      }
      else
      {
        args.GetReturnValue().Set(Node::GetOrNewInstance(isolate,
                                                         handle()->nextSibling.lock()));
      }
    }

    void MutationRecord::AttributeNameGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (handle()->attributeName.has_value())
      {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->attributeName->c_str())
                                    .ToLocalChecked());
      }
      else
      {
        args.GetReturnValue().SetNull();
      }
    }

    void MutationRecord::AttributeNamespaceGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (handle()->attributeNamespace.has_value())
      {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->attributeNamespace->c_str())
                                    .ToLocalChecked());
      }
      else
      {
        args.GetReturnValue().SetNull();
      }
    }

    void MutationRecord::OldValueGetter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (handle()->oldValue.has_value())
      {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->oldValue->c_str())
                                    .ToLocalChecked());
      }
      else
      {
        args.GetReturnValue().SetNull();
      }
    }
  }
} // namespace endor
