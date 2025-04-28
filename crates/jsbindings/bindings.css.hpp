#pragma once

#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <optional>
#include <vector>
#include <variant>
#include <string>
#include <shared_mutex>
#include <atomic>
#include "./holocron_css_parser.autogen.hpp"

namespace crates::css2
{
  using namespace ::rust;

  namespace properties
  {
    class PropertyDeclarationBlock
    {
    public:
      /**
       * Parse a CSS style declaration string into a CSS property declaration block.
       *
       * @param styleStr The CSS style declaration string.
       * @returns The CSS property declaration block.
       */
      static inline std::shared_ptr<PropertyDeclarationBlock> ParseStyleDeclaration(const std::string &styleStr);

    private:
      struct PropertyValue
      {
        std::string value;
        bool important;
      };

    public:
      PropertyDeclarationBlock() = default;
      PropertyDeclarationBlock(holocron::css::properties::PropertyDeclarationBlock &inner)
      {
        using namespace holocron::css::properties;

        auto len = getPropertyDeclarationBlockLength(inner);
        for (size_t index = 0; index < len; index++)
        {
          auto name = getPropertyDeclarationBlockItemAt(inner, index);
          auto value = getPropertyDeclarationBlockProperty(inner, name.c_str());
          bool important = isPropertyDeclarationBlockPropertyImportant(inner, name.c_str());
          {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            std::string propertyName(name);
            properties_[propertyName] = {std::string(value), important};
            propertyIndices_.push_back(propertyName);
          }
        }
      }
      PropertyDeclarationBlock(const PropertyDeclarationBlock &other)
      {
        std::shared_lock<std::shared_mutex> lock(other.mutex_);
        properties_ = other.properties_;
        propertyIndices_ = other.propertyIndices_;
        cssText_ = other.cssText_;
        isCssTextExpired_ = other.isCssTextExpired_.load();
      }

    public:
      /**
       * Get the CSS text of the property declaration block.
       *
       * @returns The CSS text.
       */
      std::string cssText() const
      {
        if (isCssTextExpired_)
        {
          std::string cssText;
          for (size_t i = 0; i < size(); i++)
          {
            std::string propertyName = item(i);
            std::optional<PropertyValue> propertyObject = getPropertyObject(propertyName);
            if (!propertyObject.has_value())
              continue;

            std::string propertyValue = propertyObject->value;
            bool important = propertyObject->important;
            if (i == 0)
              cssText = propertyName + ": " + propertyValue + (important ? " !important" : "");
            else
              cssText += "; " + propertyName + ": " + propertyValue + (important ? " !important" : "");
          }
          cssText_ = cssText;
          isCssTextExpired_ = false;
        }
        return cssText_;
      }
      /**
       * @returns The length of the property declaration block.
       */
      inline size_t size() const
      {
        return propertyIndices_.size();
      }
      /**
       * @returns The property name at the given index.
       */
      std::string item(size_t index) const
      {
        if (index >= size())
          return "";
        return propertyIndices_[index];
      }
      /**
       * Check if a property is important.
       *
       * @param propertyName The property name.
       * @returns Whether the property is important.
       */
      bool isPropertyImportant(const std::string &propertyName) const
      {
        auto property = getPropertyObject(propertyName);
        return property.has_value() ? property->important : false;
      }
      /**
       * Get the property value.
       *
       * @param propertyName The property name.
       * @returns The property value in string.
       */
      std::string getProperty(const std::string &propertyName) const
      {
        auto property = getPropertyObject(propertyName);
        return property.has_value() ? property->value : "";
      }
      /**
       * Get the property object.
       * 
       * @param propertyName The property name.
       * @returns The property object.
       */
      const std::optional<PropertyValue> getPropertyObject(const std::string &propertyName) const
      {
        auto it = properties_.find(propertyName);
        if (it != properties_.end())
          return it->second;
        return std::nullopt;
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
        auto it = properties_.find(propertyName);
        if (it != properties_.end())
        {
          auto &prop = it->second;
          prop.value = value;
          prop.important = important;
        }
        else
        {
          std::unique_lock<std::shared_mutex> lock(mutex_);
          properties_[propertyName] = {value, important};
          propertyIndices_.push_back(propertyName);
        }
        isCssTextExpired_.store(true);
      }
      /**
       * Remove a property from the declaration block.
       *
       * @param propertyName The property name.
       * @returns The removed property value.
       */
      std::string removeProperty(const std::string &propertyName)
      {
        auto it = properties_.find(propertyName);
        if (it == properties_.end())
          return "";

        std::string value = it->second.value;
        properties_.erase(it);
        propertyIndices_.erase(std::remove(propertyIndices_.begin(), propertyIndices_.end(), propertyName),
                               propertyIndices_.end());
        isCssTextExpired_.store(true);
        return value;
      }

