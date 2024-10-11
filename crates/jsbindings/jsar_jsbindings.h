#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>

extern "C"
{
  /**
   * Load the WebGL bindings.
   */
  void jsar_load_gl();

  /**
   * Release a Rust CString.
   *
   * @param s The CString to release.
   */
  extern void release_rust_cstring(char *s);

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

  /**
   * Parse a URL string to get the module extension.
   *
   * For example, "https://example.com/index.js" will return JavaScript(0), "https://example.com/audio.mp3" will returns MP3(10).
   *
   * @param url The URL string to parse.
   * @returns The module extension index.
   */
  extern int parse_url_to_module_extension(const char *url);

  /**
   * Patch a GLSL source string.
   *
   * @param input The GLSL source string to patch.
   * @returns The patched GLSL source string, caller need to release this memory by calling release_rust_cstring().
   */
  extern char *patch_glsl_source(const char *input);

  typedef struct
  {
    const char *code;
    const char *message;
  } TranspiledTypeScriptOutput;

  /**
   * Transpile TypeScript source code to JavaScript.
   *
   * @param input The TypeScript source code
   * @returns The transpiled JavaScript code and error message if any.
   */
  extern TranspiledTypeScriptOutput transpile_typescript_to_js(const char *input);

  /**
   * Release the transpiled TypeScript output.
   *
   * @param output The transpiled TypeScript output to release.
   */
  extern void release_transpiled_typescript_output(TranspiledTypeScriptOutput output);
} // extern "C"

namespace crates
{
  namespace jsar
  {
    enum class ModuleExtensionIndex
    {
      kJavaScript = 0,
      kTypeScript,
      kJSON,
      kBin,
      kData,
      kWebAssembly,
      kPNG,
      kJPEG,
      kGIF,
      kSVG,
      kMP3,
      kWAV,
      kOGG,
    };

    /**
     * Module extension class to help getting the module file type.
     */
    class ModuleExtension
    {
    public:
      ModuleExtension(ModuleExtensionIndex index) : index(index) {}

    public:
      /**
       * Returns if this module is a text source module.
       */
      inline bool isTextSourceModule()
      {
        return index == ModuleExtensionIndex::kJavaScript ||
               index == ModuleExtensionIndex::kTypeScript;
      }

      /**
       * Returns if this module is a Typescript module.
       */
      inline bool isTypeScript()
      {
        return index == ModuleExtensionIndex::kTypeScript;
      }

      /**
       * Returns if this module is a JSON module.
       */
      inline bool isJson()
      {
        return index == ModuleExtensionIndex::kJSON;
      }

      /**
       * Returns if this module is a binary module.
       */
      inline bool isBinary()
      {
        return index == ModuleExtensionIndex::kBin || index == ModuleExtensionIndex::kData;
      }

      /**
       * Returns if this module is a WebAssembly module.
       */
      inline bool isWebAssembly()
      {
        return index == ModuleExtensionIndex::kWebAssembly;
      }

      /**
       * Returns if this module is an image module.
       */
      inline bool isImage()
      {
        return index == ModuleExtensionIndex::kPNG ||
               index == ModuleExtensionIndex::kJPEG ||
               index == ModuleExtensionIndex::kGIF ||
               index == ModuleExtensionIndex::kSVG;
      }

      /**
       * Returns if this module is an audio module.
       */
      inline bool isAudio()
      {
        return index == ModuleExtensionIndex::kMP3 ||
               index == ModuleExtensionIndex::kWAV ||
               index == ModuleExtensionIndex::kOGG;
      }

    public:
      ModuleExtensionIndex index;
    };

    class TypeScriptTranspiler
    {
    public:
      /**
       * Transpile TypeScript source code to JavaScript.
       *
       * @param input The TypeScript source code.
       * @returns The transpiled JavaScript code and error message if any.
       */
      static std::string Transpile(const std::string &input)
      {
        std::string errorMessage;
        std::string transpiledCode;
        TranspiledTypeScriptOutput out = transpile_typescript_to_js(input.c_str());
        if (out.code == nullptr)
          errorMessage = out.message == nullptr ? "Failed to transpile TypeScript to JavaScript." : out.message;
        else
          transpiledCode = out.code;

        release_transpiled_typescript_output(out);
        if (!errorMessage.empty())
          throw std::runtime_error(errorMessage);
        else
          return transpiledCode;
      }
    };

    class UrlHelper
    {
    public:
      /**
       * Create a new URL string by combining a base URL and a sub path.
       *
       * @param baseUrl The base URL to use.
       * @param subPath The sub path to append to the base URL.
       * @param urlMaxLength The maximum length of the output URL string.
       * @returns The new URL string.
       */
      static inline std::string CreateUrlStringWithPath(const std::string &baseUrl, const std::string &subPath, size_t urlMaxLength = 512)
      {
        char newUrl[urlMaxLength];
        size_t len = create_url_with_path(baseUrl.c_str(), subPath.c_str(), (char **)&newUrl, sizeof(newUrl));
        return len == 0 ? "" : std::string(newUrl, len);
      }

      /**
       * Parse a given URL string to get the module extension.
       *
       * @param url The URL string to parse.
       * @returns The module extension.
       */
      static inline ModuleExtension ParseUrlToModuleExtension(const std::string &url)
      {
        return ModuleExtension((ModuleExtensionIndex)parse_url_to_module_extension(url.c_str()));
      }
    };

    namespace webgl
    {
      /**
       * GLSLPatcher is used to patch a GLSL source string.
       */
      class GLSLSourcePatcher
      {
      public:
        /**
         * Get the patched GLSL source string.
         *
         * @param source The GLSL source string to patch.
         * @returns The patched GLSL source string.
         */
        static inline std::string GetPatchedSource(const std::string &source)
        {
          char *patchedSourceStr = patch_glsl_source(source.c_str());
          std::string patchedSource(patchedSourceStr);
          release_rust_cstring(patchedSourceStr);
          return patchedSource;
        }
      };
    }
  }
}
