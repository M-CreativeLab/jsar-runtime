mod rule_list;
mod stylesheet;

mod media_rule;
mod style_rule;

pub(crate) use media_rule::MediaRule;
pub(crate) use style_rule::StyleRule;
pub(crate) use stylesheet::Stylesheet;

pub(crate) enum CssRule {
  Style(StyleRule),
  Media(MediaRule),
}
