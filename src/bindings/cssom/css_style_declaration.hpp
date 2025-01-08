#pragma once

#include <memory>
#include <napi.h>
#include <common/utility.hpp>
#include <client/cssom/css_style_declaration.hpp>

namespace cssombinding
{
  class CSSStyleDeclaration : public Napi::ObjectWrap<CSSStyleDeclaration>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_cssom::CSSStyleDeclaration> handle);

  public:
    CSSStyleDeclaration(const Napi::CallbackInfo &info);

  private: // Accessors
    Napi::Value CssTextGetter(const Napi::CallbackInfo &info);
    void CssTextSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LengthGetter(const Napi::CallbackInfo &info);

  private: // Methods
    Napi::Value GetPropertyPriority(const Napi::CallbackInfo &info);
    Napi::Value GetPropertyValue(const Napi::CallbackInfo &info);
    Napi::Value Item(const Napi::CallbackInfo &info);
    void RemoveProperty(const Napi::CallbackInfo &info);
    void SetProperty(const Napi::CallbackInfo &info);
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    /**
     * Use the handle if it is not expired.
     *
     * @tparam R The return type of the function.
     * @param fn The function to use the handle.
     * @param defaultValue The default value to return if the handle is expired.
     * @returns The result of the function.
     */
    template <typename R>
    R useHandle(const std::function<R(client_cssom::CSSStyleDeclaration &)> &fn, const R &defaultValue)
    {
      auto handle = handle_.lock();
      if (handle == nullptr)
        return defaultValue;
      return fn(*handle);
    }
    /**
     * Use the handle without return value if it is not expired.
     *
     * @param fn The function to use the handle.
     */
    void useHandle(const std::function<void(client_cssom::CSSStyleDeclaration &)> &fn)
    {
      auto handle = handle_.lock();
      if (handle == nullptr)
        return;
      fn(*handle);
    }

  private:
    std::weak_ptr<client_cssom::CSSStyleDeclaration> handle_;

  private:
    inline static thread_local Napi::FunctionReference *constructor = nullptr;
  };
}
