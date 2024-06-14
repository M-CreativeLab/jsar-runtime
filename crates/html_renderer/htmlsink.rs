use crate::node::{Attribute, ElementNodeData, Node, NodeData};
use crate::Document;
use html5ever::{
  tendril::{StrTendril, TendrilSink},
  tree_builder::{ElementFlags, NodeOrText, QuirksMode, TreeSink},
  ExpandedName, QualName,
};
use std::borrow::Cow;

pub struct DocumentHtmlParser<'a> {
  document: &'a mut Document,
  style_nodes: Vec<u32>,

  pub errors: Vec<Cow<'static, str>>,
  pub quirks_mode: QuirksMode,
}

impl<'a> DocumentHtmlParser<'a> {
  pub fn new(document: &'a mut Document) -> Self {
    DocumentHtmlParser {
      document,
      style_nodes: Vec::new(),
      errors: Vec::new(),
      quirks_mode: QuirksMode::NoQuirks,
    }
  }

  pub fn parse_into_document(
    document: &'a mut Document,
    html_source_str: &str,
  ) -> &'a mut Document {
    let sink = Self::new(document);
    html5ever::parse_document(sink, Default::default())
      .from_utf8()
      .read_from(&mut html_source_str.as_bytes())
      .unwrap()
  }

  fn create_node(&mut self, node_data: NodeData) -> usize {
    self.document.create_node(node_data)
  }

  fn create_text_node(&mut self, text: &str) -> usize {
    self.document.create_text_node(text)
  }

  fn node(&self, id: usize) -> &Node {
    &self.document.nodes[id]
  }

  fn node_mut(&mut self, id: usize) -> &mut Node {
    &mut self.document.nodes[id]
  }
}

impl<'b> TreeSink for DocumentHtmlParser<'b> {
  type Output = &'b mut Document;
  type Handle = usize; // we use the ID of the nodes in the tree as the handle

  fn finish(self) -> Self::Output {
    for id in self.style_nodes {
      // process style nodes
    }
    for error in self.errors {
      println!("HTML Parsing Error: {}", error);
    }
    self.document
  }

  fn parse_error(&mut self, msg: Cow<'static, str>) {
    self.errors.push(msg);
  }

  fn get_document(&mut self) -> Self::Handle {
    0
  }

  fn elem_name<'a>(&'a self, target: &'a Self::Handle) -> ExpandedName<'a> {
    self
      .node(*target)
      .element_data()
      .expect("TreeSink::elem_name called on a node which is not an element!")
      .name
      .expanded()
  }

  fn create_element(
    &mut self,
    name: QualName,
    attrs: Vec<html5ever::Attribute>,
    flags: ElementFlags,
  ) -> Self::Handle {
    println!("Creating element: {:?}", name);
    0
  }

  fn create_comment(&mut self, text: StrTendril) -> Self::Handle {
    println!("Creating comment: {:?}", text);
    0
  }

  fn create_pi(&mut self, target: StrTendril, data: StrTendril) -> Self::Handle {
    unimplemented!()
  }

  fn append(&mut self, parent: &Self::Handle, child: NodeOrText<Self::Handle>) {
    println!("Appending child.");
  }

  fn append_before_sibling(&mut self, sibling: &Self::Handle, new_node: NodeOrText<Self::Handle>) {
    println!("Appending before sibling.");
  }

  fn append_based_on_parent_node(
    &mut self,
    element: &Self::Handle,
    prev_element: &Self::Handle,
    child: NodeOrText<Self::Handle>,
  ) {
    println!("Appending based on parent node.");
  }

  fn append_doctype_to_document(
    &mut self,
    name: StrTendril,
    public_id: StrTendril,
    system_id: StrTendril,
  ) {
    println!(
      "Appending doctype to document: {:?} {:?} {:?}",
      name, public_id, system_id
    );
  }

  fn get_template_contents(&mut self, target: &Self::Handle) -> Self::Handle {
    unimplemented!()
  }

  fn same_node(&self, x: &Self::Handle, y: &Self::Handle) -> bool {
    x == y
  }

  fn set_quirks_mode(&mut self, mode: QuirksMode) {
    self.quirks_mode = mode;
  }

  fn add_attrs_if_missing(&mut self, target: &Self::Handle, attrs: Vec<html5ever::Attribute>) {
    println!("Adding missing attributes: {:?}", attrs);
  }

  fn remove_from_parent(&mut self, target: &Self::Handle) {
    println!("Removing from parent: {:?}", target);
  }

  fn reparent_children(&mut self, node: &Self::Handle, new_parent: &Self::Handle) {
    println!("Reparenting children: {:?} {:?}", node, new_parent);
  }
}
