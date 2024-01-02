
#include "buffer_writter.hpp"
#include "document_metadata_wrap.hpp"
#include "go_property_change_wrap.hpp"
#include "go_material_sync_change_wrap.hpp"
#include "go_wrap.hpp"
#include "gom_wrap.hpp"

using namespace gom;

void VirtualGameObjectModelWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "VirtualGameObjectModel",
      {InstanceMethod("setTitle", &VirtualGameObjectModelWrap::SetTitle),
       InstanceMethod("setMetadata", &VirtualGameObjectModelWrap::SetMetadata),
       InstanceMethod("createGameObjectAsChild", &VirtualGameObjectModelWrap::CreateGameObjectAsChild),
       InstanceMethod("createPropertyChange", &VirtualGameObjectModelWrap::CreatePropertyChange),
       InstanceMethod("createVerticesSyncChange", &VirtualGameObjectModelWrap::CreateVerticesSyncChange),
       InstanceMethod("createMaterialSyncChange", &VirtualGameObjectModelWrap::CreateMaterialSyncChange),
       InstanceMethod("serializeAndWrite", &VirtualGameObjectModelWrap::SerializeAndWrite),
       InstanceMethod("isBufferEmpty", &VirtualGameObjectModelWrap::IsBufferEmpty),
       InstanceMethod("fetchBufferFromWritter", &VirtualGameObjectModelWrap::FetchBufferFromWritter),
       InstanceMethod("fetchAndClearBufferFromWritter", &VirtualGameObjectModelWrap::FetchAndClearBufferFromWritter)});

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  env.SetInstanceData(constructor);

  exports.Set("VirtualGameObjectModel", tpl);
}

VirtualGameObjectModelWrap::VirtualGameObjectModelWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualGameObjectModelWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  this->native_handle_ = new VirtualGameObjectModel();
}

VirtualGameObjectModelWrap::~VirtualGameObjectModelWrap()
{
  if (this->native_handle_ != nullptr)
    delete this->native_handle_;
}

Napi::Value VirtualGameObjectModelWrap::SetTitle(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "Title is expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::String title = info[0].ToString();
  this->native_handle_->set_title(title.Utf8Value());
  return env.Null();
}

Napi::Value VirtualGameObjectModelWrap::SetMetadata(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  Napi::Value metadataValue = info[0];

  if (metadataValue.IsObject() && metadataValue.As<Napi::Object>().InstanceOf(DocumentMetadataWrap::constructor->Value()))
  {
    DocumentMetadataWrap *metadataWrap = Napi::ObjectWrap<DocumentMetadataWrap>::Unwrap(metadataValue.As<Napi::Object>());
    auto metadata = metadataWrap->getNativeHandle();
    this->native_handle_->set_allocated_metadata(metadata);
  }
  return env.Null();
}

