use std::{cell::RefCell, rc::Rc};

use style::{
  shared_lock as StyleSharedLock,
  stylesheets::{CssRule as StyleCssRule, Stylesheet as StylesheetImpl, StylesheetInDocument},
};

use super::{CssRule, MediaRule, StyleRule};

pub(crate) struct Stylesheet {
  handle: Rc<RefCell<StylesheetImpl>>,
  shared_lock: StyleSharedLock::SharedRwLock,
}

impl Stylesheet {
  pub fn new(handle: StylesheetImpl, shared_lock: StyleSharedLock::SharedRwLock) -> Self {
    Self {
      handle: Rc::new(RefCell::new(handle)),
      shared_lock,
    }
  }

  pub fn rules_len(&self) -> usize {
    let shared_lock = self.shared_lock.clone();
    let guard = shared_lock.read();

    let sheet = self.handle.borrow();
    sheet.rules(&guard).len()
  }

  pub fn get_rule(&self, index: usize) -> Option<super::CssRule> {
    let shared_lock = self.shared_lock.clone();
    let guard = shared_lock.read();

    let sheet = self.handle.borrow();
    let rule = sheet.rules(&guard).get(index);

    if let Some(rule) = rule {
      let guard = shared_lock.read();
      match rule {
        StyleCssRule::Style(style_rule) => {
          let style_rule = style_rule.read_with(&guard);
          Some(CssRule::Style(StyleRule::new(style_rule, &guard)))
        }
        StyleCssRule::Media(media_rule) => {
          Some(CssRule::Media(MediaRule::new(media_rule.as_ref())))
        }
        _ => None,
      }
    } else {
      None
    }
  }
}
