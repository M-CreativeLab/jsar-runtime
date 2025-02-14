use std::{cell::RefCell, rc::Rc};
use std::{ffi::*, mem, os::raw::c_char, ptr};

use cxx::{CxxString, CxxVector};
use selectors::parser::Selector;
use style::{
  context::QuirksMode,
  media_queries::MediaList,
  parser::{Parse, ParserContext as StyleParserContext},
  properties::{self as StyleProperties, parse_one_declaration_into, parse_style_attribute},
  selector_parser::SelectorImpl,
  servo_arc::Arc as StyleArc,
  shared_lock::{
    SharedRwLock as StyleSharedRwLock, SharedRwLockReadGuard as StyleSharedRwLockReadGuard,
  },
  stylesheets::{
    AllowImportRules, CssRule, CssRuleType, Origin, Stylesheet, StylesheetInDocument, UrlExtraData,
  },
  values::computed as StyleComputedValues,
  values::generics as StyleGenericsValues,
  values::specified as StyleSpecifiedValues,
};
use style_traits::{ParsingMode, ToCss};
use url::Url;

use super::{
  css::{
    properties as CrateProperties, selectors as CrateSelectors, stylesheets as CrateStylesheets,
    values::specified as CrateSpecifiedValues,
  },
  release_cstring,
};

