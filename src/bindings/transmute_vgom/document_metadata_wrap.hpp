#pragma once

#include <napi.h>

#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
  class DocumentMetadataWrap : public Napi::ObjectWrap<DocumentMetadataWrap>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    DocumentMetadataWrap(const Napi::CallbackInfo &info);
    DocumentMetadata *getNativeHandle();

  public:
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value GetSpecVersion(const Napi::CallbackInfo &info);
    void SetSpecVersion(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetDescription(const Napi::CallbackInfo &info);
    void SetDescription(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetAuthor(const Napi::CallbackInfo &info);
    void SetAuthor(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetViewportInitialScale(const Napi::CallbackInfo &info);
    void SetViewportInitialScale(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    DocumentMetadata *native_handle_;
  };
} // namespace gom