    private:
      std::unordered_map<std::string, PropertyValue> properties_;
      std::vector<std::string> propertyIndices_;
      mutable std::string cssText_;
      mutable std::atomic<bool> isCssTextExpired_ = true;
      mutable std::shared_mutex mutex_;
    };
  }

  namespace values
  {
    // Forward declarations.
    namespace specified
    {
      class Number;
      class Percentage;
      class NoCalcLength;
      class CalcLengthPercentage;
      class Length;
      class LengthPercentage;
    }

    namespace generics
    {
      template <typename T>
      class GenericMatrix
      {
      public:
        GenericMatrix() = default;
        GenericMatrix(T a, T b, T c, T d, T e, T f)
            : a(a), b(b), c(c), d(d), e(e), f(f)
        {
        }

        template <typename U = T>
        explicit GenericMatrix(const holocron::css::values::specified::NumberMatrix &inner,
                               typename std::enable_if<std::is_same<U, specified::Number>::value>::type * = nullptr)
            : a(holocron::css::values::specified::getNumberMatrixItem(inner, 0)),
              b(holocron::css::values::specified::getNumberMatrixItem(inner, 1)),
              c(holocron::css::values::specified::getNumberMatrixItem(inner, 2)),
              d(holocron::css::values::specified::getNumberMatrixItem(inner, 3)),
              e(holocron::css::values::specified::getNumberMatrixItem(inner, 4)),
              f(holocron::css::values::specified::getNumberMatrixItem(inner, 5))
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const GenericMatrix &matrix)
        {
          return os << "matrix("
                    << matrix.a << ", "
                    << matrix.b << ", "
                    << matrix.c << ", "
                    << matrix.d << ", "
                    << matrix.e << ", "
                    << matrix.f << ")";
        }

      public:
        T a;
        T b;
        T c;
        T d;
        T e;
        T f;
      };

      template <typename T>
      class GenericMatrix3D
      {
      public:
        GenericMatrix3D() = default;
        GenericMatrix3D(T m11, T m12, T m13, T m14,
                        T m21, T m22, T m23, T m24,
                        T m31, T m32, T m33, T m34,
                        T m41, T m42, T m43, T m44)
            : m11(m11), m12(m12), m13(m13), m14(m14),
              m21(m21), m22(m22), m23(m23), m24(m24),
              m31(m31), m32(m32), m33(m33), m34(m34),
              m41(m41), m42(m42), m43(m43), m44(m44)
        {
        }

