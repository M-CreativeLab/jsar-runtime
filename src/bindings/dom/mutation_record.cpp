#include "./mutation_record.hpp"
#include "./node-inl.hpp"
#include "./node_list-inl.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *MutationRecord::constructor = nullptr;
  void MutationRecord::Init(Napi::Env env)
  {
#define MODULE_NAME "MutationRecord"
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    (*constructor).SuppressDestruct();

    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  Napi::Object MutationRecord::MakeFromImpl(Napi::Env env, const dom::MutationRecord &record)
  {
    Napi::EscapableHandleScope scope(env);

    dom::MutationRecord recordNew = record;
    Napi::External<dom::MutationRecord> external = Napi::External<dom::MutationRecord>::New(env, &recordNew);
    return scope.Escape(constructor->New({external})).ToObject();
  }

  MutationRecord::MutationRecord(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<MutationRecord>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    dom::MutationRecord *record = info[0].As<Napi::External<dom::MutationRecord>>().Data();
    if (record == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto jsThis = info.This().ToObject();
    switch (record->type)
    {
    case dom::MutationType::Attributes:
      jsThis.Set("type", Napi::String::New(env, "attributes"));
      break;
    case dom::MutationType::CharacterData:
      jsThis.Set("type", Napi::String::New(env, "characterData"));
      break;
    case dom::MutationType::ChildList:
      jsThis.Set("type", Napi::String::New(env, "childList"));
      break;
    default:
      jsThis.Set("type", Napi::String::New(env, "unknown"));
      break;
    }

    auto targetNode = record->target.lock();
    if (targetNode != nullptr)
      jsThis.Set("target", Node::NewInstance(env, targetNode));

    if (!record->addedNodes.empty())
    {
      auto nodes = make_unique<dom::NodeList<dom::Node>>(record->addedNodes);
      jsThis.Set("addedNodes", NodeList::NewInstance(env, move(nodes)));
    }
    else
      jsThis.Set("addedNodes", env.Null());

    if (!record->removedNodes.empty())
    {
      auto nodes = make_unique<dom::NodeList<dom::Node>>(record->removedNodes);
      jsThis.Set("removedNodes", NodeList::NewInstance(env, move(nodes)));
    }
    else
      jsThis.Set("removedNodes", env.Null());

    if (record->attributeName.has_value())
      jsThis.Set("attributeName", Napi::String::New(env, record->attributeName.value()));
    if (record->attributeNamespace.has_value())
      jsThis.Set("attributeNamespace", Napi::String::New(env, record->attributeNamespace.value()));
    if (record->oldValue.has_value())
      jsThis.Set("oldValue", Napi::String::New(env, record->oldValue.value()));

    // TODO: support other fields?
  }
}
