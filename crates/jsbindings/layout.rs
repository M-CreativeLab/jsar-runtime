use std::{cell::RefCell, rc::Rc};
use taffy::TraversePartialTree;

/// cbindgen:ignore
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

/// cbindgen:ignore
#[repr(C)]
#[derive(Clone)]
pub struct TaffyNode {
  tree: TaffyTree,
  node: taffy::NodeId,
}

#[repr(C)]
#[derive(Clone, Copy, Debug)]
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

#[repr(u8)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum Display {
  Block = 0,
  Flex = 1,
  Grid = 2,
  None = 3,
}

impl From<taffy::Display> for Display {
  fn from(display: taffy::Display) -> Self {
    match display {
      taffy::Display::Block => Display::Block,
      taffy::Display::Flex => Display::Flex,
      taffy::Display::Grid => Display::Grid,
      taffy::Display::None => Display::None,
    }
  }
}

impl From<Display> for taffy::Display {
  fn from(display: Display) -> Self {
    match display {
      Display::Block => taffy::Display::Block,
      Display::Flex => taffy::Display::Flex,
      Display::Grid => taffy::Display::Grid,
      Display::None => taffy::Display::None,
    }
  }
}

#[repr(u8)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum Overflow {
  Visible = 0,
  Clip,
  Hidden,
  Scroll,
}

impl From<taffy::Overflow> for Overflow {
  fn from(overflow: taffy::Overflow) -> Self {
    match overflow {
      taffy::Overflow::Visible => Overflow::Visible,
      taffy::Overflow::Clip => Overflow::Clip,
      taffy::Overflow::Hidden => Overflow::Hidden,
      taffy::Overflow::Scroll => Overflow::Scroll,
    }
  }
}

impl From<Overflow> for taffy::Overflow {
  fn from(overflow: Overflow) -> Self {
    match overflow {
      Overflow::Visible => taffy::Overflow::Visible,
      Overflow::Clip => taffy::Overflow::Clip,
      Overflow::Hidden => taffy::Overflow::Hidden,
      Overflow::Scroll => taffy::Overflow::Scroll,
    }
  }
}

#[repr(u8)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum Position {
  Relative = 0,
  Absolute = 1,
}

impl From<taffy::Position> for Position {
  fn from(position: taffy::Position) -> Self {
    match position {
      taffy::Position::Relative => Position::Relative,
      taffy::Position::Absolute => Position::Absolute,
    }
  }
}

impl From<Position> for taffy::Position {
  fn from(position: Position) -> Self {
    match position {
      Position::Relative => taffy::Position::Relative,
      Position::Absolute => taffy::Position::Absolute,
    }
  }
}

/// cbindgen:derive-helper-methods
#[repr(C)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum Dimension {
  /// An absolute length in some abstract units. Users of Taffy may define what they correspond
  /// to in their application (pixels, logical pixels, mm, etc) as they see fit.
  Length(f32),
  /// The dimension is stored in percentage relative to the parent item.
  Percent(f32),
  /// The dimension should be automatically computed
  Auto,
}

impl From<taffy::Dimension> for Dimension {
  fn from(dimension: taffy::Dimension) -> Self {
    println!("Dimension: {:?}", dimension);
    match dimension {
      taffy::Dimension::Length(length) => Dimension::Length(length),
      taffy::Dimension::Percent(percent) => Dimension::Percent(percent),
      taffy::Dimension::Auto => Dimension::Auto,
    }
  }
}

impl From<Dimension> for taffy::Dimension {
  fn from(dimension: Dimension) -> Self {
    match dimension {
      Dimension::Length(length) => taffy::Dimension::Length(length),
      Dimension::Percent(percent) => taffy::Dimension::Percent(percent),
      Dimension::Auto => taffy::Dimension::Auto,
    }
  }
}

/// cbindgen:derive-helper-methods
#[repr(C)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum LengthPercentageAuto {
  /// An absolute length in some abstract units. Users of Taffy may define what they correspond
  /// to in their application (pixels, logical pixels, mm, etc) as they see fit.
  Length(f32),
  /// The dimension is stored in percentage relative to the parent item.
  Percent(f32),
  /// The dimension should be automatically computed
  Auto,
}

