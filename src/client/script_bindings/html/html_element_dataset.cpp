#include "./html_element_dataset.hpp"

namespace endor
{
  namespace script_bindings::html_bindings
  {
    using namespace std;
    using namespace v8;

    Local<Object> HTMLElementDataset::NewInstance(Isolate *isolate, shared_ptr<::endor::dom::HTMLElement> element)
    {
      EscapableHandleScope scope(isolate);
      Local<Object> datasetValue = dom_bindings::DOMStringMap::NewInstance(isolate,
                                                                           make_shared<HTMLElementDataset>(element));
      return scope.Escape(datasetValue);
    }

    HTMLElementDataset::HTMLElementDataset(std::shared_ptr<::endor::dom::HTMLElement> element)
        : data_source_(element)
    {
    }

    optional<string> HTMLElementDataset::get(const string &key) const
    {
      auto htmlElement = data_source_.lock();
      return htmlElement != nullptr
               ? htmlElement->getDataset(key)
               : nullopt;
    }

    void HTMLElementDataset::set(const string &key, const string &value)
    {
      auto htmlElement = data_source_.lock();
      if (htmlElement != nullptr)
        htmlElement->setDataset(key, value);
    }

    void HTMLElementDataset::unset(const string &key)
    {
      auto htmlElement = data_source_.lock();
      if (htmlElement != nullptr)
        htmlElement->removeDataset(key);
    }
  }
} // namespace endor
