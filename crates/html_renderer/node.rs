use html5ever::{LocalName, QualName};
use slab::Slab;
use style::shared_lock::SharedRwLock;
use style::Atom;

pub struct Node {
  /// [unsafe] The actual tree we belong to.
  pub tree: *mut Slab<Node>,

  /// The parent node of this node.
  pub parent: Option<usize>,

  /// The unique identifier of this node.
  pub id: usize,

  /// The index of this node in the parent's children list.
  pub child_idx: usize,

  /// The children of this node.
  pub children: Vec<usize>,

  /// Node type (Element, TextNode, etc) specific data
  pub raw_dom_data: NodeData,

  /// The shared lock for the document
  pub guard: SharedRwLock,

  /// The node's layout states
  pub hidden: bool,
  pub is_hovered: bool,
}

impl Node {
  pub fn new(tree: *mut Slab<Node>, id: usize, guard: SharedRwLock, data: NodeData) -> Self {
    Node {
      tree,
      id,
      parent: None,
      child_idx: 0,
      children: vec![],
      raw_dom_data: data,
      guard,

      // layout states
      hidden: false,
      is_hovered: false,
    }
  }

  pub fn tree(&self) -> &Slab<Node> {
    unsafe { &*self.tree }
  }

  pub fn with(&self, id: usize) -> &Node {
    self.tree().get(id).unwrap()
  }

  pub fn forward(&self, n: usize) -> Option<&Node> {
    self.tree()[self.parent?]
      .children
      .get(self.child_idx + n)
      .map(|id| self.with(*id))
  }

  pub fn backward(&self, n: usize) -> Option<&Node> {
    if self.child_idx < n {
      return None;
    }

    self.tree()[self.parent?]
      .children
      .get(self.child_idx - n)
      .map(|id| self.with(*id))
  }

  pub fn is_element(&self) -> bool {
    matches!(self.raw_dom_data, NodeData::Element { .. })
  }

  pub fn is_text_node(&self) -> bool {
    matches!(self.raw_dom_data, NodeData::Text { .. })
  }

  pub fn element_data(&self) -> Option<&ElementNodeData> {
    match self.raw_dom_data {
      NodeData::Element(ref data) => Some(data),
      NodeData::AnonymousBlock(ref data) => Some(data),
      _ => None,
    }
  }

  pub fn element_data_mut(&mut self) -> Option<&mut ElementNodeData> {
    match self.raw_dom_data {
      NodeData::Element(ref mut data) => Some(data),
      NodeData::AnonymousBlock(ref mut data) => Some(data),
      _ => None,
    }
  }

  pub fn text_data(&self) -> Option<&TextNodeData> {
    match self.raw_dom_data {
      NodeData::Text(ref data) => Some(data),
      _ => None,
    }
  }

  pub fn text_data_mut(&mut self) -> Option<&mut TextNodeData> {
    match self.raw_dom_data {
      NodeData::Text(ref mut data) => Some(data),
      _ => None,
    }
  }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum NodeKind {
  Document,
  Element,
  AnonymousBlock,
  Text,
  Comment,
}

#[derive(Debug, Clone)]
pub enum NodeData {
  /// The `Document` itself - the root node of a HTML document.
  Document,

  /// An element with attributes.
  Element(ElementNodeData),

  /// An anonymous block box
  AnonymousBlock(ElementNodeData),

  /// A text node.
  Text(TextNodeData),

  /// A comment.
  Comment,
}

impl NodeData {
  pub fn downcast_element(&self) -> Option<&ElementNodeData> {
    match self {
      Self::Element(data) => Some(data),
      Self::AnonymousBlock(data) => Some(data),
      _ => None,
    }
  }

  pub fn downcast_element_mut(&mut self) -> Option<&mut ElementNodeData> {
    match self {
      Self::Element(data) => Some(data),
      Self::AnonymousBlock(data) => Some(data),
      _ => None,
    }
  }

  pub fn is_element_with_tag_name(&self, name: &impl PartialEq<LocalName>) -> bool {
    let Some(elem) = self.downcast_element() else {
      return false;
    };
    *name == elem.name.local
  }

  pub fn attrs(&self) -> Option<&[Attribute]> {
    Some(&self.downcast_element()?.attrs)
  }

  pub fn attr(&self, name: impl PartialEq<LocalName>) -> Option<&str> {
    self.downcast_element()?.attr(name)
  }

  pub fn kind(&self) -> NodeKind {
    match self {
      NodeData::Document => NodeKind::Document,
      NodeData::Element(_) => NodeKind::Element,
      NodeData::AnonymousBlock(_) => NodeKind::AnonymousBlock,
      NodeData::Text(_) => NodeKind::Text,
      NodeData::Comment => NodeKind::Comment,
    }
  }
}

#[derive(PartialEq, Eq, PartialOrd, Ord, Clone, Debug)]
pub struct Attribute {
  /// The name of the attribute (e.g. the `class` in `<div class="test">`)
  pub name: QualName,
  /// The value of the attribute (e.g. the `"test"` in `<div class="test">`)
  pub value: String,
}

#[derive(Debug, Clone)]
pub struct ElementNodeData {
  /// The elements tag name, namespace and prefix
  pub name: QualName,

  /// The elements id attribute parsed as an atom (if it has one)
  pub id: Option<Atom>,

  /// The element's attributes
  pub attrs: Vec<Attribute>,
}

impl ElementNodeData {
  pub fn attrs(&self) -> &[Attribute] {
    &self.attrs
  }

  pub fn attr(&self, name: impl PartialEq<LocalName>) -> Option<&str> {
    let attr = self.attrs.iter().find(|attr| name == attr.name.local)?;
    Some(&attr.value)
  }
}

#[derive(Debug, Clone)]
pub struct TextNodeData {
  /// The textual content of the text node
  pub content: String,
}

impl TextNodeData {
  pub fn new(content: String) -> Self {
    Self { content }
  }
}

impl Eq for Node {}
impl PartialEq for Node {
  fn eq(&self, other: &Self) -> bool {
    self.id == other.id
  }
}

impl std::fmt::Debug for Node {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    // FIXME: update to reflect changes to fields
    f.debug_struct("NodeData")
      .field("parent", &self.parent)
      .field("id", &self.id)
      // .field("is_inline_root", &self.is_inline_root)
      .field("child_idx", &self.child_idx)
      .field("children", &self.children)
      // .field("layout_children", &self.layout_children.borrow())
      // .field("style", &self.style)
      .field("node", &self.raw_dom_data)
      // .field("stylo_element_data", &self.stylo_element_data)
      // .field("unrounded_layout", &self.unrounded_layout)
      // .field("final_layout", &self.final_layout)
      .finish()
  }
}
