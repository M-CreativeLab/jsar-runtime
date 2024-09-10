#pragma once

#include <stdint.h>
#include <stddef.h>

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

  /**
   * Parse a CSS color string into a RGBAColor struct.
   *
   * @param color_str The CSS color string to parse: "rgb(255, 0, 0)", "rgba(255, 0, 0, 0.5)", "#ff0000", "red", etc.
   */
  extern RGBAColor parse_csscolor(const char *color_str);

  /**
   * Create a new URL string by combining a base URL and a sub path.
   *
   * @param base_url The base URL to use.
   * @param path The sub path to append to the base URL.
   * @param out_url_str The output buffer to write the new URL string to.
   * @param out_url_max_len The maximum length of the output buffer.
   * @returns The output length of the new URL string.
   */
  extern size_t create_url_with_path(const char *base_url, const char *path, char **out_url_str, size_t out_url_max_len);
} // extern "C"
