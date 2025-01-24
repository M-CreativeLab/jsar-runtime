use std::ffi::CString;
use std::os::raw::c_char;
use std::{cell::RefCell, rc::Rc};

use cssparser::{Parser as CssParser, ParserInput};
use selectors::parser::{Component, Selector};
use style::context::QuirksMode;
use style::media_queries::MediaList;
use style::parser::ParserContext as CssParserContext;
use style::parser::{Parse, ParserContext};
use style::properties::{
  parse_one_declaration_into, parse_style_attribute, Importance, PropertyDeclarationBlock,
  PropertyId, SourcePropertyDeclaration,
};
use style::selector_parser::SelectorImpl;
use style::servo_arc::Arc;
use style::shared_lock::{self, SharedRwLock, SharedRwLockReadGuard};
use style::stylesheets::{
  AllowImportRules, CssRule, CssRuleType, Origin, Stylesheet, StylesheetInDocument,
};
use style::values::computed::font::{GenericFontFamily, SingleFontFamily};
use style::values::specified::color::Color;
use style::values::specified::font::FontFamily;
use style_traits::{ParsingMode, ToCss};
use url::Url;

use crate::release_cstring;

#[repr(C)]
pub struct RGBAColor {
  pub r: u32,
  pub g: u32,
  pub b: u32,
  pub a: u32,
}

impl RGBAColor {
  fn new(r: u32, g: u32, b: u32, a: u32) -> Self {
    Self { r, g, b, a }
  }
}

