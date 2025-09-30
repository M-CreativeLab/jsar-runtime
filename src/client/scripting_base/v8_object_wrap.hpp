#pragma once

#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>

#include <node/v8.h>
#include <node/node_api.h>
#include <common/utility.hpp>

#include "./v8_object_holder.hpp"
#include "./v8_object_wrap_base.hpp"

namespace scripting_base
{
  template <typename T, typename TCallback>
  struct MethodCallbackData
  {
    TCallback callback;
    void *data;
  };

  template <typename T, typename TGetterCallback, typename TSetterCallback>
  struct AccessorCallbackData
  {
    TGetterCallback getterCallback;
    TSetterCallback setterCallback;
    void *data;
  };

  constexpr int CONSTRUCTING_CONTEXT_FLAG = 0xFE32;

  /**
   * A struct to represent the context to construct the object.
   */
  struct ConstructingContext
  {
    enum : uint8_t
    {
      kScript = 0xf,
      kNative,
    };

    int flag;
    uint8_t source = kScript;

    ConstructingContext(uint8_t source)
        : flag(CONSTRUCTING_CONTEXT_FLAG)
        , source(source)
    {
    }

    bool isValid() const
    {
      return flag == CONSTRUCTING_CONTEXT_FLAG;
    }
    bool isNativeCall() const
    {
      return isValid() && source == kNative;
    }
    bool isScriptCall() const
    {
      return isValid() && source == kScript;
    }
  };

  /**
   * A base class for wrapping C++ objects in v8::Object instances.
   *
   * This class is useful for wrapping C++ objects when the N-API's ObjectWrap is not available such as in the case of
   * using the V8 API directly.
   *
   * @tparam T The class to wrap.
   * @tparam D The type of the optional inner instance.
   * @tparam B The base class of T, if T is derived from another ObjectWrap class.
   */
  template <typename T, typename D = void, typename B = void>
  class ObjectWrap : public ObjectWrapBase
  {
    friend class ObjectWrap<T, D, B>;

  protected:
    using InstanceMethodCallback = void (T::*)(const v8::FunctionCallbackInfo<v8::Value> &);
    using InstanceMethodCallbackData = MethodCallbackData<T, InstanceMethodCallback>;

    using InstanceGetterCallback = void (T::*)(const v8::PropertyCallbackInfo<v8::Value> &);
    using InstanceSetterCallback = void (T::*)(v8::Local<v8::Value>, const v8::PropertyCallbackInfo<void> &);
    using InstanceAccessorCallbackData = AccessorCallbackData<T, InstanceGetterCallback, InstanceSetterCallback>;

  public:
    /**
     * The name of the `ObjectWrap` class.
     *
     * @returns The name of the class.
     */
    static std::string Name()
    {
      return "Object";
    }

    static v8::Local<v8::FunctionTemplate> GetFunctionTemplate(v8::Isolate *isolate)
    {
      v8::EscapableHandleScope scope(isolate);
      return scope.Escape(function_template_.Get(isolate));
    }

