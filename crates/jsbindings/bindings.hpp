#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <iostream>
#include <memory>

#include "./bindings.autogen.h"

namespace crates
{
  using namespace holocron;

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

    /**
     * Parse a CSS color string into an RGBA color.
     *
     * @param colorStr The CSS color string.
     * @returns The RGBA color.
     */
    inline _RGBAColor parseColor(const std::string &colorStr)
    {
      return parse_csscolor(colorStr.c_str());
    }
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
    template <typename T>
    class Point
    {
    public:
      Point(T x, T y) : x_(x), y_(y) {}

    public:
      T &x() { return x_; }
      T x() const { return x_; }
      T &y() { return y_; }
      T y() const { return y_; }

    private:
      T x_;
      T y_;
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
      T &top() { return top_; }
      T top() const { return top_; }
      T &right() { return right_; }
      T right() const { return right_; }
      T &bottom() { return bottom_; }
      T bottom() const { return bottom_; }
      T &left() { return left_; }
      T left() const { return left_; }

    public:
      friend std::ostream &operator<<(std::ostream &os, const Rect<T> &style)
      {
        os << "(" << style.top_ << ", " << style.right_ << ", " << style.bottom_ << ", " << style.left_ << ")";
        return os;
      }

    private:
      T top_;
      T right_;
      T bottom_;
      T left_;
    };

    namespace style
    {
      using Display = _Display;
      using Overflow = _Overflow;
      using Position = _Position;

      inline std::ostream &operator<<(std::ostream &os, Display display)
      {
        switch (display)
        {
        case Display::Block:
          os << "Block";
          break;
        case Display::Flex:
          os << "Flex";
          break;
        case Display::Grid:
          os << "Grid";
          break;
        case Display::None:
          os << "None";
          break;
        }
        return os;
      }

      inline std::ostream &operator<<(std::ostream &os, Overflow overflow)
      {
        switch (overflow)
        {
        case Overflow::Visible:
          os << "Visible";
          break;
        case Overflow::Clip:
          os << "Clip";
          break;
        case Overflow::Hidden:
          os << "Hidden";
          break;
        case Overflow::Scroll:
          os << "Scroll";
          break;
        }
        return os;
      }

      inline std::ostream &operator<<(std::ostream &os, Position position)
      {
        switch (position)
        {
        case Position::Relative:
          os << "Relative";
          break;
        case Position::Absolute:
          os << "Absolute";
          break;
        }
        return os;
      }

      template <typename T, typename InnerType>
      class DimensionOrLengthPercentageAuto
      {
        friend class LayoutStyle;

      public:
        /**
         * A length dimension.
         *
         * @param value The length value.
         * @returns The length dimension.
         */
        inline static T Length(float value) { return T(InnerType::Length(value)); }
        /**
         * A percent dimension.
         *
         * @param value The percent value between 0 and 1.
         * @returns The percent dimension.
         */
        inline static T Percent(float value) { return T(InnerType::Percent(value)); }
        /**
         * An auto dimension without value.
         *
         * @returns The auto dimension.
         */
        inline static T Auto() { return T(InnerType::Auto()); }

      public:
        DimensionOrLengthPercentageAuto(const std::string &input = "")
        {
          if (input.ends_with("%"))
          {
            auto value = std::stof(input.substr(0, input.size() - 1)) / 100.0f;
            handle_ = InnerType::Percent(value);
          }
          else if (input.ends_with("px"))
          {
            auto value = std::stof(input.substr(0, input.size() - 2));
            handle_ = InnerType::Length(value);
          }
          else
          {
            if (input != "" && input != "auto") // Print error message if the input is not empty or "auto".
              std::cerr << "Switched to \"auto\", unknown Dimension or LengthPercentageAuto value: " << input << std::endl;
            handle_ = InnerType::Auto();
          }
        }
        DimensionOrLengthPercentageAuto(InnerType handle) : handle_(handle)
        {
        }

