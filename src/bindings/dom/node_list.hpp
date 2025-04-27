#pragma once

#include <memory>
#include <node/v8.h>
#include <node/node_api.h>
#include <napi.h>

#include <client/dom/node_list.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/scripting_base/iterator_protocol_impl.hpp>

namespace dombinding
{
  /**
   * A wrapper around the `NodeListApi` class.
   */
  class NodeList final : public scripting_base::ObjectWrap<NodeList, dom::NodeListApi>
  {
    using Base = scripting_base::ObjectWrap<NodeList, dom::NodeListApi>;

  private:
    class NodeListIterator : public scripting_base::Iterable<NodeListIterator, dom::Node>
    {
      using scripting_base::Iterable<NodeListIterator, dom::Node>::Iterable;

    public:
      v8::Local<v8::Value> createNextValue(v8::Isolate *isolate, const std::shared_ptr<dom::Node> value) override;
    };

  public:
    static std::string Name()
    {
      return "NodeList";
    }

    static void Init(Napi::Env);
    static Napi::Value NewInstance(Napi::Env, std::shared_ptr<dom::NodeListApi>);
    static NodeList *Unwrap(v8::Local<v8::Object> object) { return Base::Unwrap(object); }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

  public:
    NodeList(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &info,
             std::shared_ptr<dom::NodeListApi> list)
        : scripting_base::ObjectWrap<NodeList, dom::NodeListApi>(isolate, info, list)
    {
      if (!list->isLive())
        static_list_ = list; // add the reference to the static list to keep it alive
    }

  private:
    static void PropertyGetter(unsigned int index, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PropertySetter(unsigned int index, v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PropertyDeleter(unsigned int index, const v8::PropertyCallbackInfo<v8::Boolean> &info);
    static void PropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);
    static void LengthGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void LengthSetter(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Values(const v8::FunctionCallbackInfo<v8::Value> &info);

  public:
    v8::Local<v8::Value> get(unsigned int index) const;

  private:
    // Returns the values of the NodeList that follows the iterator protocol.
    inline v8::Local<v8::Value> values() const
    {
      return NodeListIterator::NewInstance(napi_env_, *this->inner());
    }

    inline bool hasList() const
    {
      auto list = this->inner_handle_.lock();
      return list != nullptr;
    }

    inline dom::NodeListApi &listRef() const
    {
      auto list = this->inner_handle_.lock();
      assert(list != nullptr && "list must not be null.");
      return *list;
    }

  private:
    std::shared_ptr<dom::NodeListApi> static_list_ = nullptr;
  };
}
