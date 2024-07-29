#pragma once

#include <stdint.h>

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
} // extern "C"
