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
       * @returns The property name at the given index.
       */
      std::string item(size_t index) const
      {
        const char *str = css_property_declaration_block_item(handle_, index);
        std::string itemStr(str);
        release_rust_cstring((char *)str);
        return itemStr;
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

    /**
     * Parse a font family string into a vector of font families.
     *
     * @param inputStr The font family string.
     * @returns The vector of font families.
     */
    inline std::vector<std::string> parseFontFamily(const std::string &inputStr)
    {
      char **fontFamilies = parse_font_family(inputStr.c_str());
      std::vector<std::string> families;
      for (size_t i = 0; fontFamilies[i] != nullptr; i++)
        families.push_back(fontFamilies[i]);
      release_rust_cstrings(fontFamilies);
      return families;
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
      template <typename T, T defaultValue>
        requires std::is_enum_v<T>
      class KeywordWrap
      {
        friend class LayoutStyle;

      public:
        KeywordWrap(T handle = defaultValue) : handle_(handle)
        {
        }
        ~KeywordWrap() = default;

      public:
        operator T() const { return handle_; }

      protected:
        virtual std::optional<T> parse(const std::string &input) = 0;

      protected:
        T handle_;
      };

#define DISPLAY_MAP(XX) \
  XX(Block, "block")    \
  XX(Flex, "flex")      \
  XX(Grid, "grid")      \
  XX(None, "none")

#define BOX_SIZING_MAP(XX)      \
  XX(ContentBox, "content-box") \
  XX(BorderBox, "border-box")

#define OVERFLOW_MAP(XX) \
  XX(Visible, "visible") \
  XX(Hidden, "hidden")   \
  XX(Scroll, "scroll")   \
  XX(Clip, "clip")

#define POSITION_MAP(XX)   \
  XX(Relative, "relative") \
  XX(Absolute, "absolute")

#define ALIGN_ITEMS_MAP(XX)   \
  XX(Start, "start")          \
  XX(End, "end")              \
  XX(FlexStart, "flex-start") \
  XX(FlexEnd, "flex-end")     \
  XX(Center, "center")        \
  XX(Baseline, "baseline")    \
  XX(Stretch, "stretch")

#define ALIGN_SELF_MAP(XX) \
  ALIGN_ITEMS_MAP(XX)

#define JUSTIFY_ITEMS_MAP(XX) \
  ALIGN_ITEMS_MAP(XX)

#define JUSTIFY_SELF_MAP(XX) \
  ALIGN_ITEMS_MAP(XX)

#define ALIGN_CONTENT_MAP(XX)       \
  XX(Start, "start")                \
  XX(End, "end")                    \
  XX(FlexStart, "flex-start")       \
  XX(FlexEnd, "flex-end")           \
  XX(Center, "center")              \
  XX(Stretch, "stretch")            \
  XX(SpaceBetween, "space-between") \
  XX(SpaceEvenly, "space-evenly")   \
  XX(SpaceAround, "space-around")

#define JUSTIFY_CONTENT_MAP(XX) \
  ALIGN_CONTENT_MAP(XX)

#define FLEX_DIRECTION_MAP(XX)  \
  XX(Row, "row")                \
  XX(Column, "column")          \
  XX(RowReverse, "row-reverse") \
  XX(ColumnReverse, "column-reverse")

#define FLEX_WRAP_MAP(XX) \
  XX(NoWrap, "nowrap")    \
  XX(Wrap, "wrap")        \
  XX(WrapReverse, "wrap-reverse")

      class Display : public KeywordWrap<_Display, _Display::Block>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static Display Block() { return Display(_Display::Block); }
        static Display Flex() { return Display(_Display::Flex); }
        static Display Grid() { return Display(_Display::Grid); }
        static Display None() { return Display(_Display::None); }

      public:
        Display(const std::string &input)
        {
          handle_ = parse(input).value_or(_Display::Block);
        }

      private:
        std::optional<_Display> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _Display::tag;
          DISPLAY_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Display &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)  \
  case _Display::tag: \
    os << str;        \
    break;
            DISPLAY_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      class BoxSizing : public KeywordWrap<_BoxSizing, _BoxSizing::ContentBox>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static BoxSizing ContentBox() { return BoxSizing(_BoxSizing::ContentBox); }
        static BoxSizing BorderBox() { return BoxSizing(_BoxSizing::BorderBox); }

      public:
        BoxSizing(const std::string &input)
        {
          handle_ = parse(input).value_or(_BoxSizing::ContentBox);
        }

      private:
        std::optional<_BoxSizing> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _BoxSizing::tag;
          BOX_SIZING_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const BoxSizing &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)    \
  case _BoxSizing::tag: \
    os << str;          \
    break;
            BOX_SIZING_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      class Overflow : public KeywordWrap<_Overflow, _Overflow::Visible>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static Overflow Visible() { return Overflow(_Overflow::Visible); }
        static Overflow Hidden() { return Overflow(_Overflow::Hidden); }
        static Overflow Scroll() { return Overflow(_Overflow::Scroll); }
        static Overflow Clip() { return Overflow(_Overflow::Clip); }

      public:
        Overflow(const std::string &input)
        {
          handle_ = parse(input).value_or(_Overflow::Visible);
        }

      private:
        std::optional<_Overflow> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _Overflow::tag;
          OVERFLOW_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Overflow &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)   \
  case _Overflow::tag: \
    os << str;         \
    break;
            OVERFLOW_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      class Position : public KeywordWrap<_Position, _Position::Relative>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static Position Relative() { return Position(_Position::Relative); }
        static Position Absolute() { return Position(_Position::Absolute); }

      public:
        Position(const std::string &input)
        {
          handle_ = parse(input).value_or(_Position::Relative);
        }

      private:
        std::optional<_Position> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _Position::tag;
          POSITION_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Position &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)   \
  case _Position::tag: \
    os << str;         \
    break;
            POSITION_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      class AlignItems : public KeywordWrap<_AlignItems, _AlignItems::Stretch>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static AlignItems Start() { return AlignItems(_AlignItems::Start); }
        static AlignItems End() { return AlignItems(_AlignItems::End); }
        static AlignItems FlexStart() { return AlignItems(_AlignItems::FlexStart); }
        static AlignItems FlexEnd() { return AlignItems(_AlignItems::FlexEnd); }
        static AlignItems Center() { return AlignItems(_AlignItems::Center); }
        static AlignItems Baseline() { return AlignItems(_AlignItems::Baseline); }
        static AlignItems Stretch() { return AlignItems(_AlignItems::Stretch); }

      public:
        AlignItems(const std::string &input)
        {
          handle_ = parse(input).value_or(_AlignItems::Stretch);
        }

      private:
        std::optional<_AlignItems> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _AlignItems::tag;
          ALIGN_ITEMS_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const AlignItems &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)     \
  case _AlignItems::tag: \
    os << str;           \
    break;
            ALIGN_ITEMS_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      using AlignSelf = AlignItems;
      using JustifyItems = AlignItems;
      using JustifySelf = AlignItems;

      class AlignContent : public KeywordWrap<_AlignContent, _AlignContent::Stretch>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static AlignContent Start() { return AlignContent(_AlignContent::Start); }
        static AlignContent End() { return AlignContent(_AlignContent::End); }
        static AlignContent FlexStart() { return AlignContent(_AlignContent::FlexStart); }
        static AlignContent FlexEnd() { return AlignContent(_AlignContent::FlexEnd); }
        static AlignContent Center() { return AlignContent(_AlignContent::Center); }
        static AlignContent Stretch() { return AlignContent(_AlignContent::Stretch); }
        static AlignContent SpaceBetween() { return AlignContent(_AlignContent::SpaceBetween); }
        static AlignContent SpaceEvenly() { return AlignContent(_AlignContent::SpaceEvenly); }
        static AlignContent SpaceAround() { return AlignContent(_AlignContent::SpaceAround); }

      public:
        AlignContent(const std::string &input)
        {
          handle_ = parse(input).value_or(_AlignContent::Stretch);
        }

      private:
        std::optional<_AlignContent> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _AlignContent::tag;
          ALIGN_CONTENT_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const AlignContent &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)       \
  case _AlignContent::tag: \
    os << str;             \
    break;
            ALIGN_CONTENT_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      using JustifyContent = AlignContent;

      class FlexDirection : public KeywordWrap<_FlexDirection, _FlexDirection::Row>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static FlexDirection Row() { return FlexDirection(_FlexDirection::Row); }
        static FlexDirection Column() { return FlexDirection(_FlexDirection::Column); }
        static FlexDirection RowReverse() { return FlexDirection(_FlexDirection::RowReverse); }
        static FlexDirection ColumnReverse() { return FlexDirection(_FlexDirection::ColumnReverse); }

      public:
        FlexDirection(const std::string &input)
        {
          handle_ = parse(input).value_or(_FlexDirection::Row);
        }

      private:
        std::optional<_FlexDirection> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _FlexDirection::tag;
          FLEX_DIRECTION_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const FlexDirection &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)        \
  case _FlexDirection::tag: \
    os << str;              \
    break;
            FLEX_DIRECTION_MAP(XX)
