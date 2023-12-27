#pragma once

#include <napi.h>

#include "math/mesh.hpp"
#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class VirtualGameObjectDataWrap : public Napi::ObjectWrap<VirtualGameObjectDataWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        VirtualGameObjectDataWrap(const Napi::CallbackInfo &info);
        VirtualGameObjectData *getNativeHandle();

    public:
        static Napi::FunctionReference *constructor;

    private:
        Napi::Value SetMeshTrianglesData(const Napi::CallbackInfo &info);
        Napi::Value SetMeshVertexBuffer(const Napi::CallbackInfo &info);
        Napi::Value ComputeAndSetMeshBuffers(const Napi::CallbackInfo &info);
        Napi::Value SetMeshBounds(const Napi::CallbackInfo &info);
        Napi::Value SetMaterial(const Napi::CallbackInfo &info);

        /** skeleton and skinned mesh related */
        Napi::Value AddBoneWeights(const Napi::CallbackInfo &info);
        Napi::Value AddBoneIndices(const Napi::CallbackInfo &info);
        Napi::Value AddBindPose(const Napi::CallbackInfo &info);
        Napi::Value AddBoneReference(const Napi::CallbackInfo &info);

        /** outline */
        Napi::Value SetMeshOutline(const Napi::CallbackInfo &info);

        /** Bounds related */
        Napi::Value AsBounds(const Napi::CallbackInfo &info);
        Napi::Value AsBlocker(const Napi::CallbackInfo &info);

        /** Line Rendering related */
        Napi::Value SetLineRenderingColors(const Napi::CallbackInfo &info);
        Napi::Value SetLineRenderingWidths(const Napi::CallbackInfo &info);

    private:
        VirtualGameObjectData *native_handle_;
        std::map<std::string, VertexBuffer> mesh_vertex_buffers_;
    };

} // namespace gom
