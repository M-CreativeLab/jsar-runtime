#include "document_metadata_wrap.hpp"

using namespace gom;

Napi::FunctionReference *DocumentMetadataWrap::constructor;
void DocumentMetadataWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(env, "DocumentMetadata",
                                   {
                                       InstanceAccessor<&DocumentMetadataWrap::GetSpecVersion, &DocumentMetadataWrap::SetSpecVersion>("specVersion"),
                                       InstanceAccessor<&DocumentMetadataWrap::GetDescription, &DocumentMetadataWrap::SetDescription>("description"),
                                       InstanceAccessor<&DocumentMetadataWrap::GetAuthor, &DocumentMetadataWrap::SetAuthor>("author"),
                                       InstanceAccessor<&DocumentMetadataWrap::GetViewportInitialScale, &DocumentMetadataWrap::SetViewportInitialScale>("viewportInitialScale"),
                                   });

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  env.SetInstanceData(constructor);
  exports.Set("DocumentMetadata", tpl);
}

DocumentMetadataWrap::DocumentMetadataWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<DocumentMetadataWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  this->native_handle_ = new DocumentMetadata();
}

Napi::Value DocumentMetadataWrap::GetSpecVersion(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  std::string specVersion = this->native_handle_->spec_version();
  if (specVersion.empty())
    return env.Null();

  return Napi::String::New(env, specVersion);
}

void DocumentMetadataWrap::SetSpecVersion(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsString())
    return;

  Napi::String specVersion = value.ToString();
  this->native_handle_->set_spec_version(specVersion.Utf8Value());
}

Napi::Value DocumentMetadataWrap::GetDescription(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  std::string description = this->native_handle_->description();
  if (description.empty())
    return env.Null();

  return Napi::String::New(env, description);
}

void DocumentMetadataWrap::SetDescription(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsString())
    return;

  Napi::String description = value.ToString();
  this->native_handle_->set_description(description.Utf8Value());
}

Napi::Value DocumentMetadataWrap::GetAuthor(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  std::string author = this->native_handle_->author();
  if (author.empty())
    return env.Null();

  return Napi::String::New(env, author);
}

void DocumentMetadataWrap::SetAuthor(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsString())
    return;

  Napi::String author = value.ToString();
  this->native_handle_->set_author(author.Utf8Value());
}

Napi::Value DocumentMetadataWrap::GetViewportInitialScale(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  return Napi::Number::New(env, this->native_handle_->viewport_initial_scale());
}

void DocumentMetadataWrap::SetViewportInitialScale(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsNumber())
    return;
  this->native_handle_->set_viewport_initial_scale(value.ToNumber().FloatValue());
}

DocumentMetadata *DocumentMetadataWrap::getNativeHandle()
{
  return this->native_handle_;
}