      public:
        inline bool isLength() { return handle_.IsLength(); }
        inline bool isPercent() { return handle_.IsPercent(); }
        inline bool isAuto() { return handle_.IsAuto(); }
        inline float value()
        {
          if (isLength())
            return handle_.length._0;
          else if (isPercent())
            return handle_.percent._0;
          else
            return 0.0f;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const T &value)
        {
          if (value.handle_.IsLength())
            os << "Length(" << value.handle_.length._0 << ")";
          else if (value.handle_.IsPercent())
            os << "Percent(" << value.handle_.percent._0 << ")";
          else
            os << "Auto()";
          return os;
        }

      private:
        InnerType handle_;
      };

      class Dimension : public DimensionOrLengthPercentageAuto<Dimension, _Dimension>
      {
        friend class LayoutStyle;

      public:
        static inline Dimension Length(float value) { return DimensionOrLengthPercentageAuto::Length(value); }
        static inline Dimension Percent(float value) { return DimensionOrLengthPercentageAuto::Percent(value); }
        static inline Dimension Auto() { return DimensionOrLengthPercentageAuto::Auto(); }

      public:
        using DimensionOrLengthPercentageAuto<Dimension, _Dimension>::DimensionOrLengthPercentageAuto;
      };

      class LengthPercentageAuto : public DimensionOrLengthPercentageAuto<LengthPercentageAuto, _LengthPercentageAuto>
      {
        friend class LayoutStyle;

      public:
        static inline LengthPercentageAuto Length(float value) { return DimensionOrLengthPercentageAuto::Length(value); }
        static inline LengthPercentageAuto Percent(float value) { return DimensionOrLengthPercentageAuto::Percent(value); }
        static inline LengthPercentageAuto Auto() { return DimensionOrLengthPercentageAuto::Auto(); }

      public:
        using DimensionOrLengthPercentageAuto<LengthPercentageAuto, _LengthPercentageAuto>::DimensionOrLengthPercentageAuto;
      };

      class LengthPercentage
      {
        friend class LayoutStyle;

      public:
        /**
         * A length dimension.
         *
         * @param value The length value.
         * @returns The length dimension.
         */
        static inline LengthPercentage Length(float value) { return LengthPercentage(_LengthPercentage::Length(value)); }
        /**
         * A percent dimension.
         *
         * @param value The percent value between 0 and 1.
         * @returns The percent dimension.
         */
        static inline LengthPercentage Percent(float value) { return LengthPercentage(_LengthPercentage::Percent(value)); }

      public:
        LengthPercentage(_LengthPercentage handle) : handle_(handle) {}

      public:
        friend std::ostream &operator<<(std::ostream &os, const LengthPercentage &value)
        {
          if (value.handle_.IsLength())
            os << "Length(" << value.handle_.length._0 << ")";
          else if (value.handle_.IsPercent())
            os << "Percent(" << value.handle_.percent._0 << ")";
          return os;
        }

      private:
        _LengthPercentage handle_;
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
            : style_({
                  _Display::Block,                     // display
                  _Overflow::Visible,                  // overflow_x
                  _Overflow::Visible,                  // overflow_y
                  2.0f,                                // scrollbar_width
                  _Position::Relative,                 // position
                  _Dimension::Auto(),                  // width
                  _Dimension::Auto(),                  // height
                  _LengthPercentageAuto::Length(0.0f), // margin_top
                  _LengthPercentageAuto::Length(0.0f), // margin_right
                  _LengthPercentageAuto::Length(0.0f), // margin_bottom
                  _LengthPercentageAuto::Length(0.0f), // margin_left
                  _LengthPercentage::Length(0.0f),     // padding_top
                  _LengthPercentage::Length(0.0f),     // padding_right
                  _LengthPercentage::Length(0.0f),     // padding_bottom
                  _LengthPercentage::Length(0.0f),     // padding_left
                  0.0f,                                // flex_grow
                  1.0f                                 // flex_shrink
              })
        {
        }
        /**
         * Create a new layout style from a `TaffyStyle`.
         *
         * @param sourceStyle The source style to copy.
         */
        LayoutStyle(_LayoutStyle sourceStyle)
            : style_(sourceStyle)
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const LayoutStyle &style)
        {
          os << "LayoutStyle {" << std::endl;
          os << " display: " << style.display() << "," << std::endl;
          os << " overflowX: " << style.overflow().x() << "," << std::endl;
          os << " overflowY: " << style.overflow().y() << "," << std::endl;
          os << " scrollbarWidth: " << style.scrollbarWidth() << "," << std::endl;
          os << " position: " << style.position() << "," << std::endl;
          os << " width: " << style.width() << "," << std::endl;
          os << " height: " << style.height() << "," << std::endl;
          os << " margin: " << style.margin() << "," << std::endl;
          os << " padding: " << style.padding() << "," << std::endl;
          os << " flexGrow: " << style.flexGrow() << "," << std::endl;
          os << " flexShrink: " << style.flexShrink() << " }" << std::endl;
          return os;
        }
        operator _LayoutStyle()
        {
          return style_;
        }

