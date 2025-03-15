#pragma once

#include <memory>
#include <vector>
#include <assert.h>
#include <node/v8.h>
#include <iostream>

namespace scripting_base
{
  /**
   * A base class for wrapping C++ objects in v8::Object instances.
   *
   * This class is useful for wrapping C++ objects when the N-API's ObjectWrap is not available such as in the case of
   * using the V8 API directly.
   *
   * @tparam T The class to wrap
   */
  template <typename T>
  class ObjectWrap
  {
  public:
    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @tparam Args The types of the arguments to pass to the constructor of T
     *
     * @param isolate The v8::Isolate instance
     * @param args The arguments to pass to the constructor of T
     * @returns The wrapped v8::Object
     */
    template <typename... Args>
    static v8::Local<v8::Value> NewInstance(v8::Isolate *isolate, Args &&...args)
    {
      v8::EscapableHandleScope scope(isolate);
      auto instance = new T(isolate, std::forward<Args>(args)...); // This will be freed by its finalizer
      return scope.Escape(instance->value());
    }

    /**
     * Unwrap the v8::Object to get the instance of the class T
     *
     * @param object The v8::Object to unwrap
     * @returns The instance of the class T
     */
    static T *Unwrap(v8::Local<v8::Object> object)
    {
      auto externalValue = object->GetInternalField(0);
      if (externalValue.IsEmpty())
        return nullptr;
      return static_cast<T *>(v8::Local<v8::External>::Cast(externalValue)->Value());
    }

    /**
     * Configure the function template, this method is called when the class is being initialized, the subclass can
     * override this method to configure the function template.
     *
     * @param isolate The v8::Isolate instance
     * @param tpl The v8::FunctionTemplate instance
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      // Override this method in the derived class to configure the function template
    }

  public:
    ObjectWrap(v8::Isolate *isolate, std::string name = "Object")
        : currentIsolate_(isolate)
    {
      assert(this->currentIsolate_ != nullptr);

      v8::HandleScope scope(isolate);
      v8::Local<v8::Context> context = currentIsolate_->GetCurrentContext();

      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(currentIsolate_);
      tpl->SetClassName(v8::String::NewFromUtf8(currentIsolate_, name.c_str()).ToLocalChecked());
      tpl->InstanceTemplate()->Set(currentIsolate_, "constructor", tpl, v8::PropertyAttribute::ReadOnly);
      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      T::ConfigureFunctionTemplate(isolate, tpl);

      v8::Local<v8::Function> constructor = tpl->GetFunction(context).ToLocalChecked();
      v8::Local<v8::Object> jsObject = constructor->NewInstance(context).ToLocalChecked();
      jsObject->SetInternalField(0, v8::External::New(currentIsolate_, this));

      auto finalizer = [](const auto &info)
      {
        auto wrapper = info.GetParameter();
        if (wrapper != nullptr)
          delete wrapper;
      };
      persistentHandle_.Reset(currentIsolate_, jsObject);
      persistentHandle_.SetWeak(this, finalizer, v8::WeakCallbackType::kParameter);
    };

    virtual ~ObjectWrap()
    {
      persistentHandle_.Reset();
    }

  public:
    /**
     * Get the v8::Object instance.
     *
     * @returns The v8::Object instance
     */
    v8::Local<v8::Value> value() const
    {
      return persistentHandle_.Get(currentIsolate_);
    }

  protected:
    v8::Isolate *currentIsolate_;
    v8::Persistent<v8::Object> persistentHandle_;
  };
}
