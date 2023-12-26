#include "gom.hpp"

using namespace gom;

Napi::Object InitGameObjectModelModule(Napi::Env env, Napi::Object exports)
{
    // Math Base
    VirtualVector3Wrap::Init(env, exports);
    VirtualQuaternionWrap::Init(env, exports);
    VirtualMatrixWrap::Init(env, exports);
    VirtualTransformWrap::Init(env, exports);
    VirtualTextureWrap::Init(env, exports);
    VirtualMaterialWrap::Init(env, exports);

    // Document metadata
    DocumentMetadataWrap::Init(env, exports);

    // GameObject
    VirtualGameObjectDataWrap::Init(env, exports);
    VirtualGameObjectWrap::Init(env, exports);
    VirtualGameObjectModelWrap::Init(env, exports);

    // Changes
    GameObjectPropertyChangeWrap::Init(env, exports);
    GameObjectMaterialSyncChangeWrap::Init(env, exports);

    return exports;
}