#[no_mangle]
extern "C" fn parse_font_family(input_str: *const c_char) -> *mut *mut c_char {
  use StyleComputedValues::font::{GenericFontFamily, SingleFontFamily};
  use StyleSpecifiedValues::font::FontFamily;

  let font_family_str: &str = unsafe { CStr::from_ptr(input_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let mut input = cssparser::ParserInput::new(font_family_str);
  let mut parser = cssparser::Parser::new(&mut input);
  let url = Url::parse("about:blank").unwrap().into();
  let context = StyleParserContext::new(
    Origin::Author,
    &url,
    Some(CssRuleType::Style),
    ParsingMode::DEFAULT,
    QuirksMode::NoQuirks,
    Default::default(),
    None,
    None,
  );

  let mut fonts = Vec::new();
  FontFamily::parse(&context, &mut parser)
    .map(|font_family| match font_family {
      FontFamily::System(family_name) => {
        // Do nothing
      }
      FontFamily::Values(list) => {
        for font in list.iter() {
          match font {
            SingleFontFamily::FamilyName(family_name) => {
              fonts.push(family_name.name.to_string());
            }
            SingleFontFamily::Generic(generic) => match generic {
              GenericFontFamily::None => {
                fonts.push("none".to_string());
              }
              GenericFontFamily::Serif => {
                fonts.push("serif".to_string());
              }
              GenericFontFamily::SansSerif => {
                fonts.push("sans-serif".to_string());
              }
              GenericFontFamily::Monospace => {
                fonts.push("monospace".to_string());
              }
              GenericFontFamily::Cursive => {
                fonts.push("cursive".to_string());
              }
              GenericFontFamily::Fantasy => {
                fonts.push("fantasy".to_string());
              }
              GenericFontFamily::SystemUi => {
                fonts.push("system-ui".to_string());
              }
            },
          }
        }
      }
    })
    .unwrap_or_default();

  // Convert the fonts to C strings
  let c_strings: Vec<CString> = fonts
    .into_iter()
    .map(|s| CString::new(s).unwrap())
    .collect();

  let mut c_array: Vec<*mut c_char> = c_strings.into_iter().map(|cs| cs.into_raw()).collect();
  c_array.push(ptr::null_mut());

  let res = c_array.as_mut_ptr();
  mem::forget(c_array);
  res
}

pub struct CSSParser {
  url_data: UrlExtraData,
}

impl CSSParser {
  pub fn new_from_url(document_url: &str) -> Self {
    style_config::set_bool("layout.flexbox.enabled", true);
    style_config::set_bool("layout.grid.enabled", true);

    let url_data = Url::parse(document_url).unwrap().into();
    Self { url_data }
  }

  pub fn parse_stylesheet(&self, css_text: &str, media_query: &str) -> CrateStylesheet {
    let context = StyleParserContext::new(
      Origin::Author,
      &self.url_data,
      Some(CssRuleType::Style),
      ParsingMode::DEFAULT,
      QuirksMode::NoQuirks,
      Default::default(),
      None,
      None,
    );

    let mut media_query_input = cssparser::ParserInput::new(media_query); // Media query string
    let shared_lock = StyleSharedRwLock::new();
    let media_query = StyleArc::new(shared_lock.wrap(MediaList::parse(
      &context,
      &mut cssparser::Parser::new(&mut media_query_input),
    )));

    CrateStylesheet::new(
      Stylesheet::from_str(
        &css_text,
        self.url_data.clone(),
        Origin::Author,
        media_query,
        shared_lock.clone(),
        None,
        None,
        QuirksMode::NoQuirks,
        AllowImportRules::Yes,
      ),
      shared_lock,
    )
  }

  pub fn parse_style_declaration(
    &self,
    input_str: &str,
  ) -> CrateProperties::PropertyDeclarationBlock {
    CrateProperties::PropertyDeclarationBlock::new(&parse_style_attribute(
      input_str,
      &self.url_data,
      None,
      QuirksMode::NoQuirks,
      CssRuleType::Style,
    ))
  }

  pub fn parse_color(&self, input: &str) -> ffi::Color {
    let mut input = cssparser::ParserInput::new(&input);
    let mut parser = cssparser::Parser::new(&mut input);
    let context = StyleParserContext::new(
      Origin::Author,
      &self.url_data,
      Some(CssRuleType::Style),
      ParsingMode::DEFAULT,
      QuirksMode::NoQuirks,
      Default::default(),
      None,
      None,
    );
    match StyleSpecifiedValues::Color::parse_and_compute(&context, &mut parser, None) {
      Some(color) => color
        .as_absolute()
        .map(|color| {
          let rgba = color.into_srgb_legacy();
          ffi::Color {
            r: (rgba.components.0 * 255.0) as u32,
            g: (rgba.components.1 * 255.0) as u32,
            b: (rgba.components.2 * 255.0) as u32,
            a: (rgba.alpha * 255.0) as u32,
          }
        })
        .unwrap_or_else(|| ffi::Color::default()),
      None => ffi::Color::default(),
    }
  }

  pub fn parse_fontfamily(&self, input: &str) -> Vec<String> {
    use StyleComputedValues::font::{GenericFontFamily, SingleFontFamily};
    use StyleSpecifiedValues::font::FontFamily;

    let mut input = cssparser::ParserInput::new(&input);
    let mut parser = cssparser::Parser::new(&mut input);
    let context = StyleParserContext::new(
      Origin::Author,
      &self.url_data,
      Some(CssRuleType::Style),
      ParsingMode::DEFAULT,
      QuirksMode::NoQuirks,
      Default::default(),
      None,
      None,
    );

    let mut fonts: Vec<String> = Vec::new();
    FontFamily::parse(&context, &mut parser)
      .map(|font_family| match font_family {
        FontFamily::System(family_name) => {
          // Do nothing
        }
        FontFamily::Values(list) => {
          for font in list.iter() {
            match font {
              SingleFontFamily::FamilyName(family_name) => {
                fonts.push(family_name.name.to_string());
              }
              SingleFontFamily::Generic(generic) => {
                fonts.push(generic.to_css_string());
              }
            }
          }
        }
      })
      .unwrap_or_default();

    fonts
  }
}

impl Default for CSSParser {
  fn default() -> Self {
    Self::new_from_url("about:blank")
  }
}

struct SpecifiedFontFamilyValue {
  fonts_list: Vec<String>,
}

impl SpecifiedFontFamilyValue {
  pub fn new(fonts_list: Vec<String>) -> Self {
    Self { fonts_list }
  }
  pub fn count(&self) -> usize {
    self.fonts_list.len()
  }
  pub fn item(&self, index: usize) -> String {
    self
      .fonts_list
      .get(index)
      .unwrap_or(&String::new())
      .to_string()
  }
}

// Specified base types
type SpecifiedNumberValue = CrateSpecifiedValues::Number;
type SpecifiedAngleValue = CrateSpecifiedValues::Angle;
type SpecifiedPercentageValue = CrateSpecifiedValues::Percentage;
type SpecifiedNoCalcLength = CrateSpecifiedValues::NoCalcLength;
type SpecifiedLength = CrateSpecifiedValues::Length;
type SpecifiedLengthPercentage = CrateSpecifiedValues::LengthPercentage;

// Specified matrix types
type SpecifiedNumberMatrix = CrateSpecifiedValues::NumberMatrix;
type SpecifiedNumberMatrix3D = CrateSpecifiedValues::NumberMatrix3D;

// Selector types
type PrismComponent = CrateSelectors::Component;
type PrismComponentList = CrateSelectors::ComponentList;
type PrismSelector = CrateSelectors::Selector;
type PrismSelectorList = CrateSelectors::SelectorList;

// Stylesheet types
type CrateStylesheet = CrateStylesheets::Stylesheet;
type CrateCssRule = CrateStylesheets::CssRule;
type CrateStyleRule = CrateStylesheets::StyleRule;
type CrateMediaRule = CrateStylesheets::MediaRule;

// Style property types
type PropertyDeclarationBlock = CrateProperties::PropertyDeclarationBlock;
type SpecifiedTransformValue = CrateSpecifiedValues::Transform;
type SpecifiedTransformOperation = CrateSpecifiedValues::TransformOperation;

#[cxx::bridge(namespace = "holocron::css")]
pub(crate) mod ffi {

  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::values"]
  struct Color {
    pub r: u32,
    pub g: u32,
    pub b: u32,
    pub a: u32,
  }

  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::values::specified"]
  enum AllowedNumericType {
    /// Allow all kind of numeric values.
    All,
    /// Allow only non-negative numeric values.
    NonNegative,
    /// Allow only numeric values greater or equal to 1.0.
    AtLeastOne,
    /// Allow only numeric values from 0 to 1.0.
    ZeroToOne,
  }

  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::values::specified"]
  #[cxx_name = "ValueTag"]
  enum SpecifiedValueTag {
    Calc,
    Length,
    Percentage,
  }

  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::values::specified"]
  #[cxx_name = "TransformOperationTag"]
  enum SpecifiedTransformOperationTag {
    #[cxx_name = "kInvalid"]
    Invalid,
    #[cxx_name = "kMatrix"]
    Matrix,
    #[cxx_name = "kMatrix3D"]
    Matrix3D,
    #[cxx_name = "kSkew"]
    Skew,
    #[cxx_name = "kSkewX"]
    SkewX,
    #[cxx_name = "kSkewY"]
    SkewY,
    #[cxx_name = "kTranslate"]
    Translate,
    #[cxx_name = "kTranslateX"]
    TranslateX,
    #[cxx_name = "kTranslateY"]
    TranslateY,
    #[cxx_name = "kTranslateZ"]
    TranslateZ,
    #[cxx_name = "kTranslate3D"]
    Translate3D,
    #[cxx_name = "kScale"]
    Scale,
    #[cxx_name = "kScaleX"]
    ScaleX,
    #[cxx_name = "kScaleY"]
    ScaleY,
    #[cxx_name = "kScaleZ"]
    ScaleZ,
    #[cxx_name = "kScale3D"]
    Scale3D,
    #[cxx_name = "kRotate"]
    Rotate,
    #[cxx_name = "kRotateX"]
    RotateX,
    #[cxx_name = "kRotateY"]
    RotateY,
    #[cxx_name = "kRotateZ"]
    RotateZ,
    #[cxx_name = "kRotate3D"]
    Rotate3D,
  }

  /// Represents a 2D translation transform in FFI context.
  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::values::specified::transform"]
  #[cxx_name = "Translate"]
  struct SpecifiedTransformTranslate {
    pub x: Box<SpecifiedLengthPercentage>,
    pub y: Box<SpecifiedLengthPercentage>,
  }

  /// Represents a 3D translation transform in FFI context.
  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::values::specified::transform"]
  #[cxx_name = "Translate3D"]
  struct SpecifiedTransformTranslate3D {
    pub x: Box<SpecifiedLengthPercentage>,
    pub y: Box<SpecifiedLengthPercentage>,
    pub z: Box<SpecifiedLength>,
  }

  /// The component type in a selector, such as:
  ///
  /// - LocalName: `div`
  /// - ID: `#id`
  /// - Class: `.class`
  /// - Root: `:root`
  /// - Empty: `:empty`
  /// - Scope: `:scope`
  /// - Host: `:host`
  /// - Combinator: `>`, `~`, `+`, `::`, `::part`, `::slotted`
  ///
  #[repr(u8)]
  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::selectors"]
  #[cxx_name = "ComponentType"]
  enum SelectorComponentType {
    #[cxx_name = "kLocalName"]
    LocalName,
    #[cxx_name = "kID"]
    ID,
    #[cxx_name = "kClass"]
    Class,
    #[cxx_name = "kRoot"]
    Root,
    #[cxx_name = "kEmpty"]
    Empty,
    #[cxx_name = "kScope"]
    Scope,
    #[cxx_name = "kHost"]
    Host,
    #[cxx_name = "kCombinator"]
    Combinator,
    #[cxx_name = "kUnsupported"]
    Unsupported,
  }

  /// The combinator type in a selector, such as:
  ///
  /// - Child: `>`
  /// - Descendant: ` `
  /// - NextSibling: `~`
  /// - LaterSibling: `+`
  /// - PseudoElement: `::`
  /// - SlotAssignment: `::slotted`
  /// - Part: `::part`
  ///
  #[repr(u8)]
  #[derive(Clone, Debug)]
  #[namespace = "holocron::css::selectors"]
  #[cxx_name = "Combinator"]
  enum SelectorComponentCombinator {
    #[cxx_name = "kChild"]
    Child,
    #[cxx_name = "kDescendant"]
    Descendant,
    #[cxx_name = "kNextSibling"]
    NextSibling,
    #[cxx_name = "kLaterSibling"]
    LaterSibling,
    #[cxx_name = "kPseudoElement"]
    PseudoElement,
    #[cxx_name = "kSlotAssignment"]
    SlotAssignment,
    #[cxx_name = "kPart"]
    Part,
    #[cxx_name = "kUnknown"]
    Unknown,
  }

  /// The type of CSS rule, such as Style, Media, Keyframes, Import and others.
  #[repr(u8)]
  #[namespace = "holocron::css::stylesheets"]
  enum CssRuleType {
    #[cxx_name = "kStyle"]
    Style,
    #[cxx_name = "kNamespace"]
    Namespace,
    #[cxx_name = "kImport"]
    Import,
    #[cxx_name = "kMedia"]
    Media,
    #[cxx_name = "kContainer"]
    Container,
    #[cxx_name = "kFontFace"]
    FontFace,
    #[cxx_name = "kUnsupported"]
    Unsupported,
  }

  #[namespace = "holocron::css::properties"]
  extern "Rust" {
    /// Represents a CSS property declaration block.
    type PropertyDeclarationBlock;

    /// Returns the number of declarations in the block.
    #[cxx_name = "getPropertyDeclarationBlockLength"]
    fn get_property_declaration_block_length(block: &PropertyDeclarationBlock) -> usize;

    /// Like the method on ToCss, but without the type parameter to avoid
    /// accidentally monomorphizing this large function multiple times for
    /// different writers.
    ///
    /// https://drafts.csswg.org/cssom/#serialize-a-css-declaration-block
    #[cxx_name = "propertyDeclarationBlockToCssString"]
    fn property_declaration_block_to_css_string(block: &PropertyDeclarationBlock) -> String;

    /// Returns a property name at the given index in the block.
    #[cxx_name = "getPropertyDeclarationBlockItemAt"]
    fn get_property_declaration_block_item_at_index(
      block: &PropertyDeclarationBlock,
      index: usize,
    ) -> String;

    /// Returns the value of the property with the given name.
    #[cxx_name = "getPropertyDeclarationBlockProperty"]
    fn get_property_declaration_block_property(
      block: &PropertyDeclarationBlock,
      property: &str,
    ) -> String;

    /// Sets the value of the property with the given name.
    #[cxx_name = "setPropertyDeclarationBlockProperty"]
    fn set_property_declaration_block_property(
      block: &mut PropertyDeclarationBlock,
      property: &str,
      value: &CxxString,
      important: bool,
    );

    /// Removes the property with the given name.
    #[cxx_name = "removePropertyDeclarationBlockProperty"]
    fn remove_property_declaration_block_property(
      block: &mut PropertyDeclarationBlock,
      property: &str,
    ) -> String;

    /// Returns true if the property is important, false otherwise.
    #[cxx_name = "isPropertyDeclarationBlockPropertyImportant"]
    fn is_property_declaration_block_property_important(
      block: &PropertyDeclarationBlock,
      property: &str,
    ) -> bool;
  }

  #[namespace = "holocron::css::values::specified"]
  extern "Rust" {
    #[cxx_name = "Number"]
    type SpecifiedNumberValue;
    #[cxx_name = "Angle"]
    type SpecifiedAngleValue;
    #[cxx_name = "Percentage"]
    type SpecifiedPercentageValue;
    #[cxx_name = "NoCalcLength"]
    type SpecifiedNoCalcLength;
    #[cxx_name = "Length"]
    type SpecifiedLength;
    #[cxx_name = "LengthPercentage"]
    type SpecifiedLengthPercentage;

    #[cxx_name = "getNumberValue"]
    fn get_number_value(value: &SpecifiedNumberValue) -> f32;

    #[cxx_name = "getPercentageValue"]
    fn get_percentage_value(value: &SpecifiedPercentageValue) -> f32;

    /// Returns the unitless value of a no-calc length.
    #[cxx_name = "getNoCalcLengthUnitlessValue"]
    fn get_no_calc_length_unitless_value(value: &SpecifiedNoCalcLength) -> f32;

    /// Returns the unit of a no-calc length.
    #[cxx_name = "getNoCalcLengthUnit"]
    fn get_no_calc_length_unit(value: &SpecifiedNoCalcLength) -> &str;

    /// Returns if the no-calc length is zero.
    #[cxx_name = "isNoCalcLengthZero"]
    fn is_no_calc_length_zero(value: &SpecifiedNoCalcLength) -> bool;

    /// Returns if the no-calc length is infinite.
    #[cxx_name = "isNoCalcLengthInfinite"]
    fn is_no_calc_length_infinite(value: &SpecifiedNoCalcLength) -> bool;

    /// Returns true if the specified length is a calc() value.
    #[cxx_name = "isLengthCalc"]
    fn is_length_calc(value: &SpecifiedLength) -> bool;

    /// Returns the value tag of this length.
    #[cxx_name = "getLengthValueTag"]
    fn get_length_value_tag(value: &SpecifiedLength) -> SpecifiedValueTag;

    /// Returns the no-calc length from the specified length.
    #[cxx_name = "lengthToNoCalcLength"]
    fn length_to_no_calc_length(value: &SpecifiedLength) -> Box<SpecifiedNoCalcLength>;

    /// Returns the value tag of this length percentage: calc, length or percentage.
    #[cxx_name = "getLengthPercentageValueTag"]
    fn get_length_percentage_value_tag(value: &SpecifiedLengthPercentage) -> SpecifiedValueTag;

    /// Returns the no-calc length from the specified length percentage.
    #[cxx_name = "lengthPercentageToNoCalcLength"]
    fn length_percentage_to_no_calc_length(
      value: &SpecifiedLengthPercentage,
    ) -> Box<SpecifiedNoCalcLength>;

    /// Returns the percentage value from the specified length percentage.
    #[cxx_name = "lengthPercentageToPercentage"]
    fn length_percentage_to_percentage(
      value: &SpecifiedLengthPercentage,
    ) -> Box<SpecifiedPercentageValue>;
  }

  #[namespace = "holocron::css::values::specified"]
  extern "Rust" {
    #[cxx_name = "NumberMatrix"]
    type SpecifiedNumberMatrix;
    #[cxx_name = "NumberMatrix3D"]
    type SpecifiedNumberMatrix3D;

    #[cxx_name = "FontFamily"]
    type SpecifiedFontFamilyValue;
    #[cxx_name = "Transform"]
    type SpecifiedTransformValue;
    #[cxx_name = "TransformOperation"]
    type SpecifiedTransformOperation;

    #[cxx_name = "getNumberMatrixItem"]
    fn get_number_matrix_item(
      matrix: &SpecifiedNumberMatrix,
      index: usize,
    ) -> &SpecifiedNumberValue;

    #[cxx_name = "getNumberMatrix3DItem"]
    fn get_number_matrix3d_item(
      matrix: &SpecifiedNumberMatrix3D,
      row: usize,
      col: usize,
    ) -> &SpecifiedNumberValue;

    #[cxx_name = "getFontFamilyCount"]
    fn get_fontfamily_count(font_family: &SpecifiedFontFamilyValue) -> usize;

    #[cxx_name = "getFontFamilyItem"]
    fn get_fontfamily_item(font_family: &SpecifiedFontFamilyValue, index: usize) -> String;

    #[cxx_name = "getTransformCount"]
    fn get_transform_count(transform: &SpecifiedTransformValue) -> usize;

    #[cxx_name = "getTransformOperationAt"]
    fn get_transform_operation_at_index(
      transform: &SpecifiedTransformValue,
      index: usize,
    ) -> Box<SpecifiedTransformOperation>;

    #[cxx_name = "getTransformOperationType"]
    fn get_transform_operation_type(
      transform_op: &SpecifiedTransformOperation,
    ) -> SpecifiedTransformOperationTag;

    #[cxx_name = "tryGetTransformOperationAsMatrix"]
    unsafe fn try_get_transform_operation_as_matrix<'a>(
      operation: &'a SpecifiedTransformOperation,
    ) -> Result<&'a SpecifiedNumberMatrix>;

    #[cxx_name = "tryGetTransformOperationAsMatrix3D"]
    unsafe fn try_get_transform_operation_as_matrix3d<'a>(
      operation: &'a SpecifiedTransformOperation,
    ) -> Result<&'a SpecifiedNumberMatrix3D>;

    #[cxx_name = "tryGetTransformOperationAsTranslate"]
    fn try_get_transform_operation_as_translate(
      operation: &SpecifiedTransformOperation,
    ) -> Result<SpecifiedTransformTranslate>;

    #[cxx_name = "tryGetTransformOperationAsTranslateX"]
    unsafe fn try_get_transform_operation_as_translate_x<'a>(
      operation: &'a SpecifiedTransformOperation,
    ) -> Result<&'a SpecifiedLengthPercentage>;

    #[cxx_name = "tryGetTransformOperationAsTranslateY"]
    unsafe fn try_get_transform_operation_as_translate_y<'a>(
      operation: &'a SpecifiedTransformOperation,
    ) -> Result<&'a SpecifiedLengthPercentage>;

    #[cxx_name = "tryGetTransformOperationAsTranslateZ"]
    unsafe fn try_get_transform_operation_as_translate_z<'a>(
      operation: &'a SpecifiedTransformOperation,
    ) -> Result<&'a SpecifiedLength>;

    #[cxx_name = "tryGetTransformOperationAsTranslate3D"]
    fn try_get_transform_operation_as_translate3d(
      operation: &SpecifiedTransformOperation,
    ) -> Result<SpecifiedTransformTranslate3D>;

    #[cxx_name = "showTransformOperation"]
    fn show_transform_operation(operation: &SpecifiedTransformOperation);
  }

  #[namespace = "holocron::css::selectors"]
  extern "Rust" {
    #[cxx_name = "Component"]
    type PrismComponent;
    #[cxx_name = "ComponentList"]
    type PrismComponentList;
    #[cxx_name = "Selector"]
    type PrismSelector;
    #[cxx_name = "SelectorList"]
    type PrismSelectorList;

    /// Returns the type of the component.
    #[cxx_name = "getComponentType"]
    fn get_component_type(component: &PrismComponent) -> SelectorComponentType;

    /// Returns the combinator of the component.
    #[cxx_name = "getComponentCombinator"]
    fn get_component_combinator(component: &PrismComponent) -> SelectorComponentCombinator;

    /// Returns the name of the component.
    #[cxx_name = "tryGetComponentName"]
    fn try_get_component_name(component: &PrismComponent) -> String;

    /// Returns the component list length.
    #[cxx_name = "getComponentListLength"]
    fn get_component_list_len(list: &PrismComponentList) -> usize;

    /// Returns the component at the given index.
    #[cxx_name = "getComponentAt"]
    fn get_component_at_index(
      list: &PrismComponentList,
      index: usize,
    ) -> Result<Box<PrismComponent>>;

    /// Returns the selector components.
    #[cxx_name = "getSelectorComponents"]
    fn get_selector_components(selector: &PrismSelector) -> Box<PrismComponentList>;

    /// Returns the selector list length.
    #[cxx_name = "getSelectorListLength"]
    fn get_selector_list_len(list: &PrismSelectorList) -> usize;

    /// Returns the selector at the given index.
    #[cxx_name = "getSelectorAt"]
    fn get_selector_at_index(list: &PrismSelectorList, index: usize) -> Box<PrismSelector>;
  }

  #[namespace = "holocron::css::stylesheets"]
  extern "Rust" {
    #[cxx_name = "Stylesheet"]
    type CrateStylesheet;
    #[cxx_name = "CssRule"]
    type CrateCssRule;
    #[cxx_name = "StyleRule"]
    type CrateStyleRule;
    #[cxx_name = "MediaRule"]
    type CrateMediaRule;

    /// Returns the number of rules in the stylesheet.
    #[cxx_name = "getRulesLengthFromStylesheet"]
    fn get_rules_len(sheet: &CrateStylesheet) -> usize;

    /// Returns the rule at the given index.
    #[cxx_name = "getCssRuleFromStylesheet"]
    fn get_css_rule(sheet: &CrateStylesheet, index: usize) -> Result<Box<CrateCssRule>>;

    /// Returns the type of the CSS rule.
    #[cxx_name = "getCssRuleType"]
    fn get_css_rule_type(rule: &CrateCssRule) -> CssRuleType;

    /// Returns the style rule implementation.
    #[cxx_name = "getStyleRuleImpl"]
    fn get_style_rule_impl(rule: &CrateCssRule) -> Result<Box<CrateStyleRule>>;

    /// Returns the media rule implementation.
    #[cxx_name = "getMediaRuleImpl"]
    fn get_media_rule_impl(rule: &CrateCssRule) -> Result<Box<CrateMediaRule>>;

    /// Returns the selector list of the style rule.
    #[cxx_name = "getStyleRuleSelectors"]
    fn get_style_rule_selectors(rule: &CrateStyleRule) -> Box<PrismSelectorList>;

    /// Returns the property declaration block of the style rule.
    #[cxx_name = "getStyleRuleBlock"]
    fn get_style_rule_block(rule: &CrateStyleRule) -> Box<PropertyDeclarationBlock>;
  }

  #[namespace = "holocron::css::parsing"]
  extern "Rust" {
    type CSSParser;

    /// Creates a new CSS parser from the given document URL.
    #[cxx_name = "createCSSParser"]
    fn create_css_parser(document_url: &str) -> Box<CSSParser>;

    /// Parses the given CSS text and media query.
    #[cxx_name = "parseStylesheet"]
    fn parse_stylesheet(
      parser: &CSSParser,
      css_text: &str,
      media_query: &str,
    ) -> Box<CrateStylesheet>;

    /// Parses the given style attribute.
    #[cxx_name = "parseStyleDeclaration"]
    fn parse_style_declaration2(parser: &CSSParser, input: &str) -> Box<PropertyDeclarationBlock>;

    /// Parses the given color value.
    #[cxx_name = "parseColor"]
    fn parse_color(parser: &CSSParser, input: &str) -> Color;

    /// Parses the given font family value.
    #[cxx_name = "parseFontFamily"]
    fn parse_fontfamily(parser: &CSSParser, input: &str) -> Box<SpecifiedFontFamilyValue>;

    /// Parses the given transform value.
    #[cxx_name = "parseTransform"]
    fn parse_transform(parser: &CSSParser, input: &str) -> Box<SpecifiedTransformValue>;
  }
}

