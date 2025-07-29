#pragma once

#include <memory>
#include <napi.h>
#include "client/browser/window.hpp"

namespace browserbinding
{
  /**
   * JavaScript binding for Navigator object.
   * Provides access to browser/environment information.
   */
  class Navigator : public Napi::ObjectWrap<Navigator>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<browser::Window> window);

  public:
    Navigator(const Napi::CallbackInfo &info);
    ~Navigator() = default;

  public:
    Napi::Value OnLineGetter(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
    std::weak_ptr<browser::Window> window_;
  };
} // namespace browserbinding