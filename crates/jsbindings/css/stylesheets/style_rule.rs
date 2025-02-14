use crate::css::{properties::PropertyDeclarationBlock, selectors::SelectorList};
use style::{shared_lock::SharedRwLockReadGuard, stylesheets::StyleRule as StyleRuleImpl};

#[derive(Clone, Debug)]
pub(crate) struct StyleRule {
  pub selectors: SelectorList,
  pub block: PropertyDeclarationBlock,
}

impl StyleRule {
  pub fn new(handle: &StyleRuleImpl, read_guard: &SharedRwLockReadGuard) -> Self {
    let block = handle.block.read_with(read_guard);
    Self {
      selectors: SelectorList::new(&handle.selectors),
      block: PropertyDeclarationBlock::new(block),
    }
  }
}