    static v8::Local<v8::Function> GetConstructorFunction(v8::Isolate *isolate)
    {
      v8::EscapableHandleScope scope(isolate);
      return scope.Escape(constructor_handle_.Get(isolate));
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
     * @brief Checks if a given V8 value is an instance of the class associated with this wrapper.
     *
     * This function verifies whether the provided V8 value is an object and whether it is an
     * instance of the constructor function associated with this class. It uses the V8 API to
     * perform the type check.
     *
     * @param isolate A pointer to the V8 isolate in which the check is performed.
     * @param value The V8 value to be checked.
     * @return true if the value is an object and an instance of the associated class, false otherwise.
     */
    static bool IsInstanceOf(v8::Isolate *isolate, v8::Local<v8::Value> value)
    {
      if (!value->IsObject())
        return false;

      v8::Local<v8::Object> obj = value.As<v8::Object>();
      v8::Local<v8::Function> constructor = GetConstructorFunction(isolate);
      if (constructor.IsEmpty())
        return false;
      return obj->InstanceOf(isolate->GetCurrentContext(), constructor).FromMaybe(false);
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
    static v8::Local<v8::Object> NewInstance(napi_env napiEnv, std::shared_ptr<D> inner = nullptr)
    {
      if constexpr (!std::is_same_v<D, void>)
      {
        assert(inner != nullptr && "inner must not be null when D is not void");
      }

      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::EscapableHandleScope scope(isolate);

      v8::Local<v8::Object> jsThis = NewInstance(isolate, inner);
      T *instance = T::Unwrap(isolate, jsThis);
      instance->setNapiEnv(napiEnv);
      return scope.Escape(jsThis);
    }

    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @param isolate The v8::Isolate instance
     * @param inner The optional inner instance of the class D
     * @returns The wrapped v8::Object
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<D> inner)
    {
      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Function> constructor = constructor_handle_.Get(isolate);
      if (constructor.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Constructor is not initialized for " << T::Name() << "()" << std::endl;
        return scope.Escape(v8::Local<v8::Object>());
      }

      std::vector<v8::Local<v8::Value>> args;
      v8::Local<v8::External> option = v8::External::New(isolate,
                                                         new ConstructingContext(ConstructingContext::kNative));
      args.push_back(option);
      v8::Local<v8::Object> jsThis = constructor->NewInstance(context,
                                                              args.size(),
                                                              args.data())
                                       .ToLocalChecked();
      if (jsThis.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Failed to create new instance of " << T::Name() << "()" << std::endl;
        return scope.Escape(v8::Local<v8::Object>());
      }

      // Set the inner handle if D is specified and handle is not null
      if constexpr (!std::is_same_v<D, void>)
      {
        if (inner != nullptr)
        {
          // Update the inner reference
          T *instance = T::Unwrap(isolate, jsThis);
          instance->setInner(inner);
        }
      }

      // Return the created instance
      return scope.Escape(jsThis);
    }

    /**
     * Get the instance object.
     */
    static v8::Local<v8::Object> GetOrNewInstance(v8::Isolate *isolate, std::shared_ptr<D> inner)
    {
      if constexpr (!std::is_base_of_v<JSObjectHolder, D>)
      {
        assert(false && "inner type must inherit from `JSObjectHolder`");
      }

      assert(inner != nullptr && "inner must not be null");
      if (inner->hasJSObject())
      {
        v8::EscapableHandleScope scope(isolate);
        ObjectWrapBase *objectWrap = inner->getJSObjectWrap();
        return scope.Escape(objectWrap->getJSObject(isolate));
      }
      else
      {
        return T::NewInstance(isolate, inner);
      }
    }

    static void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> object, T *instance)
    {
      assert(isolate != nullptr && "isolate must not be null");
      assert(!object.IsEmpty() && "object must not be null");
      assert(instance != nullptr && "instance must not be null");

      v8::HandleScope scope(isolate);
      object->SetInternalField(0, v8::External::New(isolate, instance));

      instance->object_handle_.Reset(isolate, object);
      instance->object_handle_.SetWeak(instance, Finalizer, v8::WeakCallbackType::kParameter);
    }

    /**
     * Unwrap the v8::Object to get the instance of the class T
     *
     * @param object The v8::Object to unwrap
     * @returns The instance of the class T
     */
    static T *Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> object)
    {
      v8::HandleScope scope(isolate);
      auto externalValue = object->GetInternalField(0);
      if (externalValue.IsEmpty())
        return nullptr;
      return static_cast<T *>(externalValue.As<v8::External>()->Value());
    }