impl From<taffy::LengthPercentageAuto> for LengthPercentageAuto {
  fn from(length_percentage_auto: taffy::LengthPercentageAuto) -> Self {
    match length_percentage_auto {
      taffy::LengthPercentageAuto::Length(length) => LengthPercentageAuto::Length(length),
      taffy::LengthPercentageAuto::Percent(percent) => LengthPercentageAuto::Percent(percent),
      taffy::LengthPercentageAuto::Auto => LengthPercentageAuto::Auto,
    }
  }
}

impl From<LengthPercentageAuto> for taffy::LengthPercentageAuto {
  fn from(length_percentage_auto: LengthPercentageAuto) -> Self {
    match length_percentage_auto {
      LengthPercentageAuto::Length(length) => taffy::LengthPercentageAuto::Length(length),
      LengthPercentageAuto::Percent(percent) => taffy::LengthPercentageAuto::Percent(percent),
      LengthPercentageAuto::Auto => taffy::LengthPercentageAuto::Auto,
    }
  }
}

/// cbindgen:derive-helper-methods
#[repr(C)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum LengthPercentage {
  /// An absolute length in some abstract units. Users of Taffy may define what they correspond
  /// to in their application (pixels, logical pixels, mm, etc) as they see fit.
  Length(f32),
  /// The dimension is stored in percentage relative to the parent item.
  Percent(f32),
}

impl From<taffy::LengthPercentage> for LengthPercentage {
  fn from(length_percentage: taffy::LengthPercentage) -> Self {
    match length_percentage {
      taffy::LengthPercentage::Length(length) => LengthPercentage::Length(length),
      taffy::LengthPercentage::Percent(percent) => LengthPercentage::Percent(percent),
    }
  }
}

impl From<LengthPercentage> for taffy::LengthPercentage {
  fn from(length_percentage: LengthPercentage) -> Self {
    match length_percentage {
      LengthPercentage::Length(length) => taffy::LengthPercentage::Length(length),
      LengthPercentage::Percent(percent) => taffy::LengthPercentage::Percent(percent),
    }
  }
}

#[repr(C)]
#[derive(Clone, Copy, PartialEq, Debug)]
pub struct LayoutStyle {
  pub display: Display,
  pub overflow_x: Overflow,
  pub overflow_y: Overflow,
  pub scrollbar_width: f32,
  pub position: Position,
  pub width: Dimension,
  pub height: Dimension,

  // Spacing Properties
  pub margin_top: LengthPercentageAuto,
  pub margin_right: LengthPercentageAuto,
  pub margin_bottom: LengthPercentageAuto,
  pub margin_left: LengthPercentageAuto,
  pub padding_top: LengthPercentage,
  pub padding_right: LengthPercentage,
  pub padding_bottom: LengthPercentage,
  pub padding_left: LengthPercentage,

  // Flex Properties
  pub flex_grow: f32,
  pub flex_shrink: f32,
}

impl From<taffy::Style> for LayoutStyle {
  fn from(value: taffy::Style) -> Self {
    Self {
      display: value.display.into(),
      overflow_x: value.overflow.x.into(),
      overflow_y: value.overflow.y.into(),
      scrollbar_width: value.scrollbar_width,
      position: value.position.into(),
      width: value.size.width.into(),
      height: value.size.height.into(),
      margin_top: value.margin.top.into(),
      margin_right: value.margin.right.into(),
      margin_bottom: value.margin.bottom.into(),
      margin_left: value.margin.left.into(),
      padding_top: value.padding.top.into(),
      padding_right: value.padding.right.into(),
      padding_bottom: value.padding.bottom.into(),
      padding_left: value.padding.left.into(),
      flex_grow: value.flex_grow,
      flex_shrink: value.flex_shrink,
    }
  }
}

