pub mod document;
pub mod htmlsink;
pub mod node;
pub mod stylo;
pub mod util;

use std::fmt::Debug;

pub use document::Document;
pub use htmlsink::DocumentHtmlParser;

use euclid::{Scale, Size2D};
use style::{
  media_queries::{Device, MediaType},
  servo::media_queries::FontMetricsProvider,
};
use style::properties::style_structs::Font;

struct HtmlFontMetricsProvider {}
impl FontMetricsProvider for HtmlFontMetricsProvider {
  fn query_font_metrics(
    &self,
    vertical: bool,
    font: &Font,
    base_size: style::values::computed::CSSPixelLength,
    in_media_query: bool,
    retrieve_math_scales: bool,
  ) -> style::font_metrics::FontMetrics {
    style::font_metrics::FontMetrics::default()
  }
}
impl Debug for HtmlFontMetricsProvider {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    write!(f, "HtmlFontMetricsProvider")
  }
}

pub fn render_html(html_source_str: &str) {
  let viewport_size = Size2D::new(800.0, 600.0);
  let device_pixel_ratio = Scale::new(1.0);
  let device = Device::new(
    MediaType::screen(),
    selectors::matching::QuirksMode::NoQuirks,
    viewport_size,
    device_pixel_ratio,
    Box::new(HtmlFontMetricsProvider {}),
  );

  let mut document = Document::new(device);
  let document = DocumentHtmlParser::parse_into_document(&mut document, html_source_str);
  document.print_tree();
}

#[test]
fn test_render_html() {
  render_html("<h1>Hello, World!</h1>");
}
