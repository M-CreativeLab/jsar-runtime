use std::{cell::RefCell, rc::Rc};

use taffy::TraversePartialTree;

#[repr(C)]
#[derive(Clone)]
pub struct TaffyTree {
  handle: Rc<RefCell<taffy::TaffyTree<()>>>,
}

impl TaffyTree {
  pub fn new() -> Self {
    let tree = taffy::TaffyTree::new();
    Self {
      handle: Rc::new(RefCell::new(tree)),
    }
  }
}

#[repr(C)]
#[derive(Clone)]
pub struct Node {
  tree: TaffyTree,
  node: taffy::NodeId,
}

#[repr(C)]
#[derive(Clone)]
pub struct LayoutOutput {
  pub width: f32,
  pub height: f32,
  pub x: f32,
  pub y: f32,
  pub border_top: f32,
  pub border_right: f32,
  pub border_bottom: f32,
  pub border_left: f32,
  pub padding_top: f32,
  pub padding_right: f32,
  pub padding_bottom: f32,
  pub padding_left: f32,
}

impl LayoutOutput {
  pub fn new(tree: &TaffyTree, node: taffy::NodeId) -> Self {
    let taffy = tree.handle.borrow();
    let layout = taffy.layout(node).expect("Failed to get layout");
    Self {
      width: layout.size.width,
      height: layout.size.height,
      x: layout.location.x,
      y: layout.location.y,
      border_top: layout.border.top,
      border_right: layout.border.right,
      border_bottom: layout.border.bottom,
      border_left: layout.border.left,
      padding_top: layout.padding.top,
      padding_right: layout.padding.right,
      padding_bottom: layout.padding.bottom,
      padding_left: layout.padding.left,
    }
  }
}

#[repr(C)]
#[derive(Clone)]
pub enum Display {
  Block = 0,
  Flex = 1,
  Grid = 2,
  None = 3,
}

impl Display {
  pub fn from_taffy(display: taffy::style::Display) -> Self {
    match display {
      taffy::style::Display::Block => Display::Block,
      taffy::style::Display::Flex => Display::Flex,
      taffy::style::Display::Grid => Display::Grid,
      taffy::style::Display::None => Display::None,
    }
  }
  pub fn to_taffy(&self) -> taffy::style::Display {
    match self {
      Display::Block => taffy::style::Display::Block,
      Display::Flex => taffy::style::Display::Flex,
      Display::Grid => taffy::style::Display::Grid,
      Display::None => taffy::style::Display::None,
    }
  }
}

#[repr(C)]
#[derive(Clone)]
pub enum Position {
  Relative = 0,
  Absolute = 1,
}

impl Position {
  pub fn from_taffy(position: taffy::style::Position) -> Self {
    match position {
      taffy::style::Position::Relative => Position::Relative,
      taffy::style::Position::Absolute => Position::Absolute,
    }
  }
  pub fn to_taffy(&self) -> taffy::style::Position {
    match self {
      Position::Relative => taffy::style::Position::Relative,
      Position::Absolute => taffy::style::Position::Absolute,
    }
  }
}

#[repr(C)]
#[derive(Clone)]
pub enum Dimension {
  Length = 0,
  Percent = 1,
  Auto = 2,
}

impl Dimension {
  pub fn from_taffy(space: taffy::style::Dimension) -> Self {
    match space {
      taffy::style::Dimension::Length(_) => Dimension::Length,
      taffy::style::Dimension::Percent(_) => Dimension::Percent,
      taffy::style::Dimension::Auto => Dimension::Auto,
    }
  }
  pub fn get_value_from_taffy(space: taffy::style::Dimension) -> f32 {
    match space {
      taffy::style::Dimension::Length(value) => value,
      taffy::style::Dimension::Percent(value) => value,
      taffy::style::Dimension::Auto => 0.0,
    }
  }
  pub fn to_taffy(&self, value: f32) -> taffy::style::Dimension {
    match self {
      Dimension::Length => taffy::style::Dimension::Length(value),
      Dimension::Percent => taffy::style::Dimension::Percent(match value > 1.0 {
        true => 1.0,
        false => value,
      }),
      Dimension::Auto => taffy::style::Dimension::Auto,
    }
  }
}

#[repr(C)]
#[derive(Clone)]
pub struct Style {
  pub display: Display,
  pub position: Position,
  pub width: Dimension,
  pub height: Dimension,
  pub width_value: f32,
  pub height_value: f32,
  pub flex_grow: f32,
  pub flex_shrink: f32,
}

impl Style {
  pub fn from_taffy(style: taffy::style::Style) -> Self {
    Self {
      display: Display::from_taffy(style.display),
      position: Position::from_taffy(style.position),
      width: Dimension::from_taffy(style.size.width),
      height: Dimension::from_taffy(style.size.height),
      width_value: Dimension::get_value_from_taffy(style.size.width),
      height_value: Dimension::get_value_from_taffy(style.size.height),
      flex_grow: style.flex_grow,
      flex_shrink: style.flex_shrink,
    }
  }
  pub fn to_taffy(&self) -> taffy::style::Style {
    taffy::style::Style {
      display: self.display.to_taffy(),
      position: self.position.to_taffy(),
      flex_grow: self.flex_grow,
      flex_shrink: self.flex_shrink,
      size: taffy::Size {
        width: self.width.to_taffy(self.width_value),
        height: self.height.to_taffy(self.height_value),
      },
      ..Default::default()
    }
  }
}