    /**
     * Initialize the class and return the constructor function.
     *
     * @param isolate The v8::Isolate instance
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate)
    {
      assert(isolate != nullptr);

      // Return the existing constructor if already initialized
      if (T::initialized_ == true)
        return T::GetConstructorFunction(isolate);

      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, T::Constructor);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, T::Name().c_str()).ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Set up inheritance if `B` is specified
      if constexpr (!std::is_same_v<B, void>)
      {
        // Ensure base class is initialized first
        B::Initialize(isolate);
        v8::Local<v8::FunctionTemplate> base = B::GetFunctionTemplate(isolate);
        if (!base.IsEmpty())
        {
          tpl->Inherit(base);
        }
      }

      T::ConfigureFunctionTemplate(isolate, tpl);

      // Update the persistent handles
      v8::Local<v8::Function> constructor = tpl->GetFunction(context).ToLocalChecked();
      T::constructor_handle_.Reset(isolate, constructor);
      T::function_template_.Reset(isolate, tpl);
      T::initialized_ = true;
      return scope.Escape(constructor);
    }

  protected:
    /**
     * Sets a read-only instance property with the specified name and value on a V8 ObjectTemplate.
     *
     * @param isolate         The V8 isolate in which to create the property.
     * @param objectTemplate  The V8 ObjectTemplate to which the property will be added.
     * @param name            The name of the property to set.
     * @param value           The value to assign to the property.
     *
     * This function adds a read-only property with the given name and value to the provided
     * ObjectTemplate, making it available on instances created from this template.
     */
    template <typename V>
    static void IntegerConstant(v8::Isolate *isolate,
                                v8::Local<v8::FunctionTemplate> tpl,
                                const char *name,
                                V value)
    {
      v8::HandleScope scope(isolate);
      v8::Local<v8::String> nameString = v8::String::NewFromUtf8(isolate, name).ToLocalChecked();
      v8::Local<v8::Number> valueNumber = v8::Integer::New(isolate, static_cast<int>(value));
      v8::PropertyAttribute attributes = static_cast<v8::PropertyAttribute>(v8::PropertyAttribute::ReadOnly |
                                                                            v8::PropertyAttribute::DontDelete);
      tpl->Set(nameString, valueNumber, attributes);
      tpl->PrototypeTemplate()->Set(nameString, valueNumber, attributes);
    }
    /**
     * Create a standardized method callback for instance methods.
     * 
     * @param isolate The v8::Isolate instance.
     * @param objectTemplate The object template to which the method will be added.
     * @param name The name of the method.
     * @param callback The method callback function.
     */
    static void InstanceMethod(v8::Isolate *isolate,
                               v8::Local<v8::ObjectTemplate> objectTemplate,
                               const char *name,
                               InstanceMethodCallback callback)
    {
      v8::HandleScope scope(isolate);

      InstanceMethodCallbackData *callbackData = new InstanceMethodCallbackData{callback, nullptr};
      v8::Local<v8::External> dataValue = v8::External::New(isolate, callbackData);

      // Manage the lifetime of the callback data
      auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceMethodCallbackData> &data)
      {
        InstanceMethodCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };
      handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
      T::callback_data_handles_.emplace_back(std::move(handle));