impl ffi::Color {
  pub fn new(r: u32, g: u32, b: u32, a: u32) -> Self {
    Self { r, g, b, a }
  }
}

impl Default for ffi::Color {
  fn default() -> Self {
    ffi::Color::new(0, 0, 0, 1)
  }
}

impl From<&SpecifiedTransformOperation> for ffi::SpecifiedTransformOperationTag {
  fn from(operation: &SpecifiedTransformOperation) -> Self {
    match operation {
      SpecifiedTransformOperation::Matrix(..) => ffi::SpecifiedTransformOperationTag::Matrix,
      SpecifiedTransformOperation::Matrix3D(..) => ffi::SpecifiedTransformOperationTag::Matrix3D,
      SpecifiedTransformOperation::Translate(..) => ffi::SpecifiedTransformOperationTag::Translate,
      SpecifiedTransformOperation::TranslateX(..) => {
        ffi::SpecifiedTransformOperationTag::TranslateX
      }
      SpecifiedTransformOperation::TranslateY(..) => {
        ffi::SpecifiedTransformOperationTag::TranslateY
      }
      SpecifiedTransformOperation::TranslateZ(..) => {
        ffi::SpecifiedTransformOperationTag::TranslateZ
      }
      SpecifiedTransformOperation::Translate3D(..) => {
        ffi::SpecifiedTransformOperationTag::Translate3D
      }
      SpecifiedTransformOperation::Scale(..) => ffi::SpecifiedTransformOperationTag::Scale,
      SpecifiedTransformOperation::ScaleX(..) => ffi::SpecifiedTransformOperationTag::ScaleX,
      SpecifiedTransformOperation::ScaleY(..) => ffi::SpecifiedTransformOperationTag::ScaleY,
      SpecifiedTransformOperation::ScaleZ(..) => ffi::SpecifiedTransformOperationTag::ScaleZ,
      SpecifiedTransformOperation::Scale3D(..) => ffi::SpecifiedTransformOperationTag::Scale3D,
      SpecifiedTransformOperation::Rotate(..) => ffi::SpecifiedTransformOperationTag::Rotate,
      SpecifiedTransformOperation::RotateX(..) => ffi::SpecifiedTransformOperationTag::RotateX,
      SpecifiedTransformOperation::RotateY(..) => ffi::SpecifiedTransformOperationTag::RotateY,
      SpecifiedTransformOperation::RotateZ(..) => ffi::SpecifiedTransformOperationTag::RotateZ,
      SpecifiedTransformOperation::Rotate3D(..) => ffi::SpecifiedTransformOperationTag::Rotate3D,
      _ => ffi::SpecifiedTransformOperationTag::Invalid,
    }
  }
}

