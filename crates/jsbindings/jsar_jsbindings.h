#include <node/node.h>
#include <node/node_api.h>

extern "C"
{
    napi_value jsbinding_canvas(napi_env env, napi_value exports);
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

ADD_RS_NODE_MODULE(canvas, "transmute:canvas")
#undef ADD_RS_NODE_MODULE
