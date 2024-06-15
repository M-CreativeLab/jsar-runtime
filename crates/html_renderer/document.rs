use crate::node::{Node, NodeData, TextNodeData};
use crate::util::walk_tree;
use slab::Slab;
use selectors::Element;
use style::media_queries::Device;
use style::{
  context::QuirksMode,
  dom::{TDocument, TNode},
  shared_lock::{SharedRwLock, StylesheetGuards},
  stylist::Stylist,
};
use url::Url;

pub struct Document {
  pub(crate) guard: SharedRwLock,
  pub(crate) stylist: Stylist,
  pub(crate) nodes: Box<Slab<Node>>,
  pub(crate) base_url: Option<Url>,
  pub(crate) scale: f32,
}

impl Document {
  pub fn new(device: Device) -> Self {
    let guard = SharedRwLock::new();
    let quirks = QuirksMode::NoQuirks;
    let stylist = Stylist::new(device, quirks);
    let nodes = Box::new(Slab::new());

    style_config::set_bool("layout.flexbox.enabled", true);
    style_config::set_bool("layout.columns.enabled", true);
    style_config::set_bool("layout.legacy_layout", true);

    let mut document = Document {
      guard,
      stylist,
      nodes,
      base_url: None,
      scale: 1.0,
    };
    document.create_node(NodeData::Document);
    document
  }

  pub fn set_base_url(&mut self, url: &str) {
    self.base_url = Url::parse(url).ok();
  }

  pub fn guard(&self) -> &SharedRwLock {
    &self.guard
  }

  pub fn set_scale(&mut self, scale: f32) {
    self.scale = scale;
  }

  pub fn tree(&self) -> &Slab<Node> {
    &self.nodes
  }

  pub fn get_node(&self, node_id: usize) -> Option<&Node> {
    self.nodes.get(node_id)
  }

  pub fn get_node_mut(&mut self, node_id: usize) -> Option<&mut Node> {
    self.nodes.get_mut(node_id)
  }

  pub fn root_node(&self) -> &Node {
    &self.nodes[0]
  }

  pub fn root_element(&self) -> &Node {
    TDocument::as_node(&self.root_node())
      .first_element_child()
      .unwrap()
      .as_element()
      .unwrap()
  }

  pub fn create_node(&mut self, node_data: NodeData) -> usize {
    let slab_ptr = self.nodes.as_mut() as *mut Slab<Node>;
    let entry = self.nodes.vacant_entry();
    let id = entry.key();
    let guard = self.guard.clone();

    entry.insert(Node::new(slab_ptr, id, guard, node_data));
    id
  }

  pub fn create_text_node(&mut self, text: &str) -> usize {
    let content = TextNodeData::new(text.to_string());
    self.create_node(NodeData::Text(content))
  }

  pub fn print_tree(&self) {
    walk_tree(0, self.root_node());
  }
}

impl AsRef<Document> for Document {
  fn as_ref(&self) -> &Document {
    self
  }
}

impl AsMut<Document> for Document {
  fn as_mut(&mut self) -> &mut Document {
    self
  }
}
