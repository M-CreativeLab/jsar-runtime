#include "base.hpp"
#include "go_data_wrap.hpp"
#include "vector3_wrap.hpp"
#include "matrix_wrap.hpp"
#include "material_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualGameObjectDataWrap::constructor;
void VirtualGameObjectDataWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "GameObjectData",
      {
          InstanceMethod("setMeshTrianglesData", &VirtualGameObjectDataWrap::SetMeshTrianglesData),
          InstanceMethod("setMeshVertexBuffer", &VirtualGameObjectDataWrap::SetMeshVertexBuffer),
          InstanceMethod("computeAndSetMeshBuffers", &VirtualGameObjectDataWrap::ComputeAndSetMeshBuffers),
          InstanceMethod("setMeshBounds", &VirtualGameObjectDataWrap::SetMeshBounds),
          InstanceMethod("setMaterial", &VirtualGameObjectDataWrap::SetMaterial),
          InstanceMethod("addBoneWeights", &VirtualGameObjectDataWrap::AddBoneWeights),
          InstanceMethod("addBoneIndices", &VirtualGameObjectDataWrap::AddBoneIndices),
          InstanceMethod("addBindPose", &VirtualGameObjectDataWrap::AddBindPose),
          InstanceMethod("addBoneReference", &VirtualGameObjectDataWrap::AddBoneReference),
          InstanceMethod("setMeshOutline", &VirtualGameObjectDataWrap::SetMeshOutline),
          InstanceMethod("asBounds", &VirtualGameObjectDataWrap::AsBounds),
          InstanceMethod("asBlocker", &VirtualGameObjectDataWrap::AsBlocker),
          InstanceMethod("setLineRenderingColors", &VirtualGameObjectDataWrap::SetLineRenderingColors),
          InstanceMethod("setLineRenderingWidths", &VirtualGameObjectDataWrap::SetLineRenderingWidths),
      });

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  env.SetInstanceData(constructor);

  exports.Set("VirtualGameObjectData", tpl);
}

VirtualGameObjectDataWrap::VirtualGameObjectDataWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualGameObjectDataWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::Object selfObj = info.This().ToObject();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsObject())
  {
    Napi::TypeError::New(env, "GameObject is expected").ThrowAsJavaScriptException();
    return;
  }

  native_handle_ = new VirtualGameObjectData();
  Napi::Object gameObject = info[0].ToObject();

  if (gameObject.Has("parent"))
  {
    Napi::Value parentValue = gameObject.Get("parent");
    if (parentValue.IsObject())
    {
      Napi::Object parentObject = parentValue.ToObject();
      if (parentObject.Has(TRANSMUTE_VGO_GUID_KEY))
      {
        Napi::Number parentGuid = parentObject.Get(TRANSMUTE_VGO_GUID_KEY).ToNumber();
        native_handle_->set_parent_guid(parentGuid.Uint32Value());
      }
    }
  }

  selfObj.Set("parentGuid", native_handle_->parent_guid());
}

VirtualGameObjectData *VirtualGameObjectDataWrap::getNativeHandle()
{
  return this->native_handle_;
}

