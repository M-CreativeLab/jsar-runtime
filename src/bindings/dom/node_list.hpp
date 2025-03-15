#pragma once

#include <node/v8.h>
#include <node/node_api.h>
#include <napi.h>

#include <client/dom/node_list.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/scripting_base/iterator_protocol_impl.hpp>

namespace dombinding
{
  template <typename TNode>
  concept is_node_type = std::is_same_v<dom::Node, TNode> || std::is_base_of_v<dom::Node, TNode>;

  template <typename TNode>
    requires is_node_type<TNode>
  class ArrayIterator : public scripting_base::Iterable<ArrayIterator<TNode>, TNode>
  {
  public:
    ArrayIterator(v8::Isolate *isolate, napi_env napiEnv, const dom::NodeList<TNode> &list)
        : scripting_base::Iterable<ArrayIterator<TNode>, TNode>(isolate, list),
          napiEnv_(napiEnv)
    {
      assert(napiEnv_ != nullptr && "napi_env must not be null");
    }

  public:
    v8::Local<v8::Value> createNextValue(v8::Isolate *isolate, const std::shared_ptr<TNode> value) override;

  private:
    napi_env napiEnv_;
  };

  /**
   * A wrapper around the `NodeList` class.
   *
   * @tparam TNode The type of the node implementation that must be a subclass of `dom::Node`
   */
  template <typename TNode>
    requires is_node_type<TNode>
  class NodeList final : public scripting_base::ObjectWrap<NodeList<TNode>>
  {
    using T = NodeList<TNode>;

  public:
    static Napi::Value NewInstance(Napi::Env env, dom::NodeList<TNode> &list);
    static inline T *Unwrap(v8::Local<v8::Object> object)
    {
      return scripting_base::ObjectWrap<NodeList<TNode>>::Unwrap(object);
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

  public:
    /**
     * Creates a new instance of the NodeList class.
     *
     * @param isolate The V8 isolate
     * @param napiEnv The N-API environment that is used to using N-API based functions
     * @param list The NodeList instance
     */
    NodeList(v8::Isolate *isolate, napi_env napiEnv, dom::NodeList<TNode> &list)
        : scripting_base::ObjectWrap<T>(isolate, "NodeList"),
          napiEnv_(napiEnv),
          list_(list)
    {
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
      return scripting_base::ObjectWrap<ArrayIterator<TNode>>::NewInstance(this->currentIsolate_,
                                                                           napiEnv_,
                                                                           list_);
    }

  private:
    napi_env napiEnv_;
    dom::NodeList<TNode> list_;
  };
}