      objectTemplate->Set(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                          v8::FunctionTemplate::New(isolate, MethodWrapper, dataValue));
    }
    /**
     * Create a standardized accessor callback for instance properties.
     */
    static void InstanceAccessor(v8::Isolate *isolate,
                                 v8::Local<v8::ObjectTemplate> objectTemplate,
                                 const char *name,
                                 InstanceGetterCallback getter,
                                 InstanceSetterCallback setter,
                                 v8::PropertyAttribute attributes = v8::PropertyAttribute::None)
    {
      auto callbackData = new InstanceAccessorCallbackData{getter, setter, nullptr};
      v8::Local<v8::External> dataValue = v8::External::New(isolate, callbackData);

      // Manage the lifetime of the callback data
      auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceAccessorCallbackData> &data)
      {
        InstanceAccessorCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };
      handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
      T::callback_data_handles_.emplace_back(std::move(handle));

      // Add the accessor to the object template
      objectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                                  GetterWrapper,
                                  SetterWrapper,
                                  dataValue,
                                  v8::AccessControl::DEFAULT,
                                  attributes);
    }
    static void InstanceReadonlyAccessor(v8::Isolate *isolate,
                                         v8::Local<v8::ObjectTemplate> objectTemplate,
                                         const char *name,
                                         InstanceGetterCallback getter,
                                         v8::PropertyAttribute attributes = v8::PropertyAttribute::ReadOnly)
    {
      auto callbackData = new InstanceAccessorCallbackData{getter, nullptr, nullptr};
      v8::Local<v8::External> dataValue = v8::External::New(isolate, callbackData);

      // Manage the lifetime of the callback data
      auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceAccessorCallbackData> &data)
      {
        InstanceAccessorCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };
      handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
      T::callback_data_handles_.emplace_back(std::move(handle));

      // Add the accessor to the object template
      objectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                                  GetterWrapper,
                                  nullptr,
                                  dataValue,
                                  v8::AccessControl::DEFAULT,
                                  attributes);
    }
    /**
     * Create a standardized error message for method failures.
     *
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param message The error message describing the failure.
     * @returns A formatted error message string.
     */
    static v8::Local<v8::String> ErrorMessage(v8::Isolate *isolate, const char *method, const char *message)
    {
      std::string str = "Failed to execute '" + std::string(method) + "' on '" + T::Name() + "': " +
                        std::string(message);
      return v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
    }
    /**
     * Create a standardized error message for method failures.
     * 
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param message The error message describing the failure.
     * @returns A formatted error message string.
     */
    static v8::Local<v8::String> MakeMethodError(v8::Isolate *isolate, const char *method, const char *message)
    {
      std::string str = "Failed to execute '" + std::string(method) + "' on '" + T::Name() + "': " +
                        std::string(message);
      return v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
    }
    /**
     * Create a standardized error message for constructor failures.
     * 
     * @param isolate The v8::Isolate instance.
     * @param message The error message describing the failure.
     * @returns A formatted error message string.
     */
    static v8::Local<v8::String> MakeConstructorError(v8::Isolate *isolate, const char *message)
    {
      std::string str = "Failed to construct '" + T::Name() + "': " + std::string(message);
      return v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
    }

  private:
    template <typename TCallbackData>
    static inline TCallbackData *GetCallbackData(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      return GetCallbackData<TCallbackData>(info.GetIsolate(), info.Data());
    }
    template <typename TCallbackData>
    static inline TCallbackData *GetCallbackData(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      return GetCallbackData<TCallbackData>(info.GetIsolate(), info.Data());
    }
    template <typename TCallbackData>
    static inline TCallbackData *GetCallbackData(const v8::PropertyCallbackInfo<void> &info)
    {
      return GetCallbackData<TCallbackData>(info.GetIsolate(), info.Data());
    }
    template <typename TCallbackData>
    static TCallbackData *GetCallbackData(v8::Isolate *isolate, v8::Local<v8::Value> data)
    {
      v8::HandleScope scope(isolate);
      assert(data->IsExternal() && "data must be an External");

      auto external = data.As<v8::External>();
      assert(external->Value() != nullptr && "callback data must not be null");

      TCallbackData *callbackData = static_cast<TCallbackData *>(external->Value());
      if (callbackData == nullptr) [[unlikely]]
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
        return nullptr;
      }
      return callbackData;
    }

    static void MethodWrapper(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceMethodCallbackData>(info);
      if (callbackData != nullptr)
      {
        assert(callbackData->callback != nullptr && "callback must not be null");

        T *instance = T::Unwrap(isolate, info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the method callback
        std::bind(callbackData->callback, instance, std::placeholders::_1)(info);
        return;
      }
    }

    static void GetterWrapper(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceAccessorCallbackData>(info);
      if (callbackData != nullptr && callbackData->getterCallback) [[likely]]
      {
        T *instance = T::Unwrap(info.GetIsolate(), info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the getter callback
        std::bind(callbackData->getterCallback,
                  instance,
                  std::placeholders::_1)(info);
        return;
      }

      // No getter defined, return undefined
      info.GetReturnValue().Set(v8::Undefined(isolate));
    }

    static void SetterWrapper(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceAccessorCallbackData>(info);
      if (callbackData != nullptr && callbackData->setterCallback) [[likely]]
      {
        T *instance = T::Unwrap(info.GetIsolate(), info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the setter callback
        std::bind(callbackData->setterCallback,
                  instance,
                  std::placeholders::_1,
                  std::placeholders::_2)(value, info);
        return;
      }
    }

  public:
    ObjectWrap(v8::Isolate *isolate)
        : ObjectWrapBase(isolate)
    {
    }
    /**
     * Constructor for ObjectWrap.
     * 
     * @param isolate The v8::Isolate instance.
     * @param args The function callback info containing the arguments.
     * @param nativeConstructingOnly If true, only allow native construction (not from script).
     */
    ObjectWrap(v8::Isolate *isolate,
               const v8::FunctionCallbackInfo<v8::Value> &args,
               bool nativeConstructingOnly = false)
        : ObjectWrapBase(isolate)
    {
      v8::HandleScope scope(isolate);
      if (!args.IsConstructCall())
      {
        isolate->ThrowException(v8::Exception::TypeError(MakeConstructorError(isolate,
                                                                              "Illegal constructor")));
        return;
      }

      ConstructingContext *constructingContext = nullptr;

      // Get the constructing context from the first argument if it is an `External` object
      if (args.Length() == 1 && args[0]->IsExternal())
      {
        v8::Local<v8::External> firstArg = args[0].As<v8::External>();
        assert(firstArg->Value() != nullptr && "option must not be null");
        constructingContext = static_cast<ConstructingContext *>(firstArg->Value());
      }

      // If `nativeConstructingOnly` is true, only allow native construction
      if (nativeConstructingOnly == true && (constructingContext == nullptr ||
                                             constructingContext->isScriptCall())) [[unlikely]]
      {
        isolate->ThrowException(v8::Exception::TypeError(MakeConstructorError(isolate,
                                                                              "Illegal constructor")));
      }

      // Delete the constructing context if it was created
      if (constructingContext != nullptr)
        delete constructingContext;
    }

    virtual ~ObjectWrap()
    {
    }

  public:
    void setInner(std::shared_ptr<D> data)
    {
      inner_handle_ = data;
      onDataSet(data);

      // Set the weak reference back to this object if D is derived from `JSObjectHolder`.
      if constexpr (std::is_base_of_v<JSObjectHolder, D>)
      {
        if (inner_handle_ != nullptr)
          inner_handle_->setReference(this);
      }
    }

    /**
     * @returns The `ObjectWrap`'s V8 value.
     */
    v8::Local<v8::Value> value() const
    {
      return object_handle_.Get(current_isolate_);
    }
    /**
     * @returns The `ObjectWrap`'s inner handle.
     */
    inline std::shared_ptr<D> inner() const
    {
      return inner_handle_;
    }
    /**
     * @returns The `ObjectWrap`'s inner handle.
     */
    inline std::shared_ptr<D> handle() const
    {
      return inner_handle_;
    }

  protected:
    virtual void onDataSet(std::shared_ptr<D> data)
    {
    }

  private:
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      v8::Isolate *isolate = args.GetIsolate();
      v8::HandleScope scope(isolate);

      if (!args.IsConstructCall())
      {
        std::cerr << "Illegal constructor call for " << T::Name() << std::endl;
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8(isolate, "Illegal constructor").ToLocalChecked()));
        return;
      }

      T *instance = new T(isolate, args);
      assert(instance != nullptr && "Failed to create instance");

      auto jsObject = args.This();
      Wrap(isolate, jsObject, instance);

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
    // Inner is a weak pointer to the optional inner instance of the class D.
    std::shared_ptr<D> inner_handle_;

  private:
    static thread_local inline bool initialized_ = false;
    static thread_local inline v8::Persistent<v8::FunctionTemplate> function_template_;
    static thread_local inline v8::Persistent<v8::Function> constructor_handle_;
    static thread_local inline std::vector<std::unique_ptr<v8::Persistent<v8::External>>> callback_data_handles_{};
  };
}
