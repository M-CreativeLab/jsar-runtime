use std::borrow::{Borrow, BorrowMut};

use style::{
  context::QuirksMode,
  properties::{self as StyleProperties},
  stylesheets::{CssRuleType, Origin},
  values::{generics as StyleGenericsValues, specified as StyleSpecifiedValues},
};
use style_traits::{ParsingMode, ToCss};
use url::Url;

use StyleProperties::{Importance, PropertyId, SourcePropertyDeclaration};

#[derive(Clone, Debug)]
pub(crate) struct PropertyDeclarationBlock {
  handle: StyleProperties::PropertyDeclarationBlock,
}

fn remove_property(decls: &mut StyleProperties::PropertyDeclarationBlock, id: &PropertyId) -> bool {
  let first_declaration = decls.first_declaration_to_remove(id);
  let first_declaration = match first_declaration {
    Some(i) => i,
    None => return false,
  };
  decls.remove_property(id, first_declaration);
  true
}

impl PropertyDeclarationBlock {
  pub fn new(handle: &StyleProperties::PropertyDeclarationBlock) -> Self {
    Self {
      handle: handle.clone(),
    }
  }

  #[inline]
  pub fn len(&self) -> usize {
    self.handle.borrow().len()
  }

  #[inline]
  pub fn item(&self, index: usize) -> String {
    self
      .handle
      .borrow()
      .declarations()
      .get(index)
      .map(|decl| decl.id().name().to_string())
      .unwrap_or_else(|| String::new())
  }

  pub fn get_property(&self, property: &str) -> String {
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

  pub fn set_property(&mut self, property: &str, value: &str, important: bool) {
    let property_id = match PropertyId::parse_enabled_for_all_content(&property) {
      Ok(id) => id,
      Err(..) => return,
    };
    let pdb = self.handle.borrow_mut();

    // Step 5
    let mut decls = SourcePropertyDeclaration::default();
    let url = Url::parse("about:blank").unwrap().into();
    let result = StyleProperties::parse_one_declaration_into(
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

    let importance = if important {
      Importance::Important
    } else {
      Importance::Normal
    };
    let mut updates = Default::default();
    if pdb.prepare_for_update(&decls, importance, &mut updates) {
      // Step 7
      // Step 8
      pdb.update(decls.drain(), importance, &mut updates);
    }
  }

  pub fn remove_property(&mut self, property: &str) -> String {
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

  #[inline]
  pub fn is_property_important(&self, property: &str) -> bool {
    let id = match PropertyId::parse_enabled_for_all_content(&property) {
      Ok(id) => id,
      Err(..) => return false,
    };
    self.handle.borrow().property_priority(&id).important()
  }

  pub fn get_importance(&self, property: &str) -> Importance {
    if self.is_property_important(property) {
      Importance::Important
    } else {
      Importance::Normal
    }
  }

  #[inline]
  pub fn to_css_string(&self) -> String {
    let mut serialization = String::new();
    self
      .handle
      .borrow()
      .to_css(&mut serialization)
      .expect("Failed to serialize CSS");
    serialization
  }
}