impl From<&selectors::parser::Combinator> for ffi::SelectorComponentCombinator {
  fn from(combinator: &selectors::parser::Combinator) -> Self {
    use selectors::parser::Combinator;
    match combinator {
      Combinator::Child => ffi::SelectorComponentCombinator::Child,
      Combinator::Descendant => ffi::SelectorComponentCombinator::Descendant,
      Combinator::NextSibling => ffi::SelectorComponentCombinator::NextSibling,
      Combinator::LaterSibling => ffi::SelectorComponentCombinator::LaterSibling,
      Combinator::PseudoElement => ffi::SelectorComponentCombinator::PseudoElement,
      Combinator::SlotAssignment => ffi::SelectorComponentCombinator::SlotAssignment,
      Combinator::Part => ffi::SelectorComponentCombinator::Part,
    }
  }
}

impl From<&PrismComponent> for ffi::SelectorComponentType {
  fn from(component: &PrismComponent) -> Self {
    component.tag
  }
}

impl From<&PrismComponent> for ffi::SelectorComponentCombinator {
  fn from(component: &PrismComponent) -> Self {
    if let Some(combinator) = component.combinator {
      combinator
    } else {
      ffi::SelectorComponentCombinator::Unknown
    }
  }
}

impl From<&CrateCssRule> for ffi::CssRuleType {
  fn from(value: &CrateCssRule) -> Self {
    match value {
      CrateCssRule::Style(..) => ffi::CssRuleType::Style,
      CrateCssRule::Media(..) => ffi::CssRuleType::Media,
      _ => ffi::CssRuleType::Unsupported,
    }
  }
}

