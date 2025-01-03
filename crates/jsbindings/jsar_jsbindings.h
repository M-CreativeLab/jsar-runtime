#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <iostream>

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

  typedef struct
  {
    const char *host;
    const char *hostname;
    const char *href;
    const char *origin;
    const char *password;
    const char *pathname;
    int32_t port;
    const char *protocol;
    const char *search;
    const char *username;
    const char *hash;
  } WHATWGUrl;

  /**
   * Parse a URL string into a WHATWGUrl struct.
   *
   * @param url The URL string to parse.
   * @returns The parsed WHATWGUrl struct.
   */
  extern WHATWGUrl parse_whatwg_url(const char *url);

  /**
   * Release a WHATWGUrl struct.
   *
   * @param url The WHATWGUrl struct to release.
   */
  extern void release_whatwg_url(WHATWGUrl url);

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

  // Layout structs
  typedef void *TaffyTree;
  typedef void *TaffyNode;
  enum class TaffyDisplay : int32_t
  {
    kBlock = 0,
    kFlex,
    kGrid,
    kNone,
  };
  enum class TaffyOverflow : int32_t
  {
    kVisible = 0,
    kClip,
    kHidden,
    kScroll,
  };
  enum class TaffyPosition : int32_t
  {
    kRelative = 0,
    kAbsolute,
  };
  enum class TaffyDimension : int32_t
  {
    kLength = 0,
    kPercent,
    kAuto,
  };
  struct TaffyStyle
  {
    TaffyDisplay display;
    TaffyOverflow overflow_x;
    TaffyOverflow overflow_y;
    float scrollbar_width;
    TaffyPosition position;
    TaffyDimension width;
    TaffyDimension height;
    float width_value;
    float height_value;
    float flex_grow;
    float flex_shrink;
  };
  struct TaffyLayoutOutput
  {
    float width;
    float height;
    float x;
    float y;
    float border_top;
    float border_right;
    float border_bottom;
    float border_left;
    float padding_top;
    float padding_right;
    float padding_bottom;
    float padding_left;
  };

  // Layout functions
  extern TaffyTree *taffy_tree_new();
  extern void taffy_tree_free(TaffyTree *tree);
  extern TaffyNode *taffy_node_new(TaffyTree *tree);
  extern void taffy_node_free(TaffyNode *node);
  extern void taffy_node_add_child(TaffyNode *parent, TaffyNode *child);
  extern void taffy_node_remove_child(TaffyNode *parent, TaffyNode *child);
  extern TaffyStyle taffy_node_get_style(TaffyNode *node);
  extern void taffy_node_set_style(TaffyNode *node, TaffyStyle style);
  extern void taffy_node_mark_dirty(TaffyNode *node);
  extern bool taffy_node_is_dirty(TaffyNode *node);
  extern bool taffy_node_is_childless(TaffyNode *node);
  extern size_t taffy_node_get_child_count(TaffyNode *node);
  extern void taffy_node_compute_layout(TaffyNode *node, float parent_width, float parent_height);
  extern TaffyLayoutOutput taffy_node_get_layout(TaffyNode *node);
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
      Url(WHATWGUrl whatwgUrl) : host(whatwgUrl.host),
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

    namespace layout
    {
      class Allocator
      {
        friend class Node;

      public:
        Allocator()
        {
          tree_ = taffy_tree_new();
        }

        ~Allocator()
        {
          taffy_tree_free(tree_);
        }

      private:
        TaffyTree *tree_;
      };

      template <typename T>
      class Rect
      {
      public:
        Rect(T top, T right, T bottom, T left)
            : top_(top), right_(right), bottom_(bottom), left_(left) {}

      public:
        T top() { return top_; }
        T right() { return right_; }
        T bottom() { return bottom_; }
        T left() { return left_; }

      private:
        T top_;
        T right_;
        T bottom_;
        T left_;
      };

      class Layout
      {
      public:
        Layout(TaffyLayoutOutput output) : data_(output) {}

      public:
        inline float width() const { return data_.width; }
        inline float height() const { return data_.height; }
        inline float x() const { return data_.x; }
        inline float y() const { return data_.y; }
        inline Rect<float> border() const
        {
          return Rect<float>(data_.border_top,
                             data_.border_right,
                             data_.border_bottom,
                             data_.border_left);
        }
        inline Rect<float> padding() const
        {
          return Rect<float>(data_.padding_top,
                             data_.padding_right,
                             data_.padding_bottom,
                             data_.padding_left);
        }

      public:
        // custom std >> operator
        friend std::ostream &operator<<(std::ostream &os, const Layout &layout)
        {
          os << "Layout {";
          os << " width: " << layout.width() << ",";
          os << " height: " << layout.height() << ",";
          os << " x: " << layout.x() << ",";
          os << " y: " << layout.y() << " }";
          os << std::endl;
          return os;
        }

      private:
        TaffyLayoutOutput data_;
      };

      class Node
      {
      public:
        Node(Allocator &allocator)
        {
          node_ = taffy_node_new(allocator.tree_);
        }
        ~Node()
        {
          taffy_node_free(node_);
        }

      public:
        inline void addChild(Node &child) { taffy_node_add_child(node_, child.node_); }
        inline void removeChild(Node &child) { taffy_node_remove_child(node_, child.node_); }
        inline size_t childCount() { return taffy_node_get_child_count(node_); }
        inline TaffyStyle style() { return taffy_node_get_style(node_); }
        inline void setStyle(TaffyStyle style) { taffy_node_set_style(node_, style); }
        inline void markDirty() { taffy_node_mark_dirty(node_); }
        inline bool isDirty() { return taffy_node_is_dirty(node_); }
        inline void computeLayout(float width, float height)
        {
          taffy_node_compute_layout(node_, width, height);
        }
        inline Layout layout() { return Layout(taffy_node_get_layout(node_)); }

      private:
        TaffyNode *node_;
      };
    }
  }
}