impl Node {
  pub fn new(tree: &TaffyTree) -> Self {
    Self {
      tree: tree.clone(),
      node: tree
        .handle
        .borrow_mut()
        .new_leaf(taffy::Style::default())
        .expect("Failed to create new node"),
    }
  }

  pub fn add_child(&mut self, child: &Node) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .add_child(self.node, child.node);
  }

  pub fn remove_child(&mut self, child: &Node) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .remove_child(self.node, child.node);
  }

  pub fn replace_child_at_index(&mut self, index: usize, child: &Node) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .replace_child_at_index(self.node, index, child.node);
  }

  pub fn remove_child_at_index(&mut self, index: usize) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .remove_child_at_index(self.node, index);
  }

  pub fn get_style(&self) -> Style {
    let taffy_style = self
      .tree
      .handle
      .borrow()
      .style(self.node)
      .expect("Failed to get style")
      .clone();
    Style::from_taffy(taffy_style)
  }

  pub fn set_style(&mut self, style: Style) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .set_style(self.node, Style::to_taffy(&style));
  }

  pub fn mark_dirty(&mut self) {
    let _ = self.tree.handle.borrow_mut().mark_dirty(self.node);
  }

  pub fn is_dirty(&self) -> bool {
    self
      .tree
      .handle
      .borrow()
      .dirty(self.node)
      .expect("Failed to get dirty")
  }

  pub fn child_count(&self) -> usize {
    self.tree.handle.borrow().child_count(self.node)
  }

  pub fn is_childless(&self) -> bool {
    self.child_count() == 0
  }

  pub fn compute_layout(&mut self, width: f32, height: f32) {
    let _ = self.tree.handle.borrow_mut().compute_layout(
      self.node,
      taffy::geometry::Size {
        width: taffy::style::AvailableSpace::Definite(width),
        height: taffy::style::AvailableSpace::Definite(height),
      },
    );
  }

  pub fn get_layout(&self) -> LayoutOutput {
    LayoutOutput::new(&self.tree, self.node)
  }
}

#[no_mangle]
pub extern "C" fn taffy_tree_new() -> *mut TaffyTree {
  Box::into_raw(Box::new(TaffyTree::new()))
}

#[no_mangle]
pub extern "C" fn taffy_tree_free(ptr: *mut TaffyTree) {
  unsafe {
    let _ = Box::from_raw(ptr);
  }
}

#[no_mangle]
pub extern "C" fn taffy_node_new(tree: *const TaffyTree) -> *mut Node {
  let tree = unsafe { &*tree };
  Box::into_raw(Box::new(Node::new(tree)))
}

#[no_mangle]
pub extern "C" fn taffy_node_free(ptr: *mut Node) {
  unsafe {
    let _ = Box::from_raw(ptr);
  }
}

#[no_mangle]
pub extern "C" fn taffy_node_add_child(parent: *mut Node, child: *const Node) {
  let parent = unsafe { &mut *parent };
  let child = unsafe { &*child };
  parent.add_child(child);
}

#[no_mangle]
pub extern "C" fn taffy_node_remove_child(parent: *mut Node, child: *const Node) {
  let parent = unsafe { &mut *parent };
  let child = unsafe { &*child };
  parent.remove_child(child);
}

#[no_mangle]
pub extern "C" fn taffy_node_replace_child_at_index(
  parent: *mut Node,
  index: usize,
  child: *const Node,
) {
  let parent = unsafe { &mut *parent };
  let child = unsafe { &*child };
  parent.replace_child_at_index(index, child);
}

#[no_mangle]
pub extern "C" fn taffy_node_remove_child_at_index(parent: *mut Node, index: usize) {
  let parent = unsafe { &mut *parent };
  parent.remove_child_at_index(index);
}

#[no_mangle]
pub extern "C" fn taffy_node_get_style(node: *const Node) -> Style {
  let node = unsafe { &*node };
  node.get_style()
}

#[no_mangle]
pub extern "C" fn taffy_node_set_style(node: *mut Node, style: Style) {
  let node = unsafe { &mut *node };
  node.set_style(style);
}

#[no_mangle]
pub extern "C" fn taffy_node_mark_dirty(node: *mut Node) {
  let node = unsafe { &mut *node };
  node.mark_dirty();
}

#[no_mangle]
pub extern "C" fn taffy_node_is_dirty(node: *const Node) -> bool {
  let node = unsafe { &*node };
  node.is_dirty()
}

#[no_mangle]
pub extern "C" fn taffy_node_is_childless(node: *const Node) -> bool {
  let node = unsafe { &*node };
  node.is_childless()
}

#[no_mangle]
pub extern "C" fn taffy_node_get_child_count(node: *const Node) -> usize {
  let node = unsafe { &*node };
  node.child_count()
}

#[no_mangle]
pub extern "C" fn taffy_node_compute_layout(node: *mut Node, width: f32, height: f32) {
  let node = unsafe { &mut *node };
  node.compute_layout(width, height);
}

#[no_mangle]
pub extern "C" fn taffy_node_get_layout(node: *const Node) -> LayoutOutput {
  let node = unsafe { &*node };
  node.get_layout()
}