fn get_property_declaration_block_length(block: &PropertyDeclarationBlock) -> usize {
  block.len()
}

fn property_declaration_block_to_css_string(block: &PropertyDeclarationBlock) -> String {
  block.to_css_string()
}

fn get_property_declaration_block_item_at_index(
  block: &PropertyDeclarationBlock,
  index: usize,
) -> String {
  block.item(index)
}

fn get_property_declaration_block_property(
  block: &PropertyDeclarationBlock,
  property: &str,
) -> String {
  block.get_property(property)
}

fn set_property_declaration_block_property(
  block: &mut PropertyDeclarationBlock,
  property: &str,
  value: &CxxString,
  important: bool,
) {
  block.set_property(property, value.to_str().unwrap_or(""), important);
}

fn remove_property_declaration_block_property(
  block: &mut PropertyDeclarationBlock,
  property: &str,
) -> String {
  block.remove_property(property)
}

fn is_property_declaration_block_property_important(
  block: &PropertyDeclarationBlock,
  property: &str,
) -> bool {
  block.is_property_important(property)
}

fn get_number_value(value: &SpecifiedNumberValue) -> f32 {
  value.get_value()
}

fn get_percentage_value(value: &SpecifiedPercentageValue) -> f32 {
  value.get_percent()
}

fn get_no_calc_length_unitless_value(value: &SpecifiedNoCalcLength) -> f32 {
  value.unitless_value()
}