#[no_mangle]
extern "C" fn parse_csscolor(color_str: *const c_char) -> RGBAColor {
  let color_string: &str = unsafe { std::ffi::CStr::from_ptr(color_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let mut input = ParserInput::new(color_string);
  let mut parser = CssParser::new(&mut input);
  let url = Url::parse("about:blank").unwrap().into();
  let context = ParserContext::new(
    Origin::Author,
    &url,
    Some(CssRuleType::Style),
    ParsingMode::DEFAULT,
    QuirksMode::NoQuirks,
    Default::default(),
    None,
    None,
  );
  match Color::parse_and_compute(&context, &mut parser, None) {
    Some(color) => {
      let rgba = color.as_absolute().unwrap().into_srgb_legacy();
      RGBAColor::new(
        (rgba.components.0 * 255.0) as u32,
        (rgba.components.1 * 255.0) as u32,
        (rgba.components.2 * 255.0) as u32,
        (rgba.alpha * 255.0) as u32,
      )
    }
    None => RGBAColor::new(0, 0, 0, 1),
  }
}

#[no_mangle]
extern "C" fn parse_font_family(input_str: *const c_char) -> *mut *mut c_char {
  let font_family_str: &str = unsafe { std::ffi::CStr::from_ptr(input_str) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let mut input = ParserInput::new(font_family_str);
  let mut parser = CssParser::new(&mut input);
  let url = Url::parse("about:blank").unwrap().into();
  let context = ParserContext::new(
    Origin::Author,
    &url,
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
  c_array.push(std::ptr::null_mut());

  let res = c_array.as_mut_ptr();
  std::mem::forget(c_array);
  res
}

/// cbindgen:ignore
#[repr(C)]
#[derive(Clone)]
pub struct CSSPropertyDeclarationBlock {
  handle: Rc<RefCell<PropertyDeclarationBlock>>,
}

fn remove_property(decls: &mut PropertyDeclarationBlock, id: &PropertyId) -> bool {
  let first_declaration = decls.first_declaration_to_remove(id);
  let first_declaration = match first_declaration {
    Some(i) => i,
    None => return false,
  };
  decls.remove_property(id, first_declaration);
  true
}

impl CSSPropertyDeclarationBlock {
  pub fn new(inner: &PropertyDeclarationBlock) -> Self {
    Self {
      handle: Rc::new(RefCell::new(inner.clone())),
    }
  }

  pub fn from_str(declaration: &str) -> Self {
    style_config::set_bool("layout.flexbox.enabled", true);
    style_config::set_bool("layout.grid.enabled", true);

    let url = Url::parse("about:blank").unwrap().into();
    let block: PropertyDeclarationBlock = parse_style_attribute(
      declaration,
      &url,
      None,
      QuirksMode::NoQuirks,
      CssRuleType::Style,
    );
    Self::new(&block)
  }

  pub fn len(&self) -> usize {
    let pdb = self.handle.borrow();
    pdb.len()
  }

  pub fn item(&self, index: usize) -> String {
    let pdb = self.handle.borrow();
    pdb
      .declarations()
      .get(index)
      .map(|decl| decl.id().name().to_string())
      .unwrap_or_else(|| String::new())
  }

  pub fn get_property(&mut self, property: &str) -> String {
    let id = match PropertyId::parse_enabled_for_all_content(&property) {
      Ok(id) => id,
      Err(..) => return String::new(),
    };
    let pdb = self.handle.borrow();
    let mut value_str = String::new();

    pdb
      .property_value_to_css(&id, &mut value_str)
      .expect("Failed to get property value");
    value_str
  }

  pub fn set_property(&mut self, property: &str, value: &str, importance: Importance) {
    let property_id = match PropertyId::parse_enabled_for_all_content(&property) {
      Ok(id) => id,
      Err(..) => return,
    };
    let mut pdb = self.handle.borrow_mut();

    // Step 5
    let mut decls = SourcePropertyDeclaration::default();
    let url = Url::parse("about:blank").unwrap().into();
    let result = parse_one_declaration_into(
      &mut decls,
      property_id,
      &value,
      Origin::Author,
      &url,
      None,
      ParsingMode::DEFAULT,
      QuirksMode::NoQuirks,
      CssRuleType::Style,
    );

    // Step 6
    match result {
      Ok(()) => {}
      Err(_) => {
        return;
      }
    }

    let mut updates = Default::default();
    if pdb.prepare_for_update(&decls, importance, &mut updates) {
      // Step 7
      // Step 8
      pdb.update(decls.drain(), importance, &mut updates);
    }
  }

  pub fn remove_property(&self, property: &str) -> String {
    let id = match PropertyId::parse_enabled_for_all_content(&property) {
      Ok(id) => id,
      Err(..) => return String::new(),
    };
    let mut pdb = self.handle.borrow_mut();
    let mut string = String::new();

    let _ = pdb.property_value_to_css(&id, &mut string);
    remove_property(&mut pdb, &id);
    string
  }

  pub fn get_importance(&self, property: &str) -> Importance {
    let id = match PropertyId::parse_enabled_for_all_content(&property) {
      Ok(id) => id,
      Err(..) => return Importance::Normal,
    };
    let pdb = self.handle.borrow();
    if pdb.property_priority(&id).important() {
      Importance::Important
    } else {
      Importance::Normal
    }
  }

  pub fn to_css_string(&self) -> String {
    let mut serialization = String::new();
    let pdb = self.handle.borrow();
    pdb
      .to_css(&mut serialization)
      .expect("Failed to serialize CSS");
    serialization
  }
}

#[no_mangle]
extern "C" fn parse_style_declaration(
  declaration: *const c_char,
) -> *mut CSSPropertyDeclarationBlock {
  let input_str: &str = unsafe { std::ffi::CStr::from_ptr(declaration) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let block = CSSPropertyDeclarationBlock::from_str(input_str);
  Box::into_raw(Box::new(block))
}

#[no_mangle]
pub extern "C" fn css_property_declaration_block_free(ptr: *mut CSSPropertyDeclarationBlock) {
  unsafe {
    let _ = Box::from_raw(ptr);
  }
}

#[no_mangle]
extern "C" fn css_property_declaration_block_len(ptr: *mut CSSPropertyDeclarationBlock) -> usize {
  let block = unsafe { &mut *ptr };
  block.len()
}

#[no_mangle]
extern "C" fn css_property_declaration_block_item(
  ptr: *mut CSSPropertyDeclarationBlock,
  index: usize,
) -> *mut c_char {
  let block = unsafe { &mut *ptr };
  let item = block.item(index);
  let c_str = std::ffi::CString::new(item).unwrap();
  c_str.into_raw()
}

#[no_mangle]
extern "C" fn css_property_declaration_block_get_property(
  ptr: *mut CSSPropertyDeclarationBlock,
  property: *const c_char,
) -> *mut c_char {
  let block = unsafe { &mut *ptr };
  let property_str: &str = unsafe { std::ffi::CStr::from_ptr(property) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let value = block.get_property(property_str);
  let c_str = std::ffi::CString::new(value).unwrap();
  c_str.into_raw()
}

#[no_mangle]
extern "C" fn css_property_declaration_block_set_property(
  ptr: *mut CSSPropertyDeclarationBlock,
  property: *const c_char,
  value: *const c_char,
  important: bool,
) {
  let block = unsafe { &mut *ptr };
  let property_str: &str = unsafe { std::ffi::CStr::from_ptr(property) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let value_str: &str = unsafe { std::ffi::CStr::from_ptr(value) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let importance = if important {
    Importance::Important
  } else {
    Importance::Normal
  };
  block.set_property(property_str, value_str, importance);
}

#[no_mangle]
extern "C" fn css_property_declaration_block_remove_property(
  ptr: *mut CSSPropertyDeclarationBlock,
  property: *const c_char,
) -> *mut c_char {
  let block = unsafe { &mut *ptr };
  let property_str: &str = unsafe { std::ffi::CStr::from_ptr(property) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let value = block.remove_property(property_str);
  let c_str = std::ffi::CString::new(value).unwrap();
  c_str.into_raw()
}

#[no_mangle]
extern "C" fn css_property_declaration_block_is_important(
  ptr: *mut CSSPropertyDeclarationBlock,
  property: *const c_char,
) -> bool {
  let block = unsafe { &mut *ptr };
  let property_str: &str = unsafe { std::ffi::CStr::from_ptr(property) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  match block.get_importance(property_str) {
    Importance::Important => true,
    Importance::Normal => false,
  }
}

#[no_mangle]
extern "C" fn css_property_declaration_block_to_css_string(
  ptr: *mut CSSPropertyDeclarationBlock,
) -> *mut c_char {
  let block = unsafe { &mut *ptr };
  let css_str = block.to_css_string();
  let c_str = std::ffi::CString::new(css_str).unwrap();
  c_str.into_raw()
}

#[repr(C)]
#[derive(Clone, Debug)]
pub struct NamedSelectorComponent {
  pub name: *mut c_char,
}

impl NamedSelectorComponent {
  pub fn new(name: String) -> Self {
    Self {
      name: CString::new(name).unwrap().into_raw(),
    }
  }
}

impl Drop for NamedSelectorComponent {
  fn drop(&mut self) {
    release_cstring(self.name);
  }
}

#[repr(u8)]
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum CombinatorSelectorComponent {
  Child,
  Descendant,
  NextSibling,
  LaterSibling,
  PseudoElement,
  SlotAssignment,
  Part,
}

/// cbindgen:derive-helper-methods
#[repr(u8)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub enum SelectorComponentInner {
  LocalName(NamedSelectorComponent),
  Id(NamedSelectorComponent),
  Class(NamedSelectorComponent),
  Root,
  Empty,
  Host,
  Combinator(CombinatorSelectorComponent),
}

#[repr(C)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub struct SelectorInner {
  size: usize,
  components: *mut *mut SelectorComponentInner,
}

impl SelectorInner {
  pub fn new(selector: Selector<SelectorImpl>) -> Self {
    let components: Vec<SelectorComponentInner> = selector
      .iter_raw_match_order()
      .map(|component| match component {
        Component::LocalName(local_name) => SelectorComponentInner::LocalName(
          NamedSelectorComponent::new(local_name.name.to_string()),
        ),
        Component::ID(id) => {
          SelectorComponentInner::Id(NamedSelectorComponent::new(id.to_string()))
        }
        Component::Class(class) => {
          SelectorComponentInner::Class(NamedSelectorComponent::new(class.to_string()))
        }
        Component::Root => SelectorComponentInner::Root,
        Component::Empty => SelectorComponentInner::Empty,
        Component::Host(_) => SelectorComponentInner::Host,
        Component::Combinator(combinator) => SelectorComponentInner::Combinator(match combinator {
          selectors::parser::Combinator::Child => CombinatorSelectorComponent::Child,
          selectors::parser::Combinator::Descendant => CombinatorSelectorComponent::Descendant,
          selectors::parser::Combinator::NextSibling => CombinatorSelectorComponent::NextSibling,
          selectors::parser::Combinator::LaterSibling => CombinatorSelectorComponent::LaterSibling,
          selectors::parser::Combinator::PseudoElement => {
            CombinatorSelectorComponent::PseudoElement
          }
          selectors::parser::Combinator::SlotAssignment => {
            CombinatorSelectorComponent::SlotAssignment
          }
          selectors::parser::Combinator::Part => CombinatorSelectorComponent::Part,
        }),
        _ => panic!("Unsupported selector component type"),
      })
      .collect();

    let size = components.len();
    let mut c_components_array: Vec<*mut SelectorComponentInner> = components
      .into_iter()
      .map(|s| Box::into_raw(Box::new(s)))
      .collect();
    c_components_array.push(std::ptr::null_mut());

    let c_components_array_ptr = c_components_array.as_mut_ptr();
    std::mem::forget(c_components_array);

    Self {
      size,
      components: c_components_array_ptr,
    }
  }
}

impl Drop for SelectorInner {
  fn drop(&mut self) {
    if self.components.is_null() {
      return;
    }
    unsafe {
      for i in 0..self.size {
        let component = *self.components.offset(i as isize);
        std::mem::drop(Box::from_raw(component));
      }
      Vec::from_raw_parts(self.components, self.size, self.size);
    }
  }
}

#[repr(C)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub struct CSSStyleRule {
  selectors: *mut *mut SelectorInner,
  block: *mut CSSPropertyDeclarationBlock,
}

#[repr(C)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub struct CSSMediaRule {
  foo: u32,
}

#[repr(C)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub struct CSSFontFaceRule {
  font_families_len: u32,
}

#[repr(C)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub struct CSSKeyframesRule {
  name: *mut c_char,
}

#[repr(C)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub struct CSSImportRule {
  href: *mut c_char,
}

/// cbindgen:derive-helper-methods
#[repr(u8)]
#[allow(dead_code)]
#[derive(Clone, Debug)]
pub enum CSSRuleInner {
  Style(CSSStyleRule),
  Media(CSSMediaRule),
  FontFace(CSSFontFaceRule),
  Keyframes(CSSKeyframesRule),
  Import(CSSImportRule),
}

impl CSSRuleInner {
  pub fn from_inner(inner: CssRule, guard: &SharedRwLockReadGuard) -> Self {
    match inner {
      CssRule::Style(style_rule) => {
        let style_rule = style_rule.read_with(guard);
        let mut selectors: Vec<SelectorInner> = Vec::new();
        for selector in style_rule.selectors.slice() {
          selectors.push(SelectorInner::new(selector.clone()));
        }

        let mut c_selectors_array: Vec<*mut SelectorInner> = selectors
          .into_iter()
          .map(|s| Box::into_raw(Box::new(s)))
          .collect();
        c_selectors_array.push(std::ptr::null_mut());

        let c_selectors_array_ptr = c_selectors_array.as_mut_ptr();
        std::mem::forget(c_selectors_array);

        let block = style_rule.block.read_with(guard);
        CSSRuleInner::Style(CSSStyleRule {
          selectors: c_selectors_array_ptr,
          block: Box::into_raw(Box::new(CSSPropertyDeclarationBlock::new(block))),
        })
      }
      CssRule::Media(media_rule) => {
        let media_list = media_rule.media_queries.read_with(guard);
        // TODO: Implement media rule
        CSSRuleInner::Media(CSSMediaRule { foo: 0 })
      }
      CssRule::FontFace(fontface_rule) => {
        let fontface_rule = fontface_rule.read_with(guard);
        // TODO: Implement font face rule
        CSSRuleInner::FontFace(CSSFontFaceRule {
          font_families_len: fontface_rule.font_face().iter().count() as u32,
        })
      }
      CssRule::Keyframes(keyframes_rule) => {
        let keyframes_rule = keyframes_rule.read_with(guard);
        let name = keyframes_rule.name.to_css_string();
        // TODO: Implement keyframes rule
        CSSRuleInner::Keyframes(CSSKeyframesRule {
          name: CString::new(name).unwrap().into_raw(),
        })
      }
      CssRule::Import(import_rule) => {
        let import_rule = import_rule.read_with(guard);
        let url = import_rule.url.to_css_string();
        CSSRuleInner::Import(CSSImportRule {
          href: CString::new(url).unwrap().into_raw(),
        })
      }
      _ => panic!("Unsupported CSS rule type"),
    }
  }
}

#[no_mangle]
pub extern "C" fn css_selectors_array_free(list: *mut *mut SelectorInner) {
  if list.is_null() {
    return;
  }
  unsafe {
    let mut i = 0;
    while !(*list.offset(i)).is_null() {
      let selector = *list.offset(i);
      std::mem::drop(Box::from_raw(selector));
      i += 1;
    }
    let len: usize = i.try_into().unwrap();
    Vec::from_raw_parts(list, len, len);
  }
}

/// cbindgen:ignore
#[repr(C)]
#[derive(Clone, Debug)]
pub struct CSSStylesheetInner {
  handle: Rc<RefCell<Stylesheet>>,
  shared_lock: SharedRwLock,
}

impl CSSStylesheetInner {
  pub fn from_str(css_text: &str, media_query: &str) -> Self {
    style_config::set_bool("layout.flexbox.enabled", true);
    style_config::set_bool("layout.grid.enabled", true);

    let url = Url::parse("about:blank").unwrap().into();
    let context = CssParserContext::new(
      Origin::Author,
      &url,
      Some(CssRuleType::Style),
      ParsingMode::DEFAULT,
      QuirksMode::NoQuirks,
      Default::default(),
      None,
      None,
    );

    let mut media_query_input = ParserInput::new(media_query); // Media query string
    let shared_lock = shared_lock::SharedRwLock::new();
    let media_query = Arc::new(shared_lock.wrap(MediaList::parse(
      &context,
      &mut CssParser::new(&mut media_query_input),
    )));
    let sheet = Stylesheet::from_str(
      &css_text,
      url,
      Origin::Author,
      media_query,
      shared_lock.clone(),
      None,
      None,
      QuirksMode::NoQuirks,
      AllowImportRules::Yes,
    );
    Self {
      handle: Rc::new(RefCell::new(sheet)),
      shared_lock,
    }
  }

  pub fn rules_len(&self) -> usize {
    let sheet = self.handle.borrow();
    let shared_lock = self.shared_lock.clone();
    let guard = shared_lock.read();
    sheet.rules(&guard).len()
  }

  pub fn get_rule(&self, index: usize) -> CSSRuleInner {
    let sheet = self.handle.borrow();
    let shared_lock = self.shared_lock.clone();
    let guard = shared_lock.read();
    let rule = sheet.rules(&guard).get(index).unwrap();
    CSSRuleInner::from_inner(rule.clone(), &guard)
  }
}

#[no_mangle]
extern "C" fn parse_css_stylesheet(
  css_text: *const c_char,
  media_query: *const c_char,
) -> *mut CSSStylesheetInner {
  let input_str: &str = unsafe { std::ffi::CStr::from_ptr(css_text) }
    .to_str()
    .expect("Failed to convert C string to Rust string");
  let media_query_str: &str = unsafe { std::ffi::CStr::from_ptr(media_query) }
    .to_str()
    .expect("Failed to convert C string to Rust string");

  let sheet = CSSStylesheetInner::from_str(input_str, media_query_str);
  Box::into_raw(Box::new(sheet))
}

#[no_mangle]
pub extern "C" fn css_stylesheet_free(ptr: *mut CSSStylesheetInner) {
  unsafe {
    let _ = Box::from_raw(ptr);
  }
}

#[no_mangle]
pub extern "C" fn css_stylesheet_rules_len(ptr: *mut CSSStylesheetInner) -> usize {
  let sheet = unsafe { &mut *ptr };
  sheet.rules_len()
}

#[no_mangle]
pub extern "C" fn css_stylesheet_get_rule(
  ptr: *mut CSSStylesheetInner,
  index: usize,
) -> CSSRuleInner {
  let sheet = unsafe { &mut *ptr };
  sheet.get_rule(index)
}

mod tests {
  #[allow(unused_imports)]
  use super::*;
  #[allow(unused_imports)]
  use std::ffi::CString;

  #[test]
  fn test_parse_csscolor() {
    let color_str = CString::new("rgba(255, 0, 0, 0.5)").unwrap();
    let color = parse_csscolor(color_str.as_ptr());
    assert_eq!(color.r, 255);
    assert_eq!(color.g, 0);
    assert_eq!(color.b, 0);
    assert_eq!(color.a, 127);
  }

  #[test]
  fn test_parse_font_family() {
    let font_family_str = CString::new("Arial, \"PingFang SC\", sans-serif").unwrap();
    let _ = parse_font_family(font_family_str.as_ptr());
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