      public:
        // Display property
        Display display() const { return style_.display; }
        void setDisplay(Display value) { style_.display = value; }
        // Overflow property
        const Point<Overflow> overflow() const { return Point<Overflow>(style_.overflow_x, style_.overflow_y); }
        void setOverflowX(Overflow value) { style_.overflow_x = value; }
        void setOverflowY(Overflow value) { style_.overflow_y = value; }
        // Scrollbar width property
        float scrollbarWidth() const { return style_.scrollbar_width; }
        void setScrollbarWidth(float value) { style_.scrollbar_width = value; }
        // Position property
        Position position() const { return style_.position; }
        void setPosition(Position value) { style_.position = value; }
        // Width and height properties
        const Dimension width() const { return Dimension(style_.width); }
        void setWidth(Dimension value) { style_.width = value.handle_; }
        Dimension height() const { return Dimension(style_.height); }
        void setHeight(Dimension value) { style_.height = value.handle_; }
        // Spacing properties
        const Rect<LengthPercentageAuto> margin() const
        {
          return Rect<LengthPercentageAuto>(LengthPercentageAuto(style_.margin_top),
                                            LengthPercentageAuto(style_.margin_right),
                                            LengthPercentageAuto(style_.margin_bottom),
                                            LengthPercentageAuto(style_.margin_left));
        }
        void setMarginTop(LengthPercentageAuto value) { style_.margin_top = value.handle_; }
        void setMarginRight(LengthPercentageAuto value) { style_.margin_right = value.handle_; }
        void setMarginBottom(LengthPercentageAuto value) { style_.margin_bottom = value.handle_; }
        void setMarginLeft(LengthPercentageAuto value) { style_.margin_left = value.handle_; }
        const Rect<LengthPercentage> padding() const
        {
          return Rect<LengthPercentage>(LengthPercentage(style_.padding_top),
                                        LengthPercentage(style_.padding_right),
                                        LengthPercentage(style_.padding_bottom),
                                        LengthPercentage(style_.padding_left));
        }
        void setPaddingTop(LengthPercentage value) { style_.padding_top = value.handle_; }
        void setPaddingRight(LengthPercentage value) { style_.padding_right = value.handle_; }
        void setPaddingBottom(LengthPercentage value) { style_.padding_bottom = value.handle_; }
        void setPaddingLeft(LengthPercentage value) { style_.padding_left = value.handle_; }
        // Flex properties
        const float flexGrow() const { return style_.flex_grow; }
        void setFlexGrow(float value) { style_.flex_grow = value; }
        const float flexShrink() const { return style_.flex_shrink; }
        void setFlexShrink(float value) { style_.flex_shrink = value; }

      private:
        _LayoutStyle style_;
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
      _TaffyTree *tree_;
    };

    /**
     * The layout result of a given node.
     */
    class Layout
    {
      friend class Node;

    private:
      Layout(_LayoutOutput output) : data_(output)
      {
      }

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
        os << " y: " << layout.y() << ", " << std::endl;
        os << " border: " << layout.border() << "," << std::endl;
        os << " padding: " << layout.padding() << "}" << std::endl;
        os << std::endl;
        return os;
      }

    private:
      _LayoutOutput data_;
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
      inline Layout layout()
      {
        return Layout(taffy_node_get_layout(node_));
      }

    private:
      _TaffyNode *node_;
    };
  }
}