fn get_no_calc_length_unit(value: &SpecifiedNoCalcLength) -> &str {
  value.unit()
}

fn is_no_calc_length_zero(value: &SpecifiedNoCalcLength) -> bool {
  value.is_zero()
}

fn is_no_calc_length_infinite(value: &SpecifiedNoCalcLength) -> bool {
  value.is_infinite()
}

fn is_length_calc(value: &SpecifiedLength) -> bool {
  value.is_calc()
}

fn get_length_value_tag(value: &SpecifiedLength) -> ffi::SpecifiedValueTag {
  if value.is_calc() {
    ffi::SpecifiedValueTag::Calc
  } else {
    ffi::SpecifiedValueTag::Length
  }
}

fn length_to_no_calc_length(value: &SpecifiedLength) -> Box<SpecifiedNoCalcLength> {
  Box::new(value.to_no_calc_length())
}

fn get_length_percentage_value_tag(value: &SpecifiedLengthPercentage) -> ffi::SpecifiedValueTag {
  if value.is_calc() {
    ffi::SpecifiedValueTag::Calc
  } else if value.is_no_calc_length() {
    ffi::SpecifiedValueTag::Length
  } else {
    ffi::SpecifiedValueTag::Percentage
  }
}

fn length_percentage_to_no_calc_length(
  value: &SpecifiedLengthPercentage,
) -> Box<SpecifiedNoCalcLength> {
  Box::new(value.to_no_calc_length())
}

fn length_percentage_to_percentage(
  value: &SpecifiedLengthPercentage,
) -> Box<SpecifiedPercentageValue> {
  Box::new(value.to_percentage())
}

fn get_number_matrix_item(matrix: &SpecifiedNumberMatrix, index: usize) -> &SpecifiedNumberValue {
  matrix.item(index)
}

