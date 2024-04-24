#pragma once

#include <node/node.h>
#include <node/node_api.h>

extern "C"
{
    // C++ exports
    void jsar_load_gl();
    const uint8_t *get_jsbundle_ptr();
    uintptr_t get_jsbundle_size();

    // JS exports
    napi_value jsbinding_htmlrender(napi_env env, napi_value exports);
} // extern "C"

#define ADD_RS_NODE_MODULE(varname, modname)              \
    static napi_module transmute_##varname##_napi_mod = { \
        NAPI_MODULE_VERSION,                              \
        node::ModuleFlags::kLinked,                       \
        nullptr,                                          \
        jsbinding_##varname,                              \
        modname,                                          \
        nullptr,                                          \
        {0},                                              \
    };

ADD_RS_NODE_MODULE(htmlrender, "transmute:htmlrender")
#undef ADD_RS_NODE_MODULE
