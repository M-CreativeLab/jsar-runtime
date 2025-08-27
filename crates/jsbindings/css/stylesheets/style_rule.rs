use std::borrow::Borrow;

use crate::css::{properties::PropertyDeclarationBlock, selectors::SelectorList};
use cssparser::ToCss;
use style::{
  shared_lock::{SharedRwLockReadGuard, ToCssWithGuard},
  stylesheets::StyleRule as StyleRuleImpl,
};

#[derive(Clone, Debug)]
pub(crate) struct StyleRule {
  pub selectors: SelectorList,
  pub selectors_text: String,
  pub block: PropertyDeclarationBlock,
}

impl StyleRule {
  pub fn new(handle: &StyleRuleImpl, read_guard: &SharedRwLockReadGuard) -> Self {
    let block = handle.block.read_with(read_guard);

    // convert handle.selectors to str
    let mut selectors_text = String::new();
    handle
      .selectors
      .borrow()
      .to_css(&mut selectors_text)
      .expect("Failed to convert selectors to CSS");
 
    Self {
      selectors: SelectorList::new(&handle.selectors),
      selectors_text,
      block: PropertyDeclarationBlock::new(block),
    }
  }
}