        template <typename U = T>
        explicit GenericMatrix3D(const holocron::css::values::specified::NumberMatrix3D &inner,
                                 typename std::enable_if<std::is_same<U, specified::Number>::value>::type * = nullptr)
            : m11(holocron::css::values::specified::getNumberMatrix3DItem(inner, 1, 1)),
              m12(holocron::css::values::specified::getNumberMatrix3DItem(inner, 1, 2)),
              m13(holocron::css::values::specified::getNumberMatrix3DItem(inner, 1, 3)),
              m14(holocron::css::values::specified::getNumberMatrix3DItem(inner, 1, 4)),
              m21(holocron::css::values::specified::getNumberMatrix3DItem(inner, 2, 1)),
              m22(holocron::css::values::specified::getNumberMatrix3DItem(inner, 2, 2)),
              m23(holocron::css::values::specified::getNumberMatrix3DItem(inner, 2, 3)),
              m24(holocron::css::values::specified::getNumberMatrix3DItem(inner, 2, 4)),
              m31(holocron::css::values::specified::getNumberMatrix3DItem(inner, 3, 1)),
              m32(holocron::css::values::specified::getNumberMatrix3DItem(inner, 3, 2)),
              m33(holocron::css::values::specified::getNumberMatrix3DItem(inner, 3, 3)),
              m34(holocron::css::values::specified::getNumberMatrix3DItem(inner, 3, 4)),
              m41(holocron::css::values::specified::getNumberMatrix3DItem(inner, 4, 1)),
              m42(holocron::css::values::specified::getNumberMatrix3DItem(inner, 4, 2)),
              m43(holocron::css::values::specified::getNumberMatrix3DItem(inner, 4, 3)),
              m44(holocron::css::values::specified::getNumberMatrix3DItem(inner, 4, 4))
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const GenericMatrix3D &matrix)
        {
          return os << "matrix3d("
                    << matrix.m11 << ", "
                    << matrix.m12 << ", "
                    << matrix.m13 << ", "
                    << matrix.m14 << ", "
                    << matrix.m21 << ", "
                    << matrix.m22 << ", "
                    << matrix.m23 << ", "
                    << matrix.m24 << ", "
                    << matrix.m31 << ", "
                    << matrix.m32 << ", "
                    << matrix.m33 << ", "
                    << matrix.m34 << ", "
                    << matrix.m41 << ", "
                    << matrix.m42 << ", "
                    << matrix.m43 << ", "
                    << matrix.m44 << ")";
        }
        T operator[](size_t index) const
        {
          switch (index)
          {
          case 0:
            return m11;
          case 1:
            return m12;
          case 2:
            return m13;
          case 3:
            return m14;
          case 4:
            return m21;
          case 5:
            return m22;
          case 6:
            return m23;
          case 7:
            return m24;
          case 8:
            return m31;
          case 9:
            return m32;
          case 10:
            return m33;
          case 11:
            return m34;
          case 12:
            return m41;
          case 13:
            return m42;
          case 14:
            return m43;
          case 15:
            return m44;
          default:
            return 0;
          }
        }