Napi::Value VirtualGameObjectDataWrap::SetMeshTrianglesData(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "data is expected").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto trianglesValue = info[0];
  if (trianglesValue.IsTypedArray())
  {
    auto buffer = trianglesValue.As<Napi::TypedArray>();
    if (buffer.TypedArrayType() == napi_uint32_array)
    {
      // mesh_triangles_data_ = buffer.As<Napi::Uint32Array>();
      info.This().ToObject().Set("mesh_triangles_data_", buffer.As<Napi::Uint32Array>());
      return Napi::Boolean::New(env, true);
    }
    else
    {
      Napi::TypeError::New(env, "data is expected to be Uint32Array").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
  else
  {
    Napi::TypeError::New(env, "data is expected to be TypedArray or Array")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value VirtualGameObjectDataWrap::SetMeshVertexBuffer(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 2)
  {
    Napi::TypeError::New(env, "type and vertex buffer is expected").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::String type = info[0].ToString();
  Napi::Value vertexBufferValue = info[1];

  if (!vertexBufferValue.IsTypedArray())
  {
    Napi::TypeError::New(env, "data is expected to be TypedArray")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Float32Array vertex_float32_array;
  std::string typeStr = type.Utf8Value();

  if (typeStr.empty())
  {
    Napi::TypeError::New(env, "type must not be an empty string").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (mesh_vertex_buffers_.find(typeStr) != mesh_vertex_buffers_.end())
  {
    std::string message = "type \"" + typeStr + "\" already setup";
    Napi::TypeError::New(env, message.c_str()).ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto buffer = vertexBufferValue.As<Napi::TypedArray>();
  if (buffer.TypedArrayType() != napi_float32_array)
  {
    Napi::TypeError::New(env, "data is expected to be Float32Array").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Float32Array vertexBufferArray = buffer.As<Napi::Float32Array>();
  VertexBuffer vertexBuffer;
  vertexBuffer.length = vertexBufferArray.ElementLength();
  vertexBuffer.byteLength = vertexBufferArray.ByteLength();
  float *pVertexBufferData = reinterpret_cast<float *>(vertexBufferArray.Data());

  vertexBuffer.data = new float[vertexBuffer.byteLength / sizeof(float)];
  std::memcpy(vertexBuffer.data, pVertexBufferData, vertexBuffer.byteLength);
  mesh_vertex_buffers_[typeStr] = vertexBuffer;

  return Napi::Boolean::New(env, true);
}

Napi::Value VirtualGameObjectDataWrap::ComputeAndSetMeshBuffers(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  VertexBuffer positionVertexBuffer = mesh_vertex_buffers_["position"];
  VertexBuffer normalsVertexBuffer, colorsVertexBuffer, uvVertexBuffer;

  auto isNormalsSet = mesh_vertex_buffers_.find("normals") != mesh_vertex_buffers_.end();
  auto isColorsSet = mesh_vertex_buffers_.find("colors") != mesh_vertex_buffers_.end();
  auto isUvSet = mesh_vertex_buffers_.find("uv") != mesh_vertex_buffers_.end();

  if (isNormalsSet)
    normalsVertexBuffer = mesh_vertex_buffers_["normals"];

  for (size_t i = 0; i < positionVertexBuffer.length; i++)
  {
    native_handle_->add_mesh_vertices(positionVertexBuffer.data[i]);
    if (isNormalsSet)
      native_handle_->add_mesh_normals_vertices(normalsVertexBuffer.data[i]);
  }

  if (isUvSet)
  {
    uvVertexBuffer = mesh_vertex_buffers_["uv"];
    for (size_t i = 0; i < uvVertexBuffer.length; i++)
      native_handle_->add_mesh_uvs_vertices(uvVertexBuffer.data[i]);
  }

  if (isColorsSet)
  {
    colorsVertexBuffer = mesh_vertex_buffers_["colors"];
    for (size_t i = 0; i < colorsVertexBuffer.length; i++)
      native_handle_->add_mesh_colors_vertices(colorsVertexBuffer.data[i]);
  }

  Napi::Uint32Array mesh_triangles_data = info.This().ToObject().Get("mesh_triangles_data_").As<Napi::Uint32Array>();
  for (size_t i = 0; i < mesh_triangles_data.ElementLength(); i++)
    native_handle_->add_mesh_triangles(mesh_triangles_data[i]);

  return info.Env().Undefined();
}

Napi::Value VirtualGameObjectDataWrap::SetMeshBounds(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "bounding object are expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value minSourceValue = info[0].As<Napi::Object>().Get("minimum");
  Napi::Value maxSourceValue = info[0].As<Napi::Object>().Get("maximum");

  if (!minSourceValue.IsEmpty())
  {
    Napi::Value minValue = VirtualVector3Wrap::constructor->New({minSourceValue});
    VirtualVector3Wrap *minWrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(minValue.As<Napi::Object>());
    native_handle_->set_allocated_mesh_bounds_min(minWrap->getNativeHandle());
  }
  if (!maxSourceValue.IsEmpty())
  {
    Napi::Value maxValue = VirtualVector3Wrap::constructor->New({maxSourceValue});
    VirtualVector3Wrap *maxWrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(maxValue.As<Napi::Object>());
    native_handle_->set_allocated_mesh_bounds_max(maxWrap->getNativeHandle());
  }
  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::SetMaterial(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 2)
  {
    Napi::TypeError::New(env, "type and material object are expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString())
  {
    Napi::TypeError::New(env, "type argument is expected to be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value mat = info[1];
  if (mat.IsObject() && mat.As<Napi::Object>().InstanceOf(VirtualMaterialWrap::constructor->Value()))
  {
    VirtualMaterialWrap *materialWrap = Napi::ObjectWrap<VirtualMaterialWrap>::Unwrap(mat.As<Napi::Object>());
    native_handle_->set_material_id(materialWrap->id);
    info.This().As<Napi::Object>().Set("material", mat);
    return Napi::Boolean::New(env, true);
  }
  else
  {
    Napi::TypeError::New(env, "material object is invalid, use binding.VirtualMaterial()")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value VirtualGameObjectDataWrap::AddBoneWeights(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() > 0 && info[0].IsNumber())
    native_handle_->add_skeleton_weights(info[0].As<Napi::Number>().FloatValue());
  if (info.Length() > 1 && info[1].IsNumber())
    native_handle_->add_skeleton_weights(info[1].As<Napi::Number>().FloatValue());
  if (info.Length() > 2 && info[2].IsNumber())
    native_handle_->add_skeleton_weights(info[2].As<Napi::Number>().FloatValue());
  if (info.Length() > 3 && info[3].IsNumber())
    native_handle_->add_skeleton_weights(info[3].As<Napi::Number>().FloatValue());

  if (native_handle_->skeleton_is_skinned_mesh() == false)
    native_handle_->set_skeleton_is_skinned_mesh(true);

  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::AddBoneIndices(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() > 0 && info[0].IsNumber())
    native_handle_->add_skeleton_indexes(info[0].As<Napi::Number>().FloatValue());
  if (info.Length() > 1 && info[1].IsNumber())
    native_handle_->add_skeleton_indexes(info[1].As<Napi::Number>().FloatValue());
  if (info.Length() > 2 && info[2].IsNumber())
    native_handle_->add_skeleton_indexes(info[2].As<Napi::Number>().FloatValue());
  if (info.Length() > 3 && info[3].IsNumber())
    native_handle_->add_skeleton_indexes(info[3].As<Napi::Number>().FloatValue());

  if (native_handle_->skeleton_is_skinned_mesh() == false)
    native_handle_->set_skeleton_is_skinned_mesh(true);

  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::AddBindPose(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  Napi::Value matrixValue = info[0];

  if (matrixValue.IsObject() && matrixValue.As<Napi::Object>().InstanceOf(VirtualMatrixWrap::constructor->Value()))
  {
    VirtualMatrixWrap *matrixWrap = Napi::ObjectWrap<VirtualMatrixWrap>::Unwrap(matrixValue.As<Napi::Object>());
    auto matrix = matrixWrap->getNativeHandle();
    native_handle_->add_skeleton_bindposes()->CopyFrom(*matrix);
  }
  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::AddBoneReference(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::Value guidToRef = info[0];
  if (guidToRef.IsString())
  {
    std::string guid = guidToRef.As<Napi::String>().Utf8Value();
    native_handle_->add_skeleton_guid_of_bones(guid);
  }
  else
  {
    Napi::TypeError::New(env, "guid is expected and to be a string")
        .ThrowAsJavaScriptException();
  }
  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::SetMeshOutline(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 2)
  {
    Napi::TypeError::New(env, "outline visible and properties are expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (!info[0].IsBoolean())
  {
    Napi::TypeError::New(env, "outline visible is expected to be a boolean")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (!info[1].IsObject())
  {
    Napi::TypeError::New(env, "outline properties is expected to be an object")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  bool outlineVisible = info[0].As<Napi::Boolean>().Value();
  if (outlineVisible == true)
  {
    Napi::Object propertiesObject = info[1].As<Napi::Object>();
    if (propertiesObject.Has("color"))
    {
      Napi::Value colorValue = propertiesObject.Get("color");
      if (colorValue.IsObject())
      {
        Napi::Object colorObject = colorValue.As<Napi::Object>();
        if (colorObject.Has("r") && colorObject.Has("g") && colorObject.Has("b"))
        {
          Napi::Value rValue = colorObject.Get("r");
          Napi::Value gValue = colorObject.Get("g");
          Napi::Value bValue = colorObject.Get("b");
          if (rValue.IsNumber() && gValue.IsNumber() && bValue.IsNumber())
          {
            native_handle_->add_outline_color(rValue.As<Napi::Number>().FloatValue());
            native_handle_->add_outline_color(gValue.As<Napi::Number>().FloatValue());
            native_handle_->add_outline_color(bValue.As<Napi::Number>().FloatValue());
          }
        }
      }
    }
    if (propertiesObject.Has("width"))
    {
      Napi::Value widthValue = propertiesObject.Get("width");
      if (widthValue.IsNumber())
      {
        native_handle_->set_outline_width(widthValue.As<Napi::Number>().FloatValue());
      }
    }
    native_handle_->set_outline_enabled(true);
  }
  else
  {
    native_handle_->set_outline_enabled(false);
  }
  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::AsBounds(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  native_handle_->set_collider_enabled(true);
  native_handle_->set_collider_type(1);
  return info.Env().Undefined();
}

Napi::Value VirtualGameObjectDataWrap::AsBlocker(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  native_handle_->set_collider_enabled(true);
  native_handle_->set_collider_type(10);
  return info.Env().Undefined();
}

Napi::Value VirtualGameObjectDataWrap::SetLineRenderingColors(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsObject())
  {
    Napi::TypeError::New(env, "colors is expected and to be an object")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Object colorStartObject = info[0].As<Napi::Object>();
  if (colorStartObject.Has("r") && colorStartObject.Has("g") && colorStartObject.Has("b"))
  {
    Napi::Value rValue = colorStartObject.Get("r");
    Napi::Value gValue = colorStartObject.Get("g");
    Napi::Value bValue = colorStartObject.Get("b");
    if (rValue.IsNumber() && gValue.IsNumber() && bValue.IsNumber())
    {
      native_handle_->add_line_rendering_color_start(rValue.As<Napi::Number>().FloatValue() / 255);
      native_handle_->add_line_rendering_color_start(gValue.As<Napi::Number>().FloatValue() / 255);
      native_handle_->add_line_rendering_color_start(bValue.As<Napi::Number>().FloatValue() / 255);
    }
  }
  else
  {
    Napi::TypeError::New(env, "color start should be a Color3 whichi contains properties: r, g, b")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  // color end
  if (info.Length() == 2 && info[1].IsObject())
  {
    Napi::Object colorEndObject = info[1].As<Napi::Object>();
    if (colorEndObject.Has("r") && colorEndObject.Has("g") && colorEndObject.Has("b"))
    {
      Napi::Value rValue = colorEndObject.Get("r");
      Napi::Value gValue = colorEndObject.Get("g");
      Napi::Value bValue = colorEndObject.Get("b");
      if (rValue.IsNumber() && gValue.IsNumber() && bValue.IsNumber())
      {
        native_handle_->add_line_rendering_color_end(rValue.As<Napi::Number>().FloatValue() / 255);
        native_handle_->add_line_rendering_color_end(gValue.As<Napi::Number>().FloatValue() / 255);
        native_handle_->add_line_rendering_color_end(bValue.As<Napi::Number>().FloatValue() / 255);
      }
    }
  }

  return env.Undefined();
}

Napi::Value VirtualGameObjectDataWrap::SetLineRenderingWidths(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "start is expected and to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  native_handle_->set_line_rendering_width_start(info[0].As<Napi::Number>().FloatValue());

  // width end
  if (info.Length() == 2 && info[1].IsNumber())
    native_handle_->set_line_rendering_width_end(info[1].As<Napi::Number>().FloatValue());

  return env.Undefined();
}
