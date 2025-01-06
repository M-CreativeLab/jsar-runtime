#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <iostream>
#include <memory>

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
  typedef struct _CSSPropertyDeclarationBlock _CSSPropertyDeclarationBlock;
  typedef struct
  {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
  } RGBAColor;

  extern RGBAColor parse_csscolor(const char *color_str);
  extern _CSSPropertyDeclarationBlock *parse_style_declaration(const char *style_str);
  extern void css_property_declaration_block_free(_CSSPropertyDeclarationBlock *pdb);
  extern size_t css_property_declaration_block_len(_CSSPropertyDeclarationBlock *pdb);
  extern const char *css_property_declaration_block_get_property(_CSSPropertyDeclarationBlock *pdb, const char *property_name);
  extern void css_property_declaration_block_set_property(_CSSPropertyDeclarationBlock *pdb, const char *property_name, const char *value, bool important);
  extern const char *css_property_declaration_block_remove_property(_CSSPropertyDeclarationBlock *pdb, const char *property_name);
  extern bool css_property_declaration_block_is_important(_CSSPropertyDeclarationBlock *pdb, const char *property_name);
  extern const char *css_property_declaration_block_to_css_string(_CSSPropertyDeclarationBlock *pdb);

  // URL functions
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

    namespace css
    {
      /**
       * The CSS property declaration block to parse CSS style declarations.
       */
      class CSSPropertyDeclarationBlock
      {
      public:
        /**
         * Parse a CSS style declaration string into a CSS property declaration block.
         *
         * @param styleStr The CSS style declaration string.
         * @returns The CSS property declaration block.
         */
        static inline std::shared_ptr<CSSPropertyDeclarationBlock> ParseStyleDeclaration(const std::string &styleStr)
        {
          return std::make_shared<CSSPropertyDeclarationBlock>(parse_style_declaration(styleStr.c_str()));
        }

      public:
        CSSPropertyDeclarationBlock(_CSSPropertyDeclarationBlock *handle) : handle_(handle) {}
        ~CSSPropertyDeclarationBlock()
        {
          css_property_declaration_block_free(handle_);
        }

      public:
        /**
         * Get the CSS text of the property declaration block.
         *
         * @returns The CSS text.
         */
        std::string cssText() const
        {
          const char *str = css_property_declaration_block_to_css_string(handle_);
          std::string cssText(str);
          release_rust_cstring((char *)str);
          return cssText;
        }
        /**
         * @returns The length of the property declaration block.
         */
        size_t size() const
        {
          return css_property_declaration_block_len(handle_);
        }
        /**
         * Check if a property is important.
         *
         * @param propertyName The property name.
         * @returns Whether the property is important.
         */
        bool isPropertyImportant(const std::string &propertyName)
        {
          return css_property_declaration_block_is_important(handle_, propertyName.c_str());
        }
        /**
         * Get the property value.
         *
         * @param propertyName The property name.
         * @returns The property value in string.
         */
        std::string getProperty(const std::string &propertyName)
        {
          const char *value = css_property_declaration_block_get_property(handle_, propertyName.c_str());
          if (value == nullptr)
            return "";

          std::string valueStr(value);
          release_rust_cstring((char *)value);
          return valueStr;
        }
        /**
         * Set a property value.
         *
         * @param propertyName The property name.
         * @param value The property value.
         * @param important Whether the property is important.
         */
        void setProperty(const std::string &propertyName, const std::string &value, bool important = false)
        {
          css_property_declaration_block_set_property(handle_, propertyName.c_str(), value.c_str(), important);
        }
        /**
         * Remove a property from the declaration block.
         *
         * @param propertyName The property name.
         * @returns The removed property value.
         */
        std::string removeProperty(const std::string &propertyName)
        {
          const char *value = css_property_declaration_block_remove_property(handle_, propertyName.c_str());
          if (value == nullptr)
            return "";

          std::string valueStr(value);
          release_rust_cstring((char *)value);
          return valueStr;
        }

      private:
        _CSSPropertyDeclarationBlock *handle_;
      };
    }

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
      namespace style
      {
        using Display = TaffyDisplay;
        using Overflow = TaffyOverflow;
        using Position = TaffyPosition;

        inline std::ostream &operator<<(std::ostream &os, Display display)
        {
          switch (display)
          {
          case Display::kBlock:
            os << "Block";
            break;
          case Display::kFlex:
            os << "Flex";
            break;
          case Display::kGrid:
            os << "Grid";
            break;
          case Display::kNone:
            os << "None";
            break;
          }
          return os;
        }

        inline std::ostream &operator<<(std::ostream &os, Overflow overflow)
        {
          switch (overflow)
          {
          case Overflow::kVisible:
            os << "Visible";
            break;
          case Overflow::kClip:
            os << "Clip";
            break;
          case Overflow::kHidden:
            os << "Hidden";
            break;
          case Overflow::kScroll:
            os << "Scroll";
            break;
          }
          return os;
        }

        inline std::ostream &operator<<(std::ostream &os, Position position)
        {
          switch (position)
          {
          case Position::kRelative:
            os << "Relative";
            break;
          case Position::kAbsolute:
            os << "Absolute";
            break;
          }
          return os;
        }

        class Dimension
        {
          friend class LayoutStyle;

        public:
          /**
           * A length dimension.
           *
           * @param value The length value.
           * @returns The length dimension.
           */
          inline static Dimension Length(float value) { return Dimension(TaffyDimension::kLength, value); }
          /**
           * A percent dimension.
           *
           * @param value The percent value between 0 and 1.
           * @returns The percent dimension.
           */
          inline static Dimension Percent(float value) { return Dimension(TaffyDimension::kPercent, value); }
          /**
           * An auto dimension without value.
           *
           * @returns The auto dimension.
           */
          inline static Dimension Auto() { return Dimension(TaffyDimension::kAuto, 0); }

        public:
          Dimension(TaffyDimension type, float value) : type_(type), value_(value) {}

        public:
          inline bool isLength() { return type_ == TaffyDimension::kLength; }
          inline bool isPercent() { return type_ == TaffyDimension::kPercent; }
          inline bool isAuto() { return type_ == TaffyDimension::kAuto; }

        public:
          friend std::ostream &operator<<(std::ostream &os, const Dimension &dimension)
          {
            if (dimension.type_ == TaffyDimension::kLength)
              os << "Length(" << dimension.value_ << ")";
            else if (dimension.type_ == TaffyDimension::kPercent)
              os << "Percent(" << dimension.value_ << ")";
            else if (dimension.type_ == TaffyDimension::kAuto)
              os << "Auto()";
            return os;
          }

        private:
          TaffyDimension type_;
          float value_;
        };

        /**
         * The layout style to apply to a layout node.
         */
        class LayoutStyle
        {
        public:
          /**
           * Create a new layout style.
           */
          LayoutStyle()
          {
          }
          /**
           * Create a new layout style from a `TaffyStyle`.
           *
           * @param sourceStyle The source style to copy.
           */
          LayoutStyle(TaffyStyle sourceStyle)
              : display(sourceStyle.display),
                overflowX(sourceStyle.overflow_x),
                overflowY(sourceStyle.overflow_y),
                scrollbarWidth(sourceStyle.scrollbar_width),
                position(sourceStyle.position),
                width(Dimension(sourceStyle.width, sourceStyle.width_value)),
                height(Dimension(sourceStyle.height, sourceStyle.height_value)),
                flexGrow(sourceStyle.flex_grow),
                flexShrink(sourceStyle.flex_shrink)
          {
          }

        public:
          friend std::ostream &operator<<(std::ostream &os, const LayoutStyle &style)
          {
            os << "LayoutStyle {" << std::endl;
            os << " display: " << style.display << "," << std::endl;
            os << " overflowX: " << style.overflowX << "," << std::endl;
            os << " overflowY: " << style.overflowY << "," << std::endl;
            os << " scrollbarWidth: " << style.scrollbarWidth << "," << std::endl;
            os << " position: " << style.position << "," << std::endl;
            os << " width: " << style.width << "," << std::endl;
            os << " height: " << style.height << "," << std::endl;
            os << " flexGrow: " << style.flexGrow << "," << std::endl;
            os << " flexShrink: " << style.flexShrink << " }" << std::endl;
            return os;
          }
          operator TaffyStyle()
          {
            TaffyStyle style;
            style.display = display;
            style.overflow_x = overflowX;
            style.overflow_y = overflowY;
            style.scrollbar_width = scrollbarWidth;
            style.position = position;
            style.width = width.type_;
            style.height = height.type_;
            style.width_value = width.value_;
            style.height_value = height.value_;
            style.flex_grow = flexGrow;
            style.flex_shrink = flexShrink;
            return style;
          }

        public:
          Display display = Display::kBlock;
          Overflow overflowX = Overflow::kVisible;
          Overflow overflowY = Overflow::kVisible;
          float scrollbarWidth = 2.0f;
          Position position = Position::kRelative;
          Dimension width = Dimension::Auto();
          Dimension height = Dimension::Auto();
          float flexGrow = 0.0f;
          float flexShrink = 1.0f;
        };
      }

      /**
       * The layout allocator to create layout nodes.
       *
       * An allocator is used to store the layout tree and nodes, please make sure it's created before any layout
       * nodes.
       */
      class Allocator
      {
        friend class Node;

      public:
        Allocator() { tree_ = taffy_tree_new(); }
        ~Allocator() { taffy_tree_free(tree_); }

      private:
        TaffyTree *tree_;
      };

      /**
       * A rectangle with top, right, bottom, and left values.
       *
       * @tparam T The type of the rectangle values.
       */
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

      /**
       * The layout result of a given node.
       */
      class Layout
      {
        friend class Node;

      private:
        Layout(TaffyLayoutOutput output) : data_(output) {}

      public:
        /**
         * @returns The node width.
         */
        inline float width() const { return data_.width; }
        /**
         * @returns The node height.
         */
        inline float height() const { return data_.height; }
        /**
         * @returns The node x position.
         */
        inline float x() const { return data_.x; }
        /**
         * @returns The node y position.
         */
        inline float y() const { return data_.y; }
        /**
         * @returns The node border.
         */
        inline Rect<float> border() const
        {
          return Rect<float>(data_.border_top,
                             data_.border_right,
                             data_.border_bottom,
                             data_.border_left);
        }
        /**
         * @returns The node padding.
         */
        inline Rect<float> padding() const
        {
          return Rect<float>(data_.padding_top,
                             data_.padding_right,
                             data_.padding_bottom,
                             data_.padding_left);
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Layout &layout)
        {
          os << "Layout {" << std::endl;
          os << " width: " << layout.width() << "," << std::endl;
          os << " height: " << layout.height() << "," << std::endl;
          os << " x: " << layout.x() << "," << std::endl;
          os << " y: " << layout.y() << " }";
          os << std::endl;
          return os;
        }

      private:
        TaffyLayoutOutput data_;
      };

      /**
       * The layout node to compute the layout.
       */
      class Node
      {
      public:
        /**
         * Create a new layout node with the specified allocator.
         *
         * @param allocator The layout allocator to use.
         */
        Node(Allocator &allocator)
        {
          node_ = taffy_node_new(allocator.tree_);
        }
        ~Node()
        {
          taffy_node_free(node_);
        }

      public:
        /**
         * Add a child node to this layout node.
         *
         * @param child The child node to add.
         */
        inline void addChild(Node &child) { taffy_node_add_child(node_, child.node_); }
        /**
         * Remove a child node from this layout node.
         *
         * @param child The child node to remove.
         */
        inline void removeChild(Node &child) { taffy_node_remove_child(node_, child.node_); }
        /**
         * @returns The child count of this layout node.
         */
        inline size_t childCount() { return taffy_node_get_child_count(node_); }
        /**
         * @returns The layout style of this node.
         */
        inline style::LayoutStyle style() { return taffy_node_get_style(node_); }
        /**
         * Set the layout style of this node.
         *
         * @param style The layout style to set.
         */
        inline void setStyle(style::LayoutStyle style)
        {
          taffy_node_set_style(node_, style);
        }
        /**
         * Manually mark this node as dirty to recompute the layout.
         */
        inline void markDirty() { taffy_node_mark_dirty(node_); }
        /**
         * Get whether this node is dirty and need to recompute the layout.
         *
         * @returns Whether this node is dirty.
         */
        inline bool isDirty() { return taffy_node_is_dirty(node_); }
        /**
         * Compute the layout of this node (and its children) with the specified parent width and height. Call this
         * method will mark all nodes as not dirty.
         *
         * @param width The parent width.
         * @param height The parent height.
         */
        inline void computeLayout(float width, float height)
        {
          taffy_node_compute_layout(node_, width, height);
        }
        /**
         * @returns The layout output of this node.
         */
        inline Layout layout() { return Layout(taffy_node_get_layout(node_)); }

      private:
        TaffyNode *node_;
      };
    }
  }
}