      public:
        T m11;
        T m12;
        T m13;
        T m14;
        T m21;
        T m22;
        T m23;
        T m24;
        T m31;
        T m32;
        T m33;
        T m34;
        T m41;
        T m42;
        T m43;
        T m44;
      };
    }

    namespace specified
    {
      class Number
      {
      public:
        Number() = default;
        Number(const holocron::css::values::specified::Number &inner)
            : value(holocron::css::values::specified::getNumberValue(inner))
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Number &number)
        {
          return os << number.value;
        }
        operator float() const { return value; }

      public:
        float value;
      };

      class Percentage
      {
      public:
        Percentage() = default;
        Percentage(const holocron::css::values::specified::Percentage &inner)
            : value(holocron::css::values::specified::getPercentageValue(inner))
        {
        }
        Percentage(Box<holocron::css::values::specified::Percentage> innerBox)
            : Percentage(*innerBox)
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Percentage &percentage)
        {
          return os << (percentage.value * 100) << "%";
        }

      public:
        float value;
      };

      class NoCalcLength
      {
      public:
        NoCalcLength() : value(0), unit("px") {}
        NoCalcLength(const holocron::css::values::specified::NoCalcLength &inner)
            : value(holocron::css::values::specified::getNoCalcLengthUnitlessValue(inner)),
              unit(holocron::css::values::specified::getNoCalcLengthUnit(inner))
        {
        }
        NoCalcLength(Box<holocron::css::values::specified::NoCalcLength> innerBox)
            : NoCalcLength(*innerBox)
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const NoCalcLength &length)
        {
          return os << length.value << length.unit;
        }

      public:
        float value;
        std::string unit;
      };

      class CalcLengthPercentage
      {
      };

      class Length
      {
      public:
        Length() = default;
        Length(const holocron::css::values::specified::Length &inner)
        {
          using namespace holocron::css::values::specified;

          if (isLengthCalc(inner))
          {
            isCalc_ = true;
            value_ = CalcLengthPercentage();
          }
          else
          {
            isCalc_ = false;
            value_.emplace<NoCalcLength>(lengthToNoCalcLength(inner));
          }
        }
        Length(Box<holocron::css::values::specified::Length> innerBox)
            : Length(*innerBox)
        {
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const Length &length)
        {
          if (length.isCalc_)
          {
            assert(std::holds_alternative<CalcLengthPercentage>(length.value_));
            return os << "Calc()";
          }
          else
          {
            assert(std::holds_alternative<NoCalcLength>(length.value_));
            return os << std::get<NoCalcLength>(length.value_);
          }
        }

      public:
        bool isCalc() const { return isCalc_; }
        float numberValue() const
        {
          if (isCalc_)
            return 0;
          return std::get<NoCalcLength>(value_).value;
        }

      private:
        bool isCalc_;
        std::variant<NoCalcLength, CalcLengthPercentage> value_;
      };

      class LengthPercentage
      {
      public:
        LengthPercentage() = default;
        LengthPercentage(const holocron::css::values::specified::LengthPercentage &inner)
        {
          using namespace holocron::css::values::specified;

          auto tag = getLengthPercentageValueTag(inner);
          switch (tag)
          {
          case ValueTag::Calc:
            value_ = CalcLengthPercentage();
            break;
          case ValueTag::Length:
            value_ = lengthPercentageToNoCalcLength(inner);
            break;
          case ValueTag::Percentage:
            value_ = lengthPercentageToPercentage(inner);
            break;
          }
        }

      public:
        friend std::ostream &operator<<(std::ostream &os, const LengthPercentage &length)
        {
          if (std::holds_alternative<NoCalcLength>(length.value_))
            return os << std::get<NoCalcLength>(length.value_);
          else if (std::holds_alternative<Percentage>(length.value_))
            return os << std::get<Percentage>(length.value_);
          else
            return os << "Calc()";
        }

      public:
        /** TODO: Support percentage */
        float numberValue() const
        {
          if (std::holds_alternative<NoCalcLength>(value_))
            return std::get<NoCalcLength>(value_).value;
          return 0;
        }

      private:
        std::variant<NoCalcLength, Percentage, CalcLengthPercentage> value_;
      };

      class Color
      {
        using Inner = holocron::css::values::Color;

      public:
        Color() = default;
        Color(Inner inner) : inner_(inner) {}

      public:
        inline float r() const { return inner_.r; }
        inline float g() const { return inner_.g; }
        inline float b() const { return inner_.b; }
        inline float a() const { return inner_.a; }

      private:
        Inner inner_;
      };

      class FontFamily
      {
        using Inner = holocron::css::values::specified::FontFamily;

      private:
        static const std::vector<std::string> InitializeFonts(const Inner &inner)
        {
          using namespace holocron::css::values::specified;

          auto len = getFontFamilyCount(inner);
          std::vector<std::string> fonts;
          for (size_t i = 0; i < len; i++)
            fonts.push_back(std::string(getFontFamilyItem(inner, i)));
          return fonts;
        }

      public:
        FontFamily() = default;
        FontFamily(holocron::css::values::specified::FontFamily &inner)
            : fonts_(InitializeFonts(inner))
        {
        }

      public:
        const std::vector<std::string> fonts() const { return fonts_; }

      private:
        const std::vector<std::string> fonts_;
      };

      namespace transform
      {
        using TransformOperationType = holocron::css::values::specified::TransformOperationTag;

        class Translate
        {
        public:
          Translate(const holocron::css::values::specified::transform::Translate inner)
              : x(*inner.x),
                y(*inner.y)
          {
          }

        public:
          friend std::ostream &operator<<(std::ostream &os, const Translate &translate)
          {
            return os << "translate(" << translate.x << ", " << translate.y << ")";
          }

        public:
          specified::LengthPercentage x;
          specified::LengthPercentage y;
        };

        class Translate3D
        {
        public:
          Translate3D(const holocron::css::values::specified::transform::Translate3D inner)
              : x(*inner.x),
                y(*inner.y),
                z(*inner.z)
          {
          }

        public:
          friend std::ostream &operator<<(std::ostream &os, const Translate3D &translate)
          {
            return os << "translate3d(" << translate.x << ", " << translate.y << ", " << translate.z << ")";
          }

        public:
          specified::LengthPercentage x;
          specified::LengthPercentage y;
          specified::Length z;
        };

        class TransformOperation
        {
          using OperationImpl = std::variant<
              generics::GenericMatrix<Number>,
              generics::GenericMatrix3D<Number>,
              specified::Length,
              specified::LengthPercentage,
              Translate,
              Translate3D>;

        public:
          TransformOperation(const holocron::css::values::specified::TransformOperation &inner)
              : type_(holocron::css::values::specified::getTransformOperationType(inner))
          {
            using namespace holocron::css::values::specified;
            switch (type_)
            {
            case TransformOperationTag::kTranslate:
              this->impl_ = tryGetTransformOperationAsTranslate(inner);
              break;
            case TransformOperationTag::kTranslateX:
              this->impl_ = tryGetTransformOperationAsTranslateX(inner);
              break;
            case TransformOperationTag::kTranslateY:
              this->impl_ = tryGetTransformOperationAsTranslateY(inner);
              break;
            case TransformOperationTag::kTranslateZ:
              this->impl_ = tryGetTransformOperationAsTranslateZ(inner);
              break;
            case TransformOperationTag::kTranslate3D:
              this->impl_ = tryGetTransformOperationAsTranslate3D(inner);
              break;
            case TransformOperationTag::kMatrix:
              this->impl_ = generics::GenericMatrix<Number>(tryGetTransformOperationAsMatrix(inner));
              break;
            case TransformOperationTag::kMatrix3D:
              this->impl_ = generics::GenericMatrix3D<Number>(tryGetTransformOperationAsMatrix3D(inner));
              break;
            default:
              break;
            }
          }
          TransformOperation(Box<holocron::css::values::specified::TransformOperation> innerBox)
              : TransformOperation(*innerBox)
          {
          }

        public:
          friend std::ostream &operator<<(std::ostream &os, const TransformOperation &operation)
          {
            using namespace holocron::css::values::specified;
            switch (operation.type_)
            {
            case TransformOperationTag::kTranslate:
              os << operation.getImplAs<Translate>();
              break;
            case TransformOperationTag::kTranslateX:
              assert(std::holds_alternative<specified::LengthPercentage>(operation.impl_));
              os << "translateX(" << operation.getImplAs<specified::LengthPercentage>() << ")";
              break;
            case TransformOperationTag::kTranslateY:
              assert(std::holds_alternative<specified::LengthPercentage>(operation.impl_));
              os << "translateY(" << operation.getImplAs<specified::LengthPercentage>() << ")";
              break;
            case TransformOperationTag::kTranslateZ:
              os << "translateZ(" << operation.getImplAs<specified::Length>() << ")";
              break;
            case TransformOperationTag::kTranslate3D:
              os << operation.getImplAs<Translate3D>();
              break;
            case TransformOperationTag::kScale:
              os << "scale()";
              break;
            case TransformOperationTag::kScaleX:
              os << "scaleX()";
              break;
            case TransformOperationTag::kScaleY:
              os << "scaleY()";
              break;
            case TransformOperationTag::kScaleZ:
              os << "scaleZ()";
              break;
            case TransformOperationTag::kRotate:
              os << "rotate()";
              break;
            case TransformOperationTag::kRotateX:
              os << "rotateX()";
              break;
            case TransformOperationTag::kRotateY:
              os << "rotateY()";
              break;
            case TransformOperationTag::kRotateZ:
              os << "rotateZ()";
              break;
            case TransformOperationTag::kSkew:
              os << "skew()";
              break;
            case TransformOperationTag::kSkewX:
              os << "skewX()";
              break;
            case TransformOperationTag::kSkewY:
              os << "skewY()";
              break;
            case TransformOperationTag::kMatrix:
              os << operation.getImplAs<generics::GenericMatrix<Number>>();
              break;
            case TransformOperationTag::kMatrix3D:
              os << operation.getImplAs<generics::GenericMatrix3D<Number>>();
              break;
            default:
              os << "undefined()";
              break;
            }
            return os;
          }

        public:
          const TransformOperationType type() const { return type_; }

          /**
           * Get the operation implementation as a specific type.
           */
          template <typename T>
          inline const T &getImplAs() const
          {
            assert(std::holds_alternative<T>(impl_));
            return std::get<T>(impl_);
          }

        private:
          TransformOperationType type_;
          OperationImpl impl_;
        };

        class Transform
        {
        public:
          Transform(holocron::css::values::specified::Transform &inner)
          {
            using namespace holocron::css::values::specified;

            auto len = getTransformCount(inner);
            for (size_t index = 0; index < len; index++)
              operations_.push_back(TransformOperation(getTransformOperationAt(inner, index)));
          }

        public:
          const std::vector<TransformOperation> &operations() const { return operations_; }

        private:
          std::vector<TransformOperation> operations_;
        };
      }

      namespace grid
      {
        class GridTemplateComponent
        {
        public:
          GridTemplateComponent(Box<holocron::css::values::specified::GridTemplateComponent> inner)
              : inner_(std::move(inner))
          {
          }

        private:
          Box<holocron::css::values::specified::GridTemplateComponent> inner_;
        };
      }
    }

  }

  namespace selectors
  {
    using Combinator = holocron::css::selectors::Combinator;
    using ComponentType = holocron::css::selectors::ComponentType;
    using PseudoClassType = holocron::css::selectors::PseudoClassType;

    inline std::string to_string(const Combinator &combinator)
    {
      switch (combinator)
      {
      case Combinator::kDescendant:
        return " ";
      case Combinator::kChild:
        return " > ";
      case Combinator::kNextSibling:
        return " + ";
      case Combinator::kLaterSibling:
        return " ~ ";
      case Combinator::kPseudoElement:
        return "::";
      case Combinator::kSlotAssignment:
        return " / ";
      default:
        return " ? "; // Represents an unknown combinator
      }
    }

    inline std::string to_string(const PseudoClassType &pseudo_class_type)
    {
      switch (pseudo_class_type)
      {
      case PseudoClassType::kHover:
        return ":hover";
      case PseudoClassType::kActive:
        return ":active";
      case PseudoClassType::kFocus:
        return ":focus";
      case PseudoClassType::kFocusVisible:
        return ":focus-visible";
      case PseudoClassType::kFocusWithin:
        return ":focus-within";
      default:
        return "";
      }
    }

    inline std::ostream &operator<<(std::ostream &os, const Combinator &combinator)
    {
      os << to_string(combinator);
      return os;
    }

    class Component
    {
    public:
      Component(const holocron::css::selectors::Component &inner)
          : type(holocron::css::selectors::getComponentType(inner)),
            combinator(holocron::css::selectors::getComponentCombinator(inner)),
            name_(holocron::css::selectors::tryGetComponentName(inner)),
            pseudo_class_type(holocron::css::selectors::getComponentPseudoClassType(inner))
      {
      }

    public:
      operator std::string() const
      {
        std::string str;
        switch (type)
        {
        case ComponentType::kLocalName:
          str = name();
          break;
        case ComponentType::kID:
          str = "#" + name();
          break;
        case ComponentType::kClass:
          str = "." + name();
          break;
        case ComponentType::kRoot:
          str = ":root";
          break;
        case ComponentType::kEmpty:
          str = ":empty";
          break;
        case ComponentType::kHost:
          str = ":host";
          break;
        case ComponentType::kPseudoElement:
          str = "::" + name();
          break;
        case ComponentType::kPseudoClass:
          str = to_string(pseudo_class_type);
          break;
        case ComponentType::kCombinator:
          str = to_string(combinator);
          break;
        default:
          break;
        }
        return str;
      }
      friend std::ostream &operator<<(std::ostream &os, const Component &component)
      {
        return os << static_cast<std::string>(component);
      }

    public:
      inline bool isLocalName() const { return type == ComponentType::kLocalName; }
      inline bool isId() const { return type == ComponentType::kID; }
      inline bool isClass() const { return type == ComponentType::kClass; }
      inline bool isRoot() const { return type == ComponentType::kRoot; }
      inline bool isEmpty() const { return type == ComponentType::kEmpty; }
      inline bool isHost() const { return type == ComponentType::kHost; }
      inline bool isPseudoElement() const { return type == ComponentType::kPseudoElement; }
      inline bool isPseudoClass() const { return type == ComponentType::kPseudoClass; }
      inline bool isCombinator() const { return type == ComponentType::kCombinator; }

      inline const std::string &name() const { return name_; }
      inline const std::string &id() const
      {
        assert(isId());
        return name_;
      }
      inline const std::string &className() const
      {
        assert(isClass());
        return name_;
      }

      // Pseudo-class subtype check
      inline bool isHover() const { return isPseudoClass() && pseudo_class_type == PseudoClassType::kHover; }
      inline bool isActive() const { return isPseudoClass() && pseudo_class_type == PseudoClassType::kActive; }
      inline bool isFocus() const { return isPseudoClass() && pseudo_class_type == PseudoClassType::kFocus; }

    public:
      const ComponentType type;
      const Combinator combinator;

    private:
      std::string name_;
      PseudoClassType pseudo_class_type = PseudoClassType::kUnknown;
    };

    class Selector
    {
    public:
      Selector(const holocron::css::selectors::Selector &inner)
      {
        using namespace holocron::css::selectors;

        auto components = getSelectorComponents(inner);
        {
          ComponentList &list = *components;
          auto len = getComponentListLength(list);
          for (size_t index = 0; index < len; index++)
          {
            auto component = getComponentAt(list, index);
            components_.push_back(Component(*component));
          }
        }
      }

    public:
      operator std::string() const
      {
        std::string str;
        for (auto it = components_.rbegin(); it != components_.rend(); it++)
          str += static_cast<std::string>(*it);
        return str;
      }
      friend std::ostream &operator<<(std::ostream &os, const Selector &selector)
      {
        return os << static_cast<std::string>(selector);
      }

    public:
      const std::vector<Component> &components() const { return components_; }

    private:
      std::vector<Component> components_;
    };

    class SelectorList : std::vector<Selector>
    {
    public:
      SelectorList(holocron::css::selectors::SelectorList &inner)
      {
        using namespace holocron::css::selectors;

        auto len = getSelectorListLength(inner);
        for (size_t index = 0; index < len; index++)
        {
          auto selector = getSelectorAt(inner, index);
          push_back(Selector(*selector));
        }
      }

    public:
      auto begin() { return std::vector<Selector>::begin(); }
      auto end() { return std::vector<Selector>::end(); }
      auto begin() const { return std::vector<Selector>::begin(); }
      auto end() const { return std::vector<Selector>::end(); }
      operator std::string() const
      {
        std::string str;
        for (auto it = begin(); it != end(); it++)
        {
          if (it != begin())
            str += ", ";
          str += static_cast<std::string>(*it);
        }
        return str;
      }
      friend std::ostream &operator<<(std::ostream &os, const SelectorList &list)
      {
        return os << static_cast<std::string>(list);
      }
    };
  }

  namespace stylesheets
  {
    using CssRuleType = holocron::css::stylesheets::CssRuleType;

    class CssRule
    {
    public:
      CssRule(CssRuleType type) : type(type) {}
      CssRule(holocron::css::stylesheets::CssRule &inner)
      {
      }
      virtual ~CssRule() = default;

    public:
      const CssRuleType type = CssRuleType::kUnsupported;
    };

    class StyleRule : public CssRule
    {
    public:
      StyleRule(holocron::css::stylesheets::StyleRule &inner)
          : CssRule(CssRuleType::kStyle)
      {
        auto selectors = holocron::css::stylesheets::getStyleRuleSelectors(inner);
        selectors_ = std::make_unique<selectors::SelectorList>(*selectors);

        auto block = holocron::css::stylesheets::getStyleRuleBlock(inner);
        block_ = std::make_unique<properties::PropertyDeclarationBlock>(*block);
      }

    public:
      const selectors::SelectorList &selectors() const { return *selectors_; }
      const properties::PropertyDeclarationBlock &block() const { return *block_; }
      std::unique_ptr<properties::PropertyDeclarationBlock> takeBlock() { return std::move(block_); }

    private:
      std::unique_ptr<selectors::SelectorList> selectors_;
      std::unique_ptr<properties::PropertyDeclarationBlock> block_;
    };

    class MediaRule : public CssRule
    {
    public:
      MediaRule(holocron::css::stylesheets::MediaRule &inner)
          : CssRule(CssRuleType::kMedia)
      {
      }
    };

    class Stylesheet
    {
      using CssRuleReference = std::shared_ptr<CssRule>;

    private:
      static CssRuleReference MakeCssRule(holocron::css::stylesheets::CssRule &inner)
      {
        using namespace holocron::css::stylesheets;

        CssRuleType ruleType = getCssRuleType(inner);
        if (ruleType == CssRuleType::kStyle)
        {
          auto impl = getStyleRuleImpl(inner);
          return std::make_shared<StyleRule>(*impl);
        }
        else if (ruleType == CssRuleType::kMedia)
        {
          auto impl = getMediaRuleImpl(inner);
          return std::make_shared<MediaRule>(*impl);
        }
        else
          throw std::runtime_error("Unsupported CSS rule type: " + std::to_string(static_cast<int>(ruleType)));
      }

    public:
      Stylesheet() = default;
      Stylesheet(holocron::css::stylesheets::Stylesheet &inner)
      {
        using namespace holocron::css::stylesheets;

        auto len = getRulesLengthFromStylesheet(inner);
        for (size_t index = 0; index < len; index++)
        {
          try
          {
            auto rule = getCssRuleFromStylesheet(inner, index);
            rules_.push_back(MakeCssRule(*rule));
          }
          catch (const std::exception &e)
          {
            std::cerr << "Failed to add CssRule: " << e.what() << std::endl;
          }
        }
      }

    public:
      const std::vector<CssRuleReference> &rules() const { return rules_; }

    private:
      std::vector<CssRuleReference> rules_;
    };
  }

  namespace parsing
  {
    class CSSParser
    {
    public:
      static CSSParser &Default()
      {
        static CSSParser default_parser("about:blank");
        return default_parser;
      }

    public:
      CSSParser(const std::string url = "about:blank")
          : inner_(holocron::css::parsing::createCSSParser(url))
      {
      }

    public:
      stylesheets::Stylesheet parseStylesheet(const std::string cssText, const std::string media = "") const
      {
        return stylesheets::Stylesheet(*holocron::css::parsing::parseStylesheet(*inner_, cssText, media));
      }
      selectors::SelectorList parseSelectors(const std::string selectors) const
      {
        return selectors::SelectorList(*holocron::css::parsing::parseSelectors(*inner_, selectors));
      }
      properties::PropertyDeclarationBlock parseStyleDeclaration(const std::string str) const
      {
        return properties::PropertyDeclarationBlock(*holocron::css::parsing::parseStyleDeclaration(*inner_, str));
      }
      const values::specified::Color parseColor(const std::string str) const
      {
        return values::specified::Color(holocron::css::parsing::parseColor(*inner_, str));
      }
      const values::specified::FontFamily parseFontFamily(const std::string str) const
      {
        return values::specified::FontFamily(
            *holocron::css::parsing::parseFontFamily(*inner_, str));
      }
      const values::specified::transform::Transform parseTransform(const std::string str) const
      {
        return values::specified::transform::Transform(
            *holocron::css::parsing::parseTransform(*inner_, str));
      }
      const values::specified::grid::GridTemplateComponent parseGridTemplateComponent(const std::string str) const
      {
        return values::specified::grid::GridTemplateComponent(
            holocron::css::parsing::parseGridTemplate(*inner_, str));
      }

    private:
      Box<holocron::css::parsing::CSSParser> inner_;
    };

    inline std::optional<const selectors::SelectorList> parseSelectors(const std::string selectors)
    {
      try
      {
        return CSSParser::Default().parseSelectors(selectors);
      }
      catch (...)
      {
        return std::nullopt;
      }
    }

    inline const values::specified::Color parseColor(const std::string str)
    {
      return CSSParser::Default().parseColor(str);
    }

    inline const std::vector<std::string> parseFontFamily(const std::string str)
    {
      return CSSParser::Default().parseFontFamily(str).fonts();
    }

    inline const values::specified::transform::Transform parseTransform(const std::string str)
    {
      return CSSParser::Default().parseTransform(str);
    }
  }
}
