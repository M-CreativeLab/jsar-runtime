#pragma once

#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>

#include <node/v8.h>
#include <node/node_api.h>
#include <common/utility.hpp>

namespace scripting_base
{
  /**
   * A base class for wrapping C++ objects in v8::Object instances.
   *
   * This class is useful for wrapping C++ objects when the N-API's ObjectWrap is not available such as in the case of
   * using the V8 API directly.
   *
   * @tparam T The class to wrap.
   * @tparam D The type of the optional inner instance.
   */
  template <typename T, typename D = void>
  class ObjectWrap
  {
    friend class ObjectWrap<T, D>;

  public:
    /**
     * The name of the `ObjectWrap` class.
     *
     * @returns The name of the class.
     */
    static std::string Name()
    {
      return "ObjectWrap";
    }

    static v8::Local<v8::Function> ConstructorFunction(v8::Isolate *isolate)
    {
      return constructor_handle_.Get(isolate);
    }

    /**
     * Configure the function template, this method is called when the class is being initialized, the subclass can
     * override this method to configure the function template.
     *
     * @param isolate The v8::Isolate instance
     * @param tpl The v8::FunctionTemplate instance
     */
    static void ConfigureFunctionTemplate(v8::Isolate *, v8::Local<v8::FunctionTemplate>)
    {
      // Override this method in the derived class to configure the function template
    }

    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @tparam Args The types of the arguments to pass to the constructor of T
     *
     * @param isolate The v8::Isolate instance
     * @param inner The optional inner instance of the class D
     * @returns The wrapped v8::Object
     */
    static v8::Local<v8::Value> NewInstance(napi_env napiEnv, std::shared_ptr<D> inner = nullptr)
    {
      if constexpr (!std::is_same_v<D, void>)
      {
        assert(inner != nullptr && "inner must not be null when D is not void");
      }

      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Function> constructor = constructor_handle_.Get(isolate);
      if (TR_UNLIKELY(constructor.IsEmpty()))
        return scope.Escape(v8::Local<v8::Value>());

      std::vector<v8::Local<v8::Value>> args;
      if (inner != nullptr)
      {
        SharedReference<D> innerSharedRef(inner);
        v8::Local<v8::External> innerExternal = v8::External::New(isolate, &innerSharedRef);
        args.push_back(innerExternal);
      }

      v8::Local<v8::Object> jsThis = constructor->NewInstance(context, args.size(), args.data()).ToLocalChecked();
      if (TR_UNLIKELY(jsThis.IsEmpty()))
        return scope.Escape(v8::Local<v8::Value>());

      T *instance = Unwrap(jsThis);
      instance->setNapiEnv(napiEnv);
      return scope.Escape(jsThis);
    }

    static void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> object, T *instance)
    {
      assert(isolate != nullptr && "isolate must not be null");
      assert(!object.IsEmpty() && "object must not be null");
      assert(instance != nullptr && "instance must not be null");

      v8::HandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      object->SetInternalField(0, v8::External::New(isolate, instance));
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
     * Initialize the class and return the constructor function.
     *
     * @param isolate The v8::Isolate instance
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate)
    {
      assert(isolate != nullptr);

      v8::HandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, T::Name().c_str()).ToLocalChecked());
      tpl->SetCallHandler(ObjectWrap<T, D>::CallHandler);
      tpl->InstanceTemplate()->Set(isolate, "constructor", tpl, v8::PropertyAttribute::ReadOnly);
      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      T::ConfigureFunctionTemplate(isolate, tpl);

      v8::Local<v8::Function> constructor = tpl->GetFunction(context).ToLocalChecked();
      ObjectWrap<T, D>::constructor_handle_.Reset(isolate, constructor);
      return constructor;
    }

  public:
    ObjectWrap(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args,
               std::shared_ptr<D> inner = nullptr)
        : current_isolate_(isolate),
          inner_handle_(inner) {};

    virtual ~ObjectWrap()
    {
    }

  public:
    void setNapiEnv(napi_env env) { napi_env_ = env; }

    v8::Local<v8::Value> value() const { return object_handle_.Get(current_isolate_); }
    std::shared_ptr<D> inner() const { return inner_handle_.lock(); }

  private:
    static void CallHandler(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      v8::Isolate *isolate = args.GetIsolate();

      if (!args.IsConstructCall())
      {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal constructor").ToLocalChecked()));
        return;
      }

      T *instance = nullptr;
      if constexpr (!std::is_same_v<D, void>)
      {
        if (args.Length() < 1 || !args[0]->IsExternal())
        {
          isolate->ThrowException(v8::Exception::TypeError(
              v8::String::NewFromUtf8(isolate, "Illegal constructor").ToLocalChecked()));
          return;
        }

        v8::Local<v8::External> innerExternal = v8::Local<v8::External>::Cast(args[0]);
        SharedReference<D> *innerSharedRef = static_cast<SharedReference<D> *>(innerExternal->Value());
        assert(innerSharedRef != nullptr && "innerSharedRef must not be null");
        instance = new T(isolate, args, innerSharedRef->value);
      }
      else
      {
        instance = new T(isolate, args);
      }

      auto jsObject = args.This();
      Wrap(isolate, jsObject, instance);

      instance->object_handle_.Reset(isolate, jsObject);
      instance->object_handle_.SetWeak(instance, Finalizer, v8::WeakCallbackType::kParameter);

      args.GetReturnValue().Set(jsObject);
    }

    static void Finalizer(const v8::WeakCallbackInfo<T> &data)
    {
      T *instance = data.GetParameter();
      if (instance != nullptr)
        instance->object_handle_.Reset();
      data.SetSecondPassCallback(Cleanup);
    }

    static void Cleanup(const v8::WeakCallbackInfo<T> &data)
    {
      T *instance = data.GetParameter();
      if (instance != nullptr)
        delete instance;
    }

  protected:
    napi_env napi_env_;
    v8::Isolate *current_isolate_;
    v8::Persistent<v8::Object> object_handle_;

    // Inner is a weak pointer to the optional inner instance of the class D.
    std::weak_ptr<D> inner_handle_;

  private:
    static thread_local inline v8::Persistent<v8::Function> constructor_handle_;
  };
}
