use style::stylesheets::MediaRule as MediaRuleImpl;

#[derive(Clone, Debug)]
pub(crate) struct MediaRule {}

impl MediaRule {
  pub fn new(handle: &MediaRuleImpl) -> Self {
    Self {}
  }
}