fn get_number_matrix3d_item(
  matrix3d: &SpecifiedNumberMatrix3D,
  row: usize,
  col: usize,
) -> &SpecifiedNumberValue {
  matrix3d.item(row, col)
}

fn get_fontfamily_count(font_family: &SpecifiedFontFamilyValue) -> usize {
  font_family.count()
}

fn get_fontfamily_item(font_family: &SpecifiedFontFamilyValue, index: usize) -> String {
  font_family.item(index)
}

fn get_transform_count(transform: &SpecifiedTransformValue) -> usize {
  transform.len()
}

fn get_transform_operation_at_index(
  transform: &SpecifiedTransformValue,
  index: usize,
) -> Box<SpecifiedTransformOperation> {
  Box::new(transform.get_operation_ref(index).clone())
}

fn get_transform_operation_type(
  transform_op: &SpecifiedTransformOperation,
) -> ffi::SpecifiedTransformOperationTag {
  transform_op.into()
}

fn try_get_transform_operation_as_matrix(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<&SpecifiedNumberMatrix> {
  if let Some(matrix) = operation.try_get_matrix() {
    Ok(matrix)
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as matrix"
    ))
  }
}

fn try_get_transform_operation_as_matrix3d(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<&SpecifiedNumberMatrix3D> {
  if let Some(matrix3d) = operation.try_get_matrix3d() {
    Ok(matrix3d)
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as matrix3d"
    ))
  }
}

fn try_get_transform_operation_as_translate(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<ffi::SpecifiedTransformTranslate> {
  if let Some((x, y)) = operation.try_into_translate() {
    Ok(ffi::SpecifiedTransformTranslate {
      x: Box::new(x.clone()),
      y: Box::new(y.clone()),
    })
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as translate"
    ))
  }
}

fn try_get_transform_operation_as_translate_x(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<&SpecifiedLengthPercentage> {
  if let Some(x) = operation.try_get_translate_x() {
    Ok(x)
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as translateX"
    ))
  }
}

fn try_get_transform_operation_as_translate_y(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<&SpecifiedLengthPercentage> {
  if let Some(y) = operation.try_get_translate_y() {
    Ok(y)
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as translateY"
    ))
  }
}

fn try_get_transform_operation_as_translate_z(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<&SpecifiedLength> {
  if let Some(z) = operation.try_get_translate_z() {
    Ok(z)
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as translateZ"
    ))
  }
}

fn try_get_transform_operation_as_translate3d(
  operation: &SpecifiedTransformOperation,
) -> anyhow::Result<ffi::SpecifiedTransformTranslate3D> {
  if let Some((x, y, z)) = operation.try_into_translate3d() {
    Ok(ffi::SpecifiedTransformTranslate3D {
      x: Box::new(x),
      y: Box::new(y),
      z: Box::new(z),
    })
  } else {
    Err(anyhow::anyhow!(
      "Failed to get transform operation as translate3d"
    ))
  }
}

fn show_transform_operation(operation: &SpecifiedTransformOperation) {
  operation.show();
}

fn get_component_type(component: &PrismComponent) -> ffi::SelectorComponentType {
  component.into()
}

fn get_component_combinator(component: &PrismComponent) -> ffi::SelectorComponentCombinator {
  component.into()
}

fn try_get_component_name(component: &PrismComponent) -> String {
  if let Some(name) = component.name.clone() {
    name
  } else {
    String::new()
  }
}

fn get_component_list_len(list: &PrismComponentList) -> usize {
  list.len()
}

fn get_component_at_index(
  list: &PrismComponentList,
  index: usize,
) -> Result<Box<PrismComponent>, String> {
  let component = list.item(index);
  if let Some(component) = component {
    Ok(Box::new(component.clone()))
  } else {
    Err("Failed to get component".into())
  }
}

fn get_selector_components(selector: &PrismSelector) -> Box<PrismComponentList> {
  Box::new(selector.components.clone())
}

fn get_selector_list_len(list: &PrismSelectorList) -> usize {
  list.len()
}

fn get_selector_at_index(list: &PrismSelectorList, index: usize) -> Box<PrismSelector> {
  if list.is_empty() {
    panic!("Access to empty selector list is not allowed");
  }

  let selector = list.item(index);
  if let Some(selector) = selector {
    Box::new(selector)
  } else {
    panic!("Failed to get selector at index {}", index)
  }
}

fn get_rules_len(sheet: &CrateStylesheet) -> usize {
  sheet.rules_len()
}

fn get_css_rule(sheet: &CrateStylesheet, index: usize) -> anyhow::Result<Box<CrateCssRule>> {
  let css_rule = sheet.get_rule(index);
  if let Some(rule) = css_rule {
    Ok(Box::new(rule))
  } else {
    Err(anyhow::anyhow!("Failed to get CSS rule at index {}", index))
  }
}

fn get_css_rule_type(rule: &CrateCssRule) -> ffi::CssRuleType {
  rule.into()
}

fn get_style_rule_impl(rule: &CrateCssRule) -> Result<Box<CrateStyleRule>, String> {
  match rule {
    CrateCssRule::Style(style_rule) => Ok(Box::new(style_rule.clone())),
    _ => Err("Unsupported CSS rule type".into()),
  }
}

fn get_media_rule_impl(rule: &CrateCssRule) -> Result<Box<CrateMediaRule>, String> {
  match rule {
    CrateCssRule::Media(media_rule) => Ok(Box::new(media_rule.clone())),
    _ => Err("Unsupported CSS rule type".into()),
  }
}

fn get_style_rule_selectors(rule: &CrateStyleRule) -> Box<PrismSelectorList> {
  Box::new(rule.selectors.clone())
}

fn get_style_rule_block(rule: &CrateStyleRule) -> Box<PropertyDeclarationBlock> {
  Box::new(rule.block.clone())
}

fn create_css_parser(document_url: &str) -> Box<CSSParser> {
  Box::new(CSSParser::new_from_url(document_url))
}

fn parse_stylesheet(parser: &CSSParser, css_text: &str, media_query: &str) -> Box<CrateStylesheet> {
  Box::new(parser.parse_stylesheet(css_text, media_query))
}

