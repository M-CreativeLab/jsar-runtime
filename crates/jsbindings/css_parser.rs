use std::ffi::CString;
use std::os::raw::c_char;
use std::{cell::RefCell, rc::Rc};

use cssparser::{Parser, ParserInput};
use style::context::QuirksMode;
use style::parser::{Parse, ParserContext};
use style::properties::{
  parse_one_declaration_into, parse_style_attribute, Importance, PropertyDeclarationBlock,
  PropertyId, SourcePropertyDeclaration,
};
use style::stylesheets::{CssRuleType, Origin};
use style::values::computed::font::{GenericFontFamily, SingleFontFamily};
use style::values::specified::color::Color;
use style::values::specified::font::FontFamily;
use style_traits::ParsingMode;
use url::Url;

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
  let mut parser = Parser::new(&mut input);
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
  let mut parser = Parser::new(&mut input);
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
  let mut c_strings: Vec<CString> = fonts
    .into_iter()
    .map(|s| CString::new(s).unwrap())
    .collect();

  let mut c_array: Vec<*mut c_char> = c_strings
    .into_iter()
    .map(|cs| cs.into_raw())
    .collect();
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
  pub fn from_str(declaration: &str) -> Self {
    let url = Url::parse("about:blank").unwrap().into();
    let block = parse_style_attribute(
      declaration,
      &url,
      None,
      QuirksMode::NoQuirks,
      CssRuleType::Style,
    );
    Self {
      handle: Rc::new(RefCell::new(block)),
    }
  }

  pub fn len(&self) -> usize {
    let pdb = self.handle.borrow();
    pdb.len()
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

mod tests {
  use super::*;
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
      "color: rgba(255,0,0,0.5);height:20px;width:100% !important;",
    );
    let color_str = pdb.get_property("color");
    let height_str = pdb.get_property("height");
    let width_str = pdb.get_property("width");

    assert_eq!(pdb.len(), 3);
    assert_eq!(color_str, "rgba(255, 0, 0, 0.5)".to_string());
    assert_eq!(height_str, "20px".to_string());
    assert_eq!(width_str, "100%".to_string());

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
    assert_eq!(pdb.len(), 4);

    // Test remove property
    let removed = pdb.remove_property("height");
    assert_eq!(removed, "20px".to_string());
    assert_eq!(pdb.get_property("height"), "");
    assert_eq!(pdb.len(), 3);

    let css_str = pdb.to_css_string();
    println!("cssText: {:?}", css_str);
  }
}