Napi::Value VirtualGameObjectModelWrap::CreateGameObjectAsChild(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "Guid is expected").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 2 || !info[1].IsObject())
  {
    Napi::TypeError::New(env, "GameObject is expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::String guid = info[0].ToString();
  Napi::Object gameObject = info[1].ToObject();
  // if (gameObject.Get("id").ToString().Utf8Value() == "__root__")
  // {
  //     // FIXME: Skip to create GameObject when the node id is "__root__"
  //     return env.Null();
  // }

  VirtualGameObject *go = native_handle_->add_game_objects();
  auto externalObj = Napi::External<VirtualGameObject>::New(env, go);
  Napi::Object instance = VirtualGameObjectWrap::constructor->New({guid, gameObject, externalObj});
  return instance;
}

Napi::Value VirtualGameObjectModelWrap::CreatePropertyChange(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "Guid is expected").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 2 || !info[1].IsObject())
  {
    Napi::TypeError::New(env, "Change descriptor is expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::String guid = info[0].ToString();
  Napi::Object changeDescriptor = info[1].ToObject();

  auto property_change_native_handle = native_handle_->add_property_changes_on_game_object();
  auto externalObj = Napi::External<PropertyChangeOnGameObject>::New(env, property_change_native_handle);
  return GameObjectPropertyChangeWrap::constructor->New({guid, changeDescriptor, externalObj});
}

Napi::Value VirtualGameObjectModelWrap::CreateVerticesSyncChange(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "Guid is expected").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::String guid = info[0].ToString();
  auto vertices_change_native_handle = native_handle_->add_vertices_changes_on_game_object();
  vertices_change_native_handle->set_target_object_guid(guid.Utf8Value());

  auto trianglesValue = info[1];
  if (!trianglesValue.IsTypedArray())
  {
    Napi::TypeError::New(env, "Triangles is expected to be a TypedArray")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  else
  {
    auto buffer = trianglesValue.As<Napi::TypedArray>();
    if (buffer.TypedArrayType() != napi_uint32_array)
    {
      Napi::TypeError::New(env, "data is expected to be Uint32Array").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Uint32Array triangles_data = buffer.As<Napi::Uint32Array>();
    for (size_t i = 0; i < triangles_data.ElementLength(); i++)
      vertices_change_native_handle->add_mesh_triangles(triangles_data[i]);
  }

  auto verticesValue = info[2].As<Napi::Object>();
  auto isPositionChanged = verticesValue.Has("position");
  auto isNormalsChanged = verticesValue.Has("normals");

  if (isPositionChanged)
  {
    auto bufferValue = verticesValue.Get("position");
    if (!bufferValue.IsTypedArray() || bufferValue.As<Napi::TypedArray>().TypedArrayType() != napi_float32_array)
    {
      Napi::TypeError::New(env, "position is expected to be Float32Array")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    Napi::Float32Array position_data = bufferValue.As<Napi::Float32Array>();
    for (size_t i = 0; i < position_data.ElementLength(); i++)
      vertices_change_native_handle->add_mesh_vertices(position_data[i]);
  }
  if (isNormalsChanged)
  {
    auto bufferValue = verticesValue.Get("normals");
    if (!bufferValue.IsTypedArray() || bufferValue.As<Napi::TypedArray>().TypedArrayType() != napi_float32_array)
    {
      Napi::TypeError::New(env, "normals is expected to be Float32Array")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    Napi::Float32Array normals_data = bufferValue.As<Napi::Float32Array>();
    for (size_t i = 0; i < normals_data.ElementLength(); i++)
      vertices_change_native_handle->add_mesh_normals_vertices(normals_data[i]);
  }
  return env.Null();
}

Napi::Value VirtualGameObjectModelWrap::CreateMaterialSyncChange(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "Guid is expected").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 2 || !info[1].IsString())
  {
    Napi::TypeError::New(env, "type is expected and to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 3 || !info[2].IsObject())
  {
    Napi::TypeError::New(env, "Material is expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::String guid = info[0].ToString();
  Napi::String type = info[1].ToString();
  Napi::Object material = info[2].ToObject();

  auto material_change_native_handle = native_handle_->add_material_changes_on_game_object();
  auto externalObj = Napi::External<MaterialSyncChangeOnGameObject>::New(env, material_change_native_handle);
  return GameObjectMaterialSyncChangeWrap::constructor->New({guid, type, material, externalObj});
}

Napi::Value VirtualGameObjectModelWrap::SerializeAndWrite(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::String channelIdString = info[0].ToString();
  std::string channelId = channelIdString.Utf8Value();

  if (GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()] == nullptr)
  {
    Napi::TypeError::New(env, "Channel ID is not registered");
    return env.Null();
  }
  size_t size = this->native_handle_->ByteSizeLong();
  std::vector<uint8_t> buffer(size);

  if (this->native_handle_->SerializeToArray(buffer.data(), buffer.size()))
  {
    GameObjectModelBufferWritter *gom = GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()];
    gom->setBuffer(buffer.data(), buffer.size());
    return Napi::Number::New(env, buffer.size());
  }
  else
  {
    return Napi::Number::New(env, 0);
  }
}

Napi::Value VirtualGameObjectModelWrap::IsBufferEmpty(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::String channelIdString = info[0].ToString();
  std::string channelId = channelIdString.Utf8Value();

  if (GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()] == nullptr)
  {
    Napi::TypeError::New(env, "Channel ID is not registered");
    return env.Null();
  }
  GameObjectModelBufferWritter *gom = GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()];
  return Napi::Boolean::New(env, gom->bufferSize() == 0);
}

Napi::Value VirtualGameObjectModelWrap::FetchBufferFromWritter(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::String channelIdString = info[0].ToString();
  std::string channelId = channelIdString.Utf8Value();

  if (GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()] == nullptr)
  {
    Napi::TypeError::New(env, "Channel ID is not registered")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  GameObjectModelBufferWritter *gom = GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()];

  uint8_t *data;
  size_t size;

  if (gom->loadBuffer(&data, &size) == -1 || data == nullptr)
    return env.Null();
  return Napi::Buffer<uint8_t>::Copy(env, data, size);
}

Napi::Value VirtualGameObjectModelWrap::FetchAndClearBufferFromWritter(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::String channelIdString = info[0].ToString();
  std::string channelId = channelIdString.Utf8Value();

  if (GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()] == nullptr)
  {
    Napi::TypeError::New(env, "Channel ID is not registered");
    return env.Null();
  }
  GameObjectModelBufferWritter *gom = GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()];

  uint8_t *data;
  size_t size;

  if (gom->loadBuffer(&data, &size) == -1 || data == nullptr)
    return env.Null();

  auto jval = Napi::Buffer<uint8_t>::Copy(env, data, size);
  gom->resetBuffer();
  return jval;
}
