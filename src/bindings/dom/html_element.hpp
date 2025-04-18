#pragma once

#include <memory>
#include <napi.h>
#include <client/dom/html_element.hpp>

#include "./element-inl.hpp"
#include "./dom_string_map.hpp"

namespace dombinding
{
  class HTMLElementDataset final : public DOMStringMap
  {
  public:
    static inline Napi::Value NewInstance(Napi::Env env, std::shared_ptr<dom::HTMLElement> htmlElement)
    {
      return DOMStringMap::NewInstance(env, new HTMLElementDataset(htmlElement));
    }

  public:
    HTMLElementDataset(std::shared_ptr<dom::HTMLElement> htmlElement)
        : dataSource(htmlElement)
    {
    }

  public:
    std::optional<std::string> get(const std::string &key) const override
    {
      auto htmlElement = dataSource.lock();
      return htmlElement != nullptr
                 ? htmlElement->getDataset(key)
                 : std::nullopt;
    }
    void set(const std::string &key, const std::string &value) override
    {
      auto htmlElement = dataSource.lock();
      if (htmlElement != nullptr)
        htmlElement->setDataset(key, value);
    }
    void unset(const std::string &key) override
    {
      auto htmlElement = dataSource.lock();
      if (htmlElement != nullptr)
        htmlElement->removeDataset(key);
    }

  public:
    std::weak_ptr<dom::HTMLElement> dataSource;
  };

  template <typename ObjectType, typename HTMLElementType>
  class HTMLElementBase : public ElementBase<ObjectType, HTMLElementType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env);

  public:
    using ElementBase<ObjectType, HTMLElementType>::ElementBase;

  public:
    Napi::Value DatasetGetter(const Napi::CallbackInfo &info);
    Napi::Value StyleGetter(const Napi::CallbackInfo &info);
  };

  class HTMLElement : public HTMLElementBase<HTMLElement, dom::HTMLElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
