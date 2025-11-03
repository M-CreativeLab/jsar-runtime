#pragma once

#include <memory>
#include <client/html/html_element.hpp>
#include <client/script_bindings/dom/dom_string_map.hpp>

namespace endor
{
  namespace script_bindings::html_bindings
  {
    class HTMLElementDataset : public dom_bindings::DOMStringMapSource
    {
    public:
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::endor::dom::HTMLElement> element);

    public:
      HTMLElementDataset(std::shared_ptr<::endor::dom::HTMLElement> element);

    public:
      std::optional<std::string> get(const std::string &key) const override;
      void set(const std::string &key, const std::string &value) override;
      void unset(const std::string &key) override;

    private:
      std::weak_ptr<::endor::dom::HTMLElement> data_source_;
    };
  }
} // namespace endor
