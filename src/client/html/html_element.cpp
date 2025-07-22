#include <client/cssom/units.hpp>
#include <client/dom/document-inl.hpp>
#include <client/dom/attr.hpp>
#include <client/dom/browsing_context.hpp>

#include "./html_element.hpp"

namespace dom
{
  using namespace std;

  string DashStyleToCamelCase(const string &dashStyle)
  {
    string camelCase;
    bool upper = false;
    for (auto c : dashStyle)
    {
      if (c == '-')
      {
        upper = true;
      }
      else
      {
        if (upper)
        {
          camelCase += toupper(c);
          upper = false;
        }
        else
        {
          camelCase += c;
        }
      }
    }
    return camelCase;
  }

  string CamelCaseToDashStyle(const string &camelCase)
  {
    string dashStyle;
    for (auto c : camelCase)
    {
      if (isupper(c))
      {
        if (!dashStyle.empty())
          dashStyle += '-';
        dashStyle += tolower(c);
      }
      else
      {
        dashStyle += c;
      }
    }
    return dashStyle;
  }

  // TODO: Implement the following methods.
  void HTMLElement::blur()
  {
  }
  void HTMLElement::focus()
  {
  }
  void HTMLElement::click()
  {
    // TODO(yorkie): support disabled state.
    simulateClick(glm::vec3(0.0f, 0.0f, 0.0f));
  }

  optional<string> HTMLElement::getDataset(const string &key)
  {
    auto it = dataset_.find(key);
    if (it != dataset_.end())
      return it->second;
    return nullopt;
  }

  HTMLElement::HTMLElement(const HTMLElement &other)
      : Element(other)
      , dir(other.dir)
      , draggable(other.draggable)
      , hidden(other.hidden)
      , innerText(other.innerText)
      , lang(other.lang)
      , nonce(other.nonce)
      , outerText(other.outerText)
      , title(other.title)
      , offset_width_(other.offset_width_)
      , offset_height_(other.offset_height_)
      , dataset_(other.dataset_)
      , style_(other.style_)
      // Don't copy the resource-loading related fields.
      , load_async_handle_()
      , load_requests_mutex_()
      , load_requests_()
  {
  }

  void HTMLElement::setDataset(const string &key, const string &value)
  {
    dataset_[key] = value;
    setAttribute("data-" + CamelCaseToDashStyle(key), value);
  }

  void HTMLElement::removeDataset(const string &key)
  {
    dataset_.erase(key);
    removeAttribute("data-" + CamelCaseToDashStyle(key));
  }

  void HTMLElement::fetchResource(const string &url, function<void(const void *data, size_t length)> callback)
  {
    assert(ownerDocument->expired() == false && "The owner document is expired.");
    auto browsingContext = ownerDocument->lock()->browsingContext;
    browsingContext->fetchArrayBufferLikeResource(url, callback);
  }

  void HTMLElement::fetchResourceThreadSafe(const string &url, function<void(const void *data, size_t length)> callback)
  {
    assert(load_async_handle_.data == this &&
           "The async handle data is not set to this element.");

    unique_lock<shared_mutex> lock(load_requests_mutex_);
    // Schedule the image loading on the scripting thread.
    load_requests_.emplace_back(url, std::move(callback));
    uv_async_send(&load_async_handle_);
  }

  void HTMLElement::createdCallback(bool from_scripting)
  {
    Element::createdCallback(from_scripting);

    // Update dataset from the attributes.
    for (auto &attr : attributeNodes_)
    {
      if (attr.first.size() > 5 && attr.first.substr(0, 5) == "data-")
      {
        string key = DashStyleToCamelCase(attr.first.substr(5));
        string value = attr.second->value;
        dataset_[key] = value;
      }
    }

    // Configure the style property change callback.
    auto onPropertyChanged = [this](const string &name)
    {
      markAsDirty();
    };
    // Create style declaration from the default style & the style attribute.
    style_ = make_shared<client_cssom::CSSStyleDeclaration>(getAttribute("style"));
    style_->setPropertyChangedCallback(onPropertyChanged);
  }

  void HTMLElement::attributeChangedCallback(const std::string &name,
                                             const std::string &oldValue,
                                             const std::string &newValue)
  {
    Element::attributeChangedCallback(name, oldValue, newValue);

    // Update the style property if the attribute is changed.
    if (name == "style")
    {
      // Update the style property.
      style_ = make_shared<client_cssom::CSSStyleDeclaration>(newValue);
    }

    // Update the dataset if the attribute is changed.
    if (name.substr(0, 5) == "data-")
    {
      string key = DashStyleToCamelCase(name.substr(5));
      if (newValue.empty())
      {
        dataset_.erase(key);
      }
      else
      {
        dataset_[key] = newValue;
      }
    }
  }

  void HTMLElement::connectedCallback()
  {
    Element::connectedCallback();

    load_async_handle_.data = this;
    auto handle_load_requests = [](uv_async_t *handle)
    {
      auto element = static_cast<HTMLElement *>(handle->data);
      assert(element != nullptr && "The async handle data is not set to this element.");
      element->processLoadRequests();
    };
    uv_async_init(TrClientContextPerProcess::Get()->getScriptingEventLoop(),
                  &load_async_handle_,
                  handle_load_requests);
  }

  void HTMLElement::disconnectedCallback()
  {
    Element::disconnectedCallback();

    // Clear the load requests when the element is disconnected.
    unique_lock<shared_mutex> lock(load_requests_mutex_);
    load_requests_.clear();

    // Reset the async handle data to avoid dangling pointer.
    load_async_handle_.data = nullptr;
    uv_close(reinterpret_cast<uv_handle_t *>(&load_async_handle_), nullptr);
  }

  void HTMLElement::markAsDirty()
  {
    Element::markAsDirty();
    invalidateStyleCache();
  }

  void HTMLElement::invalidateStyleCache()
  {
    auto document = getOwnerDocumentReference();
    if (document != nullptr)
      document->styleCache().resetStyle(getPtr<HTMLElement>());
  }

  void HTMLElement::processLoadRequests()
  {
    assert(ownerDocument->expired() == false && "The owner document is expired.");
    auto browsingContext = ownerDocument->lock()->browsingContext;

    unique_lock<shared_mutex> lock(load_requests_mutex_);
    while (!load_requests_.empty())
    {
      LoadResourceRequest request = load_requests_.front();
      load_requests_.pop_front();

      // If the URL is empty, skip the request.
      if (request.url.empty())
        continue;
      browsingContext->fetchArrayBufferLikeResource(request.url, request.call);
    }
  }
}
