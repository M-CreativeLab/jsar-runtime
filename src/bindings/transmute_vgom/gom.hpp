#pragma once

#include "buffer_writter.hpp"
#include "vector3_wrap.hpp"
#include "matrix_wrap.hpp"
#include "quaternion_wrap.hpp"
#include "transform_wrap.hpp"
#include "texture_wrap.hpp"
#include "material_wrap.hpp"
#include "document_metadata_wrap.hpp"
#include "go_data_wrap.hpp"
#include "go_property_change_wrap.hpp"
#include "go_material_sync_change_wrap.hpp"
#include "go_wrap.hpp"
#include "gom_wrap.hpp"

Napi::Object InitGameObjectModelModule(Napi::Env env, Napi::Object exports);