#undef XX
          }
          return os;
        }
      };

      class FlexWrap : public KeywordWrap<_FlexWrap, _FlexWrap::NoWrap>
      {
        using KeywordWrap::KeywordWrap;

      public:
        static FlexWrap NoWrap() { return FlexWrap(_FlexWrap::NoWrap); }
        static FlexWrap Wrap() { return FlexWrap(_FlexWrap::Wrap); }
        static FlexWrap WrapReverse() { return FlexWrap(_FlexWrap::WrapReverse); }

      public:
        FlexWrap(const std::string &input)
        {
          handle_ = parse(input).value_or(_FlexWrap::NoWrap);
        }

      private:
        std::optional<_FlexWrap> parse(const std::string &input) override
        {
#define XX(tag, str) \
  if (input == str)  \
    return _FlexWrap::tag;
          FLEX_WRAP_MAP(XX)
#undef XX
          return std::nullopt;
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const FlexWrap &value)
        {
          switch (value.handle_)
          {
#define XX(tag, str)   \
  case _FlexWrap::tag: \
    os << str;         \
    break;
            FLEX_WRAP_MAP(XX)
#undef XX
          }
          return os;
        }
      };

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
        inline bool isLength() const { return handle_.IsLength(); }
        inline bool isPercent() const { return handle_.IsPercent(); }
        inline bool isAuto() const { return handle_.IsAuto(); }
        inline float value() const
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
          os << static_cast<std::string>(value);
          return os;
        }
        operator std::string() const
        {
          if (isLength())
            return std::to_string(handle_.length._0) + "px";
          else if (isPercent())
            return std::to_string(handle_.percent._0 * 100.0f) + "%";
          else
            return "auto";
        }
        T operator/(float value) const
        {
          if (isLength())
            return Length(handle_.length._0 / value);
          else if (isPercent())
            return Percent(handle_.percent._0 / value);
          else
            return Auto();
        }
        T operator*(float value) const
        {
          if (isLength())
            return Length(handle_.length._0 * value);
          else if (isPercent())
            return Percent(handle_.percent._0 * value);
          else
            return Auto();
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
        LengthPercentage(const std::string &input = "")
        {
          if (input.ends_with("%"))
          {
            auto value = std::stof(input.substr(0, input.size() - 1)) / 100.0f;
            handle_ = _LengthPercentage::Percent(value);
          }
          else if (input.ends_with("px"))
          {
            auto value = std::stof(input.substr(0, input.size() - 2));
            handle_ = _LengthPercentage::Length(value);
          }
          else
          {
            handle_ = _LengthPercentage::Length(0.0f);
          }
        }
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
                  _BoxSizing::ContentBox,              // box_sizing
                  _Overflow::Visible,                  // overflow_x
                  _Overflow::Visible,                  // overflow_y
                  2.0f,                                // scrollbar_width
                  _Position::Relative,                 // position
                  _Dimension::Auto(),                  // width
                  _Dimension::Auto(),                  // height
                  _Dimension::Auto(),                  // min_width
                  _Dimension::Auto(),                  // min_height
                  _Dimension::Auto(),                  // max_width
                  _Dimension::Auto(),                  // max_height
                  _LengthPercentageAuto::Length(0.0f), // margin_top
                  _LengthPercentageAuto::Length(0.0f), // margin_right
                  _LengthPercentageAuto::Length(0.0f), // margin_bottom
                  _LengthPercentageAuto::Length(0.0f), // margin_left
                  _LengthPercentage::Length(0.0f),     // padding_top
                  _LengthPercentage::Length(0.0f),     // padding_right
                  _LengthPercentage::Length(0.0f),     // padding_bottom
                  _LengthPercentage::Length(0.0f),     // padding_left
                  _LengthPercentage::Length(0.0f),     // border_top
                  _LengthPercentage::Length(0.0f),     // border_right
                  _LengthPercentage::Length(0.0f),     // border_bottom
                  _LengthPercentage::Length(0.0f),     // border_left
                  _AlignItems::Stretch,                // align_items
                  _AlignSelf::Stretch,                 // align_self
                  _JustifyItems::Stretch,              // justify_items
                  _JustifySelf::Stretch,               // justify_self
                  _AlignContent::Stretch,              // align_content
                  _JustifyContent::Stretch,            // justify_content
                  _LengthPercentage::Length(0.0f),     // gap_x
                  _LengthPercentage::Length(0.0f),     // gap_y
                  _FlexDirection::Row,                 // flex_direction
                  _FlexWrap::Wrap,                     // flex_wrap
                  _Dimension::Auto(),                  // flex_basis
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
          os << "    display: " << style.display() << "," << std::endl;
          os << "  boxSizing: " << style.boxSizing() << "," << std::endl;
          os << "  overflowX: " << style.overflow().x() << "," << std::endl;
          os << "  overflowY: " << style.overflow().y() << "," << std::endl;
          os << "   position: " << style.position() << "," << std::endl;
          os << "       size: " << "(" << style.width() << ", " << style.height() << ")," << std::endl;
          os << "  [min]size: " << "(" << style.minWidth() << ", " << style.minHeight() << ")," << std::endl;
          os << "  [max]size: " << "(" << style.maxWidth() << ", " << style.maxHeight() << ")," << std::endl;
          os << "     margin: " << style.margin() << "," << std::endl;
          os << "    padding: " << style.padding() << "," << std::endl;
          os << "}";
          return os;
        }
        operator _LayoutStyle() { return style_; }

      public:
        // Display property
        Display display() const { return style_.display; }
        void setDisplay(Display value) { style_.display = value; }

        // BoxSizing property
        BoxSizing boxSizing() const { return style_.box_sizing; }
        void setBoxSizing(BoxSizing value) { style_.box_sizing = value; }

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

        // Width & height properties
        Dimension width() const { return Dimension(style_.width); }
        void setWidth(const Dimension value) { style_.width = value.handle_; }
        Dimension height() const { return Dimension(style_.height); }
        void setHeight(const Dimension value) { style_.height = value.handle_; }

        // Min/Max width & height
        Dimension minWidth() const { return Dimension(style_.min_width); }
        void setMinWidth(const Dimension value) { style_.min_width = value.handle_; }
        Dimension minHeight() const { return Dimension(style_.min_height); }
        void setMinHeight(const Dimension value) { style_.min_height = value.handle_; }
        Dimension maxWidth() const { return Dimension(style_.max_width); }
        void setMaxWidth(const Dimension value) { style_.max_width = value.handle_; }
        Dimension maxHeight() const { return Dimension(style_.max_height); }
        void setMaxHeight(const Dimension value) { style_.max_height = value.handle_; }

        // Spacing properties
        Rect<LengthPercentageAuto> margin() const
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
        Rect<LengthPercentage> padding() const
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
        Rect<LengthPercentage> border() const
        {
          return Rect<LengthPercentage>(LengthPercentage(style_.border_top),
                                        LengthPercentage(style_.border_right),
                                        LengthPercentage(style_.border_bottom),
                                        LengthPercentage(style_.border_left));
        }
        void setBorderTop(LengthPercentage value) { style_.border_top = value.handle_; }
        void setBorderRight(LengthPercentage value) { style_.border_right = value.handle_; }
        void setBorderBottom(LengthPercentage value) { style_.border_bottom = value.handle_; }
        void setBorderLeft(LengthPercentage value) { style_.border_left = value.handle_; }

        // Flex properties
        AlignItems alignItems() const { return style_.align_items; }
        void setAlignItems(AlignItems value) { style_.align_items = value; }
        AlignSelf alignSelf() const { return style_.align_self; }
        void setAlignSelf(AlignSelf value) { style_.align_self = value; }
        JustifyItems justifyItems() const { return style_.justify_items; }
        void setJustifyItems(JustifyItems value) { style_.justify_items = value; }
        JustifySelf justifySelf() const { return style_.justify_self; }
        void setJustifySelf(JustifySelf value) { style_.justify_self = value; }
        AlignContent alignContent() const { return style_.align_content; }
        void setAlignContent(AlignContent value) { style_.align_content = value; }
        JustifyContent justifyContent() const { return style_.justify_content; }
        void setJustifyContent(JustifyContent value) { style_.justify_content = value; }
        LengthPercentage rowGap() const { return LengthPercentage(style_.gap_x); }
        void setRowGap(LengthPercentage value) { style_.gap_x = value.handle_; }
        LengthPercentage columnGap() const { return LengthPercentage(style_.gap_y); }
        void setColumnGap(LengthPercentage value) { style_.gap_y = value.handle_; }
        FlexDirection flexDirection() const { return style_.flex_direction; }
        void setFlexDirection(FlexDirection value) { style_.flex_direction = value; }
        FlexWrap flexWrap() const { return style_.flex_wrap; }
        void setFlexWrap(FlexWrap value) { style_.flex_wrap = value; }
        const Dimension flexBasis() const { return Dimension(style_.flex_basis); }
        void setFlexBasis(Dimension value) { style_.flex_basis = value.handle_; }
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

    public:
      Layout()
          : width_(0.0f),
            height_(0.0f),
            x_(0.0f),
            y_(0.0f),
            border_(0.0f, 0.0f, 0.0f, 0.0f),
            padding_(0.0f, 0.0f, 0.0f, 0.0f)
      {
      }

    private:
      Layout(_LayoutOutput output)
          : width_(output.width),
            height_(output.height),
            x_(output.x),
            y_(output.y),
            border_(output.border_top,
                    output.border_right,
                    output.border_bottom,
                    output.border_left),
            padding_(output.padding_top,
                     output.padding_right,
                     output.padding_bottom,
                     output.padding_left)
      {
      }

    public:
      /**
       * @returns The node width.
       */
      inline float width() const { return width_; }
      /**
       * @returns The node height.
       */
      inline float height() const { return height_; }
      /**
       * @returns The node x position.
       */
      inline float left() const { return x_; }
      /**
       * @returns The node y position.
       */
      inline float top() const { return y_; }
      /**
       * @returns The node border.
       */
      inline Rect<float> border() const { return border_; }
      /**
       * @returns The node padding.
       */
      inline Rect<float> padding() const { return padding_; }

    public:
      friend std::ostream &operator<<(std::ostream &os, const Layout &layout)
      {
        os << "Layout {" << std::endl;
        os << " width: " << layout.width() << "," << std::endl;
        os << " height: " << layout.height() << "," << std::endl;
        os << " left: " << layout.left() << "," << std::endl;
        os << " top: " << layout.top() << ", " << std::endl;
        os << " border: " << layout.border() << "," << std::endl;
        os << " padding: " << layout.padding() << "}" << std::endl;
        os << std::endl;
        return os;
      }

    protected:
      float width_;
      float height_;
      float x_;
      float y_;
      Rect<float> border_;
      Rect<float> padding_;
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
