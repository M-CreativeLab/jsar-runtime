#pragma once

#include <stdint.h>
#include <stddef.h>

#include <atomic>
#include <mutex>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include "./bindings.autogen.h"
#include "./holocron.autogen.hpp"
#include "./holocron_layout.autogen.hpp"

// Modules
#include "./bindings.css-inl.hpp"
#include "./bindings.layout.hpp"
#include "./bindings.webgl.hpp"

namespace crates
{
  using namespace holocron;

  /**
   * Module extension class to help getting the module file type.
   */
  class ModuleExtension
  {
  public:
    ModuleExtension(holocron::ModuleExtensionIndex index) : index(index) {}

  public:
    /**
     * Returns if this module is a text source module.
     */
    inline bool isTextSourceModule()
    {
      return index == holocron::ModuleExtensionIndex::kJavaScript ||
             index == holocron::ModuleExtensionIndex::kTypeScript;
    }

    /**
     * Returns if this module is a Typescript module.
     */
    inline bool isTypeScript()
    {
      return index == holocron::ModuleExtensionIndex::kTypeScript;
    }

    /**
     * Returns if this module is a JSON module.
     */
    inline bool isJson()
    {
      return index == holocron::ModuleExtensionIndex::kJSON;
    }

    /**
     * Returns if this module is a binary module.
     */
    inline bool isBinary()
    {
      return index == holocron::ModuleExtensionIndex::kBin ||
             index == holocron::ModuleExtensionIndex::kData;
    }

    /**
     * Returns if this module is a WebAssembly module.
     */
    inline bool isWebAssembly()
    {
      return index == holocron::ModuleExtensionIndex::kWebAssembly;
    }

    /**
     * Returns if this module is an image module.
     */
    inline bool isImage()
    {
      return index == holocron::ModuleExtensionIndex::kPNG ||
             index == holocron::ModuleExtensionIndex::kJPEG ||
             index == holocron::ModuleExtensionIndex::kGIF ||
             index == holocron::ModuleExtensionIndex::kSVG;
    }

    /**
     * Returns if this module is an audio module.
     */
    inline bool isAudio()
    {
      return index == holocron::ModuleExtensionIndex::kMP3 ||
             index == holocron::ModuleExtensionIndex::kWAV ||
             index == holocron::ModuleExtensionIndex::kOGG;
    }

  public:
    holocron::ModuleExtensionIndex index;
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
      _TranspiledTypeScriptOutput out = transpile_typescript_to_js(input.c_str());
      if (out.code == nullptr)
        errorMessage = out.error_message == nullptr ? "Failed to transpile TypeScript to JavaScript." : out.error_message;
      else
        transpiledCode = out.code;

      release_transpiled_typescript_output(out);
      if (!errorMessage.empty())
        throw std::runtime_error(errorMessage);
      else
        return transpiledCode;
    }
  };

  class Url
  {
  public:
    /**
     * Parse a URL string into a WHATWGUrl struct.
     *
     * @param url The URL string to parse.
     * @returns The parsed WHATWGUrl struct.
     */
    static inline Url Parse(const std::string &url)
    {
      return Url(parse_whatwg_url(url.c_str()));
    }

  private:
    Url(_WHATWGUrl whatwgUrl) : host(whatwgUrl.host),
                                hostname(whatwgUrl.hostname),
                                href(whatwgUrl.href),
                                origin(whatwgUrl.origin),
                                password(whatwgUrl.password),
                                pathname(whatwgUrl.pathname),
                                port(whatwgUrl.port),
                                protocol(whatwgUrl.protocol),
                                search(whatwgUrl.search),
                                username(whatwgUrl.username),
                                hash(whatwgUrl.hash)
    {
      release_whatwg_url(whatwgUrl);
    }

  public:
    std::string host;
    std::string hostname;
    std::string href;
    std::string origin;
    std::string password;
    std::string pathname;
    int32_t port;
    std::string protocol;
    std::string search;
    std::string username;
    std::string hash;
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
      auto index = holocron::parseURLToModuleExtension(url.c_str());
      std::cout << "ParseUrlToModuleExtension(" << url << "): " << static_cast<int>(index) << std::endl;
      return ModuleExtension(index);
    }
  };

}
