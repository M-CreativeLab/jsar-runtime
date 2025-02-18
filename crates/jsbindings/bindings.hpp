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

      holocron::TranspiledScriptSource out = holocron::transpileTypeScript(input);
      if (out.error != "")
        errorMessage = std::string(out.error);
      else
        transpiledCode = std::string(out.code);

      if (!errorMessage.empty())
        throw std::runtime_error(errorMessage);
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
      return Url(holocron::parseWHATWGUrl(url));
    }

  private:
    Url(holocron::WHATWGUrl whatwgUrl) : host(whatwgUrl.host),
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
     * @returns The new URL string.
     */
    static inline std::string CreateUrlStringWithPath(const std::string &baseUrl, const std::string &subPath)
    {
      return std::string(holocron::createUrlWithPath(baseUrl, subPath));
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
      return ModuleExtension(index);
    }
  };

}
