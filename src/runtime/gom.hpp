#pragma once

#include "gom/buffer_writter.hpp"
#include "gom/vector3_wrap.hpp"
#include "gom/matrix_wrap.hpp"
#include "gom/quaternion_wrap.hpp"
#include "gom/transform_wrap.hpp"
#include "gom/texture_wrap.hpp"
#include "gom/material_wrap.hpp"
#include "gom/document_metadata_wrap.hpp"
#include "gom/go_data_wrap.hpp"
#include "gom/go_property_change_wrap.hpp"
#include "gom/go_material_sync_change_wrap.hpp"
#include "gom/go_wrap.hpp"
#include "gom/gom_wrap.hpp"

Napi::Object InitGameObjectModelModule(Napi::Env env, Napi::Object exports);
