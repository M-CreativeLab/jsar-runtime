#pragma once

#include <memory>
#include <optional>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/scripting_base/v8_object_holder.hpp>

namespace endor
{
  namespace script_bindings::dom_bindings
  {
    /**
     * An abstract interface representing a source of key-value string pairs for DOMStringMap.
     */
    class DOMStringMapSource : public scripting_base::JSObjectHolder
    {
    public:
      virtual ~DOMStringMapSource() = default;

    public:
      virtual std::optional<std::string> get(const std::string &key) const = 0;
      virtual void set(const std::string &key, const std::string &value) = 0;
      virtual void unset(const std::string &key) = 0;
    };

    class DOMStringMap;
    using DOMStringMapBase = scripting_base::ObjectWrap<DOMStringMap, DOMStringMapSource>;

    class DOMStringMap : public DOMStringMapBase
    {
      using DOMStringMapBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "DOMStringMap";
      }
      static bool NativeConstructorRequired()
      {
        return true;
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
      {
        v8::Local<v8::ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

        // Set up named property handlers for dynamic access to dataset attributes
        instanceTemplate->SetHandler(v8::NamedPropertyHandlerConfiguration(NamedPropertyGetter,
                                                                           NamedPropertySetter,
                                                                           nullptr,
                                                                           NamedPropertyDeleter,
                                                                           nullptr));
      }

      template <typename D>
      requires std::is_base_of_v<DOMStringMapSource, D>
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<D> source)
      {
        assert(source != nullptr && "source should not be null");
        v8::EscapableHandleScope scope(isolate);

        auto base_source = std::dynamic_pointer_cast<DOMStringMapSource>(source);
        if (!base_source)
        {
          isolate->ThrowException(v8::Exception::TypeError(MakeMethodError(isolate,
                                                                           "new",
                                                                           "Invalid source type")));
          return v8::Local<v8::Object>();
        }
        else
        {
          v8::Local<v8::Object> instance = DOMStringMapBase::NewInstance(isolate, base_source);
          return scope.Escape(instance);
        }
      }

    public:
      DOMStringMap(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
          : DOMStringMapBase(isolate, args)
      {
      }

    private:
      static void NamedPropertyGetter(v8::Local<v8::Name> property,
                                      const v8::PropertyCallbackInfo<v8::Value> &info)
      {
        v8::Isolate *isolate = info.GetIsolate();
        v8::HandleScope scope(isolate);

        v8::String::Utf8Value property_utf8(isolate, property);

        auto instance = Unwrap(isolate, info.Holder());
        if (!instance)
        {
          info.GetReturnValue().SetUndefined();
          return;
        }

        std::optional<std::string> value = instance->handle()->get(*property_utf8);
        if (value.has_value())
          info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, value->c_str()).ToLocalChecked());
        else
          info.GetReturnValue().SetUndefined();
      }

      static void NamedPropertySetter(v8::Local<v8::Name> property,
                                      v8::Local<v8::Value> value,
                                      const v8::PropertyCallbackInfo<v8::Value> &info)
      {
        v8::Isolate *isolate = info.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> context = isolate->GetCurrentContext();

        auto instance = Unwrap(isolate, info.Holder());
        if (!instance)
        {
          isolate->ThrowException(v8::Exception::TypeError(
            MakeMethodError(isolate, "set", "Instance is null")));
          return;
        }

        v8::String::Utf8Value key_utf8(isolate, property);
        std::string key(*key_utf8);

        v8::MaybeLocal<v8::String> maybe_str = value->ToString(context);
        if (maybe_str.IsEmpty())
        {
          isolate->ThrowException(v8::Exception::TypeError(
            MakeMethodError(isolate, "set", "Value cannot be converted to string")));
          return;
        }
        v8::Local<v8::String> str = maybe_str.ToLocalChecked();
        v8::String::Utf8Value value_utf8(isolate, str);
        std::string value_str(*value_utf8);

        instance->handle()->set(key, value_str);
        info.GetReturnValue().Set(value);
      }

      static void NamedPropertyDeleter(v8::Local<v8::Name> property,
                                       const v8::PropertyCallbackInfo<v8::Boolean> &info)
      {
        v8::Isolate *isolate = info.GetIsolate();
        v8::HandleScope scope(isolate);

        v8::String::Utf8Value property_utf8(isolate, property);

        auto instance = Unwrap(isolate, info.Holder());
        if (!instance)
        {
          info.GetReturnValue().Set(false);
          return;
        }

        instance->handle()->unset(*property_utf8);
        info.GetReturnValue().Set(true);
      }
    };
  }
} // namespace endor
