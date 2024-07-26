#pragma once

#include <node/node.h>
#include <node/node_api.h>

#define ADD_RS_NODE_MODULE(varname, modname)                          \
    napi_value jsbinding_##varname(napi_env env, napi_value exports); \
    static napi_module transmute_##varname##_napi_mod = {             \
        NAPI_MODULE_VERSION,                                          \
        node::ModuleFlags::kLinked,                                   \
        nullptr,                                                      \
        jsbinding_##varname,                                          \
        modname,                                                      \
        nullptr,                                                      \
        {0},                                                          \
    };

extern "C"
{
    // C++ exports
    void jsar_load_gl();

    // CSS parsing functions
    typedef struct
    {
        uint32_t r;
        uint32_t g;
        uint32_t b;
        uint32_t a;
    } RGBAColor;
    extern RGBAColor parse_csscolor(const char *color_str);

    // HTML rendering functions
    extern void render_html(const char *html_str);

    // JS exports
    ADD_RS_NODE_MODULE(htmlrender, "transmute:htmlrender")
} // extern "C"

#undef ADD_RS_NODE_MODULE