impl From<LayoutStyle> for taffy::Style {
  fn from(value: LayoutStyle) -> Self {
    taffy::Style {
      display: value.display.into(),
      overflow: taffy::geometry::Point {
        x: value.overflow_x.into(),
        y: value.overflow_y.into(),
      },
      scrollbar_width: value.scrollbar_width,
      position: value.position.into(),
      size: taffy::Size {
        width: value.width.into(),
        height: value.height.into(),
      },
      margin: taffy::geometry::Rect {
        top: value.margin_top.into(),
        right: value.margin_right.into(),
        bottom: value.margin_bottom.into(),
        left: value.margin_left.into(),
      },
      padding: taffy::geometry::Rect {
        top: value.padding_top.into(),
        right: value.padding_right.into(),
        bottom: value.padding_bottom.into(),
        left: value.padding_left.into(),
      },
      flex_grow: value.flex_grow,
      flex_shrink: value.flex_shrink,
      ..Default::default()
    }
  }
}

impl TaffyNode {
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

  pub fn add_child(&mut self, child: &TaffyNode) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .add_child(self.node, child.node);
  }

  pub fn remove_child(&mut self, child: &TaffyNode) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .remove_child(self.node, child.node);
  }

  pub fn replace_child_at_index(&mut self, index: usize, child: &TaffyNode) {
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

  pub fn get_style(&self) -> LayoutStyle {
    self
      .tree
      .handle
      .borrow()
      .style(self.node)
      .expect("Failed to get style")
      .clone()
      .into()
  }

  pub fn set_style(&mut self, style: LayoutStyle) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .set_style(self.node, style.into());
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
pub extern "C" fn taffy_node_new(tree: *const TaffyTree) -> *mut TaffyNode {
  let tree = unsafe { &*tree };
  Box::into_raw(Box::new(TaffyNode::new(tree)))
}

#[no_mangle]
pub extern "C" fn taffy_node_free(ptr: *mut TaffyNode) {
  unsafe {
    let _ = Box::from_raw(ptr);
  }
}

#[no_mangle]
pub extern "C" fn taffy_node_add_child(parent: *mut TaffyNode, child: *const TaffyNode) {
  let parent = unsafe { &mut *parent };
  let child = unsafe { &*child };
  parent.add_child(child);
}

#[no_mangle]
pub extern "C" fn taffy_node_remove_child(parent: *mut TaffyNode, child: *const TaffyNode) {
  let parent = unsafe { &mut *parent };
  let child = unsafe { &*child };
  parent.remove_child(child);
}

#[no_mangle]
pub extern "C" fn taffy_node_replace_child_at_index(
  parent: *mut TaffyNode,
  index: usize,
  child: *const TaffyNode,
) {
  let parent = unsafe { &mut *parent };
  let child = unsafe { &*child };
  parent.replace_child_at_index(index, child);
}

#[no_mangle]
pub extern "C" fn taffy_node_remove_child_at_index(parent: *mut TaffyNode, index: usize) {
  let parent = unsafe { &mut *parent };
  parent.remove_child_at_index(index);
}

#[no_mangle]
pub extern "C" fn taffy_node_get_style(node: *const TaffyNode) -> LayoutStyle {
  let node = unsafe { &*node };
  node.get_style()
}

#[no_mangle]
pub extern "C" fn taffy_node_set_style(node: *mut TaffyNode, style: LayoutStyle) {
  let node = unsafe { &mut *node };
  node.set_style(style);
}

#[no_mangle]
pub extern "C" fn taffy_node_mark_dirty(node: *mut TaffyNode) {
  let node = unsafe { &mut *node };
  node.mark_dirty();
}

#[no_mangle]
pub extern "C" fn taffy_node_is_dirty(node: *const TaffyNode) -> bool {
  let node = unsafe { &*node };
  node.is_dirty()
}

#[no_mangle]
pub extern "C" fn taffy_node_is_childless(node: *const TaffyNode) -> bool {
  let node = unsafe { &*node };
  node.is_childless()
}

#[no_mangle]
pub extern "C" fn taffy_node_get_child_count(node: *const TaffyNode) -> usize {
  let node = unsafe { &*node };
  node.child_count()
}

#[no_mangle]
pub extern "C" fn taffy_node_compute_layout(node: *mut TaffyNode, width: f32, height: f32) {
  let node = unsafe { &mut *node };
  node.compute_layout(width, height);
}

#[no_mangle]
pub extern "C" fn taffy_node_get_layout(node: *const TaffyNode) -> LayoutOutput {
  let node = unsafe { &*node };
  node.get_layout()
}