fn parse_style_declaration2(parser: &CSSParser, input: &str) -> Box<PropertyDeclarationBlock> {
  Box::new(parser.parse_style_declaration(input))
}

fn parse_color(parser: &CSSParser, input: &str) -> ffi::Color {
  parser.parse_color(input)
}

fn parse_fontfamily(parser: &CSSParser, input: &str) -> Box<SpecifiedFontFamilyValue> {
  Box::new(SpecifiedFontFamilyValue::new(
    parser.parse_fontfamily(input),
  ))
}

fn parse_transform(parser: &CSSParser, input_str: &str) -> Box<SpecifiedTransformValue> {
  let mut input = cssparser::ParserInput::new(input_str);
  let mut input = cssparser::Parser::new(&mut input);
  let context = StyleParserContext::new(
    Origin::Author,
    &parser.url_data,
    Some(CssRuleType::Style),
    ParsingMode::DEFAULT,
    QuirksMode::NoQuirks,
    Default::default(),
    None,
    None,
  );

  let transform = StyleSpecifiedValues::Transform::parse(&context, &mut input)
    .map(|transform| transform)
    .unwrap();
  Box::new(SpecifiedTransformValue::new(transform))
}

mod tests {
  #[allow(unused_imports)]
  use super::*;
  use StyleGenericsValues::transform::ToMatrix;

  #[test]
  fn test_parse_color() {
    let css_parser = CSSParser::default();
    let parsed_color = parse_color(&css_parser, "rgba(255, 0, 0, 0.5)");
    assert_eq!(parsed_color.r, 255);
    assert_eq!(parsed_color.g, 0);
    assert_eq!(parsed_color.b, 0);
    assert_eq!(parsed_color.a, 127);
  }

  #[test]
  fn test_parse_font_family() {
    let css_parser = CSSParser::default();
    let fonts = css_parser.parse_fontfamily("Arial, \"PingFang SC\", sans-serif");
    assert_eq!(fonts.len(), 3);
    assert_eq!(fonts[0], "Arial");
    assert_eq!(fonts[1], "PingFang SC");
    assert_eq!(fonts[2], "sans-serif");
  }

  #[test]
  fn test_parse_transform() {
    // let transform = CSSTransformInstance::from_str("translate3d(10px, 20em, 5px)");
    // assert_eq!(transform.len(), 1);
    // let item = transform.item(0);
    // assert_eq!(item.is_3d(), true);
    // assert_eq!(item.is_translate(), true);
    // match item {
    //   GenericTransformOperation::Translate3D(x, y, z) => {
    //     assert_eq!("10px", x.to_css_string());
    //     assert_eq!("20em", y.to_css_string());
    //     assert_eq!("5px", z.to_css_string());
    //   }
    //   _ => panic!("Expected translate operation"),
    // }
  }

  #[test]
  fn test_parse_style_declaration() {
    let mut pdb = CSSPropertyDeclarationBlock::from_str(
      "display:flex;color:rgba(255,0,0,0.5);height:20px;width:100% !important;",
    );
    let display_str = pdb.get_property("display");
    let color_str = pdb.get_property("color");
    let height_str = pdb.get_property("height");
    let width_str = pdb.get_property("width");

    assert_eq!(pdb.len(), 4);
    assert_eq!(display_str, "flex".to_string());
    assert_eq!(color_str, "rgba(255, 0, 0, 0.5)".to_string());
    assert_eq!(height_str, "20px".to_string());
    assert_eq!(width_str, "100%".to_string());
    assert_eq!(pdb.item(0), "display");
    assert_eq!(pdb.item(1), "color");
    assert_eq!(pdb.item(2), "height");
    assert_eq!(pdb.item(3), "width");
    assert_eq!(pdb.item(4), ""); // Returns empty string for out of bounds index

    // Test importance
    assert_eq!(pdb.get_importance("color"), Importance::Normal);
    assert_eq!(pdb.get_importance("width"), Importance::Important);

    // Test set property
    pdb.set_property("color", "rgba(0, 255, 0, 0.5)", Importance::Important);
    assert_eq!(
      pdb.get_property("color"),
      "rgba(0, 255, 0, 0.5)".to_string()
    );
    pdb.set_property(
      "background-image",
      "url(https://foobar)",
      Importance::Normal,
    );
    assert_eq!(pdb.len(), 5);

    // Test remove property
    let removed = pdb.remove_property("height");
    assert_eq!(removed, "20px".to_string());
    assert_eq!(pdb.get_property("height"), "");
    assert_eq!(pdb.len(), 4);

    let css_str = pdb.to_css_string();
    println!("cssText: {:?}", css_str);
  }

  #[test]
  fn test_parse_stylesheets() {
    let css_text = r#"
      @import url("foo.css");
      @media screen and (min-width: 900px) {
        body {
          color: red;
        }
      }
      @keyframes slidein {
        from {
          transform: translateX(0%);
        }
        to {
          transform: translateX(100%);
        }
      }
      body, .foo > div#bar {
        display: flex;
        color: rgba(255, 0, 0, 0.5);
        height: 20px;
        width: 100% !important;
      }
    "#;
    let sheet = CSSStylesheetInner::from_str(css_text, "");
    assert_eq!(sheet.rules_len() > 0, true);

    for i in 0..sheet.rules_len() {
      match sheet.get_rule(i) {
        CSSRuleInner::Style(style_rule) => {
          let block = unsafe { &mut *style_rule.block };
          assert_eq!(block.get_property("color"), "rgba(255, 0, 0, 0.5)");

          unsafe {
            let it = style_rule.selectors;
            let mut i = 0;
            while !(*it.offset(i)).is_null() {
              let selector = *it.offset(i);
              let selector = &*selector;
              println!("Selector {:?}", selector);
              i += 1;
            }
            assert_eq!(i, 2);
          }

          // Free the selectors array
          css_selectors_array_free(style_rule.selectors);
        }
        _ => {
          // Skip
        }
      }
    }
  }
}
