use std::{cell::RefCell, ops::BitOr, rc::Rc};

use paste::paste;
use style::values::{
  generics::{self as StyloGenericsValues},
  specified as StyloSpecifiedValues,
};
use taffy::prelude::{FromFr, FromLength, FromPercent, TaffyFitContent};
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
  pub tree: TaffyTree,
  pub node: taffy::NodeId,
}

impl TaffyNode {
  pub fn new(tree: &TaffyTree) -> Self {
    Self {
      tree: tree.clone(),
      node: tree
        .handle
        .borrow_mut()
        .new_leaf(taffy::Style {
          display: taffy::Display::Block,
          ..Default::default()
        })
        .expect("Failed to create new node"),
    }
  }

  pub fn remove(&mut self) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .remove(self.node)
      .expect("Failed to remove node");
  }

  pub fn add_child(&mut self, child: &TaffyNode) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .add_child(self.node, child.node)
      .expect("Failed to add child");
  }

  pub fn remove_child(&mut self, child: &TaffyNode) {
    let _ = self
      .tree
      .handle
      .borrow_mut()
      .remove_child(self.node, child.node)
      .expect("Failed to remove child");
  }

  /// Inserts a child before the specified child.
  ///
  /// # Arguments
  ///
  /// * `child` - The child to insert.
  /// * `before_child` - The child to insert before.
  ///
  pub fn insert_child(&mut self, child: &TaffyNode, before_child: &TaffyNode) {
    let mut tree_handle = self.tree.handle.borrow_mut();
    match tree_handle
      .children(self.node)
      .unwrap()
      .iter()
      .position(|&n| n == before_child.node)
    {
      Some(index) => {
        tree_handle
          .insert_child_at_index(self.node, index, child.node)
          .expect("Failed to insert child.");
      }
      None => {
        // FIXME(yorkie): append the child to the end of the list if the before_child is not found.
        tree_handle
          .add_child(self.node, child.node)
          .expect("Failed to add child.");
      }
    }
  }

  /// Replaces the old child with the new child. If `copy_children` is true, the children of the old child will be
  /// copied to the new child.
  ///
  /// # Arguments
  ///
  /// * `old_child` - The child to replace.
  /// * `new_child` - The new child to replace the old child with.
  /// * `copy_children` - If true, the children of the old child will be copied to the new child.
  ///
  /// # Panics
  ///
  /// Panics if the old child is not a child of the node.
  ///
  /// # Example
  ///
  /// ```rust
  /// use holocron::layout::{TaffyTree, TaffyNode};
  ///
  /// let tree = TaffyTree::new();
  /// let mut parent = TaffyNode::new(&tree);
  /// let mut old_child = TaffyNode::new(&tree);
  /// let mut new_child = TaffyNode::new(&tree);
  /// parent.add_child(&old_child);
  /// parent.replace_child(&old_child, &new_child, true);
  ///
  /// assert_eq!(parent.child_count(), 1);
  /// ```
  ///
  pub fn replace_child(
    &mut self,
    old_child: &TaffyNode,
    new_child: &TaffyNode,
    copy_children: bool,
  ) {
    let mut tree_handle = self.tree.handle.borrow_mut();
    let index = tree_handle
      .children(self.node)
      .unwrap()
      .iter()
      .position(|&n| n == old_child.node)
      .unwrap();
    let old_child = tree_handle
      .replace_child_at_index(self.node, index, new_child.node)
      .unwrap();

    if copy_children {
      tree_handle
        .children(old_child)
        .map(|children| {
          tree_handle
            .set_children(new_child.node, &children)
            .expect("Failed to set children")
        })
        .expect("Failed to get children");
    }
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
    self
      .tree
      .handle
      .try_borrow()
      .map(|tree| tree.child_count(self.node))
      .unwrap_or(0)
  }

  pub fn is_childless(&self) -> bool {
    self.child_count() == 0
  }

  pub fn compute_layout(&mut self, width: f32, height: f32) {
    use taffy::{geometry::Size, style::AvailableSpace};

    let mut tree = self
      .tree
      .handle
      .try_borrow_mut()
      .expect("Failed to borrow tree");

    let _ = tree
      .compute_layout(
        self.node,
        Size {
          width: AvailableSpace::Definite(width),
          height: AvailableSpace::Definite(height),
        },
      )
      .map_err(|e| {
        panic!("Failed to compute layout: {:?}", e);
      });
  }

  pub fn print(&mut self) -> Result<(), String> {
    let mut tree_handle = match self.tree.handle.try_borrow_mut() {
      Ok(handle) => handle,
      Err(_) => return Err("Cannot borrow tree handle: already borrowed".to_string()),
    };
    tree_handle.print_tree(self.node);
    Ok(())
  }
}

#[cxx::bridge(namespace = "holocron::layout")]
mod ffi {
  #[repr(u8)]
  #[derive(Clone, Copy, Debug)]
  enum Display {
    Block,
    Flex,
    Grid,
    None,
  }

  #[repr(u8)]
  #[derive(Clone, Copy, Debug)]
  enum BoxSizing {
    ContentBox,
    BorderBox,
  }

  #[derive(Clone, Copy, Debug)]
  enum Overflow {
    Visible = 0,
    Clip,
    Hidden,
    Scroll,
  }

  #[derive(Clone, Copy, Debug)]
  enum Position {
    Relative = 0,
    Absolute,
  }

  #[derive(Clone, Copy, Debug)]
  enum AlignItems {
    Start,
    End,
    FlexStart,
    FlexEnd,
    Center,
    Baseline,
    Stretch,
  }

  #[derive(Clone, Copy, Debug)]
  enum AlignSelf {
    Auto,
    Start,
    End,
    FlexStart,
    FlexEnd,
    Center,
    Baseline,
    Stretch,
  }

  #[derive(Clone, Copy, Debug)]
  enum AlignContent {
    Start,
    End,
    FlexStart,
    FlexEnd,
    Center,
    Stretch,
    SpaceBetween,
    SpaceEvenly,
    SpaceAround,
  }

  #[derive(Clone, Copy, Debug)]
  enum JustifyItems {
    Start,
    End,
    FlexStart,
    FlexEnd,
    Center,
    Baseline,
    Stretch,
  }

  #[derive(Clone, Copy, Debug)]
  enum JustifySelf {
    Start,
    End,
    FlexStart,
    FlexEnd,
    Center,
    Baseline,
    Stretch,
  }

  #[derive(Clone, Copy, Debug)]
  enum JustifyContent {
    Start,
    End,
    FlexStart,
    FlexEnd,
    Center,
    Stretch,
    SpaceBetween,
    SpaceEvenly,
    SpaceAround,
  }

  #[derive(Clone, Copy, Debug)]
  enum FlexDirection {
    Row,
    Column,
    RowReverse,
    ColumnReverse,
  }

  #[derive(Clone, Copy, Debug)]
  pub enum FlexWrap {
    NoWrap,
    Wrap,
    WrapReverse,
  }

  #[derive(Clone, Copy, Debug)]
  struct CSSPixelLength {
    pub value: f32,
  }

  #[derive(Clone, Copy, Debug)]
  struct Percentage {
    pub percent: f32,
  }

  #[repr(u8)]
  #[derive(Clone, Copy, Debug)]
  enum DimensionTag {
    Length,
    Percentage,
    Auto,
  }

  #[derive(Clone, Copy, Debug)]
  struct Dimension {
    pub tag: DimensionTag,
    pub length: CSSPixelLength,
    pub percentage: Percentage,
  }

  #[derive(Clone, Copy, Debug)]
  enum LengthPercentageTag {
    Length,
    Percentage,
  }

  #[derive(Clone, Copy, Debug)]
  struct LengthPercentage {
    pub tag: LengthPercentageTag,
    pub length: CSSPixelLength,
    pub percentage: Percentage,
  }

  #[derive(Clone, Copy, Debug)]
  enum LengthPercentageAutoTag {
    Length,
    Percentage,
    Auto,
  }

  #[derive(Clone, Copy, Debug)]
  struct LengthPercentageAuto {
    pub tag: LengthPercentageAutoTag,
    pub length: CSSPixelLength,
    pub percentage: Percentage,
  }

  #[derive(Clone, Copy, Debug)]
  struct OverflowXY {
    pub x: Overflow,
    pub y: Overflow,
  }

  #[derive(Clone, Copy, Debug)]
  struct DimensionRange {
    pub min: Dimension,
    pub max: Dimension,
  }

  #[derive(Clone, Copy, Debug)]
  struct LengthPercentageXY {
    pub x: LengthPercentage,
    pub y: LengthPercentage,
  }

  #[derive(Clone, Copy, Debug)]
  struct LengthPercentageRect {
    pub top: LengthPercentage,
    pub right: LengthPercentage,
    pub bottom: LengthPercentage,
    pub left: LengthPercentage,
  }

  #[derive(Clone, Copy, Debug)]
  struct LengthPercentageAutoRect {
    pub top: LengthPercentageAuto,
    pub right: LengthPercentageAuto,
    pub bottom: LengthPercentageAuto,
    pub left: LengthPercentageAuto,
  }

  #[derive(Clone, Debug)]
  struct Style {
    pub display: Display,
    #[cxx_name = "boxSizing"]
    pub box_sizing: BoxSizing,
    pub overflow: OverflowXY,
    #[cxx_name = "scrollbarWidth"]
    pub scrollbar_width: f32,
    pub position: Position,
    pub width: Dimension,
    pub height: Dimension,
    #[cxx_name = "widthRange"]
    pub width_range: DimensionRange,
    #[cxx_name = "heightRange"]
    pub height_range: DimensionRange,

    // Spacing Properties
    pub margin: LengthPercentageAutoRect,
    pub padding: LengthPercentageRect,
    pub border: LengthPercentageRect,

    // Flex Properties
    #[cxx_name = "alignItems"]
    pub align_items: AlignItems,
    #[cxx_name = "alignSelf"]
    pub align_self: AlignSelf,
    #[cxx_name = "alignContent"]
    pub align_content: AlignContent,
    #[cxx_name = "justifyItems"]
    pub justify_items: JustifyItems,
    #[cxx_name = "justifySelf"]
    pub justify_self: JustifySelf,
    #[cxx_name = "justifyContent"]
    pub justify_content: JustifyContent,
    pub gap: LengthPercentageXY,
    #[cxx_name = "flexDirection"]
    pub flex_direction: FlexDirection,
    #[cxx_name = "flexWrap"]
    pub flex_wrap: FlexWrap,
    #[cxx_name = "flexBasis"]
    pub flex_basis: Dimension,
    #[cxx_name = "flexGrow"]
    pub flex_grow: f32,
    #[cxx_name = "flexShrink"]
    pub flex_shrink: f32,

    // Grid Properties
    #[cxx_name = "gridTemplateRows"]
    pub grid_template_rows: String,
    #[cxx_name = "gridTemplateColumns"]
    pub grid_template_columns: String,
    #[cxx_name = "gridAutoRows"]
    pub grid_auto_rows: String,
    #[cxx_name = "gridAutoColumns"]
    pub grid_auto_columns: String,
    #[cxx_name = "gridAutoFlow"]
    pub grid_auto_flow: String,
    #[cxx_name = "gridRowStart"]
    pub grid_row_start: String,
    #[cxx_name = "gridRowEnd"]
    pub grid_row_end: String,
    #[cxx_name = "gridColumnStart"]
    pub grid_column_start: String,
    #[cxx_name = "gridColumnEnd"]
    pub grid_column_end: String,
  }

  #[derive(Clone, Copy, Debug)]
  struct NumberRect {
    pub top: f32,
    pub right: f32,
    pub bottom: f32,
    pub left: f32,
  }

  #[derive(Clone, Copy, Debug)]
  struct LayoutOutput {
    pub width: f32,
    pub height: f32,
    #[cxx_name = "contentWidth"]
    pub content_width: f32,
    #[cxx_name = "contentHeight"]
    pub content_height: f32,
    pub x: f32,
    pub y: f32,
    pub border: NumberRect,
    pub padding: NumberRect,
  }

  extern "Rust" {
    type TaffyTree;
    type TaffyNode;

    #[cxx_name = "createAllocator"]
    fn create_allocator() -> Box<TaffyTree>;

    #[cxx_name = "createNode"]
    fn create_node(tree: &TaffyTree) -> Box<TaffyNode>;

    #[cxx_name = "removeNode"]
    fn remove_node(node: &mut TaffyNode);

    #[cxx_name = "addChild"]
    fn add_child(parent: &mut TaffyNode, child: &TaffyNode);

    #[cxx_name = "removeChild"]
    fn remove_child(parent: &mut TaffyNode, child: &TaffyNode);

    #[cxx_name = "removeChildAtIndex"]
    fn remove_child_at_index(parent: &mut TaffyNode, index: usize);

    #[cxx_name = "insertChild"]
    fn insert_child(parent: &mut TaffyNode, child: &TaffyNode, before_child: &TaffyNode);

    #[cxx_name = "replaceChild"]
    fn replace_child(
      parent: &mut TaffyNode,
      old_child: &TaffyNode,
      new_child: &TaffyNode,
      copy_children: bool,
    );

    #[cxx_name = "replaceChildAtIndex"]
    fn replace_child_at_index(parent: &mut TaffyNode, index: usize, child: &TaffyNode);

    #[cxx_name = "markNodeDirty"]
    fn mark_node_dirty(node: &mut TaffyNode);

    #[cxx_name = "isNodeDirty"]
    fn is_node_dirty(node: &TaffyNode) -> bool;

    #[cxx_name = "isNodeChildless"]
    fn is_node_childless(node: &TaffyNode) -> bool;

    #[cxx_name = "getChildCount"]
    fn get_child_count(node: &TaffyNode) -> usize;

    #[cxx_name = "getNodeStyle"]
    fn get_node_style(node: &TaffyNode) -> Style;

    #[cxx_name = "setNodeStyle"]
    fn set_node_style(node: &mut TaffyNode, style: Style);

    #[cxx_name = "getLayoutOutput"]
    fn get_layout_output(node: &TaffyNode) -> LayoutOutput;

    #[cxx_name = "computeLayout"]
    fn compute_layout(node: &mut TaffyNode, width: f32, height: f32);

    #[cxx_name = "printNode"]
    fn print_node(node: &mut TaffyNode);
  }
}

macro_rules! impl_default_for {
  ($name:ident, $default:ident) => {
    impl Default for ffi::$name {
      fn default() -> Self {
        ffi::$name::$default
      }
    }
  };
}

macro_rules! impl_from {
  ($dst:ty, $src:ty, {$($variant:ident),+}, $default:expr) => {
    #[allow(unreachable_patterns)]
    impl From<$src> for $dst {
      fn from(value: $src) -> Self {
        match value {
          $(<$src>::$variant => Self::$variant,)+
          _ => $default,
        }
      }
    }
  };
}

macro_rules! impl_type_casting {
  ($first:ty, $second:ty, {$($variant:ident),+}, $default:ident) => {
    impl_from!($first, $second, {$($variant),+}, Self::$default);
    impl_from!($second, $first, {$($variant),+}, Self::$default);
  };
}

macro_rules! impl_type_casting_simple {
  ($name:ident, {$($variant:ident),+}, $default:ident) => {
    impl_type_casting!(ffi::$name, taffy::$name, {$($variant),+}, $default);
  };
}

impl From<taffy::Rect<f32>> for ffi::NumberRect {
  fn from(rect: taffy::Rect<f32>) -> Self {
    Self {
      top: rect.top,
      right: rect.right,
      bottom: rect.bottom,
      left: rect.left,
    }
  }
}

impl_type_casting_simple!(Display, { Block, Flex, Grid, None }, Block);
impl_type_casting_simple!(BoxSizing, { ContentBox, BorderBox }, ContentBox);
impl_type_casting_simple!(Overflow, { Visible, Clip, Hidden, Scroll }, Visible);
impl_type_casting_simple!(Position, { Relative, Absolute }, Relative);

impl_default_for!(Display, Block);
impl_default_for!(BoxSizing, ContentBox);
impl_default_for!(Overflow, Visible);
impl_default_for!(Position, Relative);

macro_rules! impl_xy_casting {
  ($name:ident, $container:ty) => {
    paste! {
      impl From<$container<taffy::$name>> for ffi::[< $name XY >] {
        fn from(value: $container<taffy::$name>) -> Self {
          Self {
            x: value.x.into(),
            y: value.y.into(),
          }
        }
      }
      impl From<ffi::[< $name XY >]> for $container<taffy::$name> {
        fn from(value: ffi::[< $name XY >]) -> Self {
          $container {
            x: value.x.into(),
            y: value.y.into(),
          }
        }
      }
    }
  };
}

impl_xy_casting!(Overflow, taffy::Point);

impl Default for ffi::CSSPixelLength {
  fn default() -> Self {
    Self { value: 0.0 }
  }
}

impl Default for ffi::Percentage {
  fn default() -> Self {
    Self { percent: 0.0 }
  }
}

impl ffi::LengthPercentage {
  pub fn length(value: f32) -> Self {
    Self {
      tag: ffi::LengthPercentageTag::Length,
      length: ffi::CSSPixelLength { value },
      percentage: ffi::Percentage::default(),
    }
  }
  pub fn percentage(percent: f32) -> Self {
    Self {
      tag: ffi::LengthPercentageTag::Percentage,
      percentage: ffi::Percentage { percent },
      length: ffi::CSSPixelLength::default(),
    }
  }
}

impl ffi::LengthPercentageAuto {
  pub fn length(value: f32) -> Self {
    Self {
      tag: ffi::LengthPercentageAutoTag::Length,
      length: ffi::CSSPixelLength { value },
      percentage: ffi::Percentage::default(),
    }
  }
  pub fn percentage(percent: f32) -> Self {
    Self {
      tag: ffi::LengthPercentageAutoTag::Percentage,
      percentage: ffi::Percentage { percent },
      length: ffi::CSSPixelLength::default(),
    }
  }
  pub fn auto() -> Self {
    Self {
      tag: ffi::LengthPercentageAutoTag::Auto,
      percentage: ffi::Percentage::default(),
      length: ffi::CSSPixelLength::default(),
    }
  }
}

impl ffi::Dimension {
  pub fn length(value: f32) -> Self {
    Self {
      tag: ffi::DimensionTag::Length,
      length: ffi::CSSPixelLength { value },
      percentage: ffi::Percentage::default(),
    }
  }
  pub fn percentage(percent: f32) -> Self {
    Self {
      tag: ffi::DimensionTag::Percentage,
      percentage: ffi::Percentage { percent },
      length: ffi::CSSPixelLength::default(),
    }
  }
  pub fn auto() -> Self {
    Self {
      tag: ffi::DimensionTag::Auto,
      length: ffi::CSSPixelLength::default(),
      percentage: ffi::Percentage::default(),
    }
  }
}

impl From<taffy::LengthPercentage> for ffi::LengthPercentage {
  fn from(value: taffy::LengthPercentage) -> Self {
    let value = value.into_raw();
    if value.is_length_or_percentage() {
      match value.tag() {
        taffy::CompactLength::LENGTH_TAG => Self::length(value.value()),
        taffy::CompactLength::PERCENT_TAG => Self::percentage(value.value()),
        _ => unreachable!(),
      }
    } else {
      unreachable!()
    }
  }
}

impl From<ffi::LengthPercentage> for taffy::LengthPercentage {
  fn from(value: ffi::LengthPercentage) -> Self {
    match value.tag {
      ffi::LengthPercentageTag::Length => taffy::LengthPercentage::from_length(value.length.value),
      ffi::LengthPercentageTag::Percentage => {
        taffy::LengthPercentage::from_percent(value.percentage.percent)
      }
      _ => unreachable!(),
    }
  }
}

impl From<taffy::LengthPercentageAuto> for ffi::LengthPercentageAuto {
  fn from(value: taffy::LengthPercentageAuto) -> Self {
    let value = value.into_raw();
    if value.is_auto() {
      Self::auto()
    } else if value.is_length_or_percentage() {
      match value.tag() {
        taffy::CompactLength::LENGTH_TAG => Self::length(value.value()),
        taffy::CompactLength::PERCENT_TAG => Self::percentage(value.value()),
        _ => unreachable!(),
      }
    } else {
      unreachable!()
    }
  }
}

impl From<ffi::LengthPercentageAuto> for taffy::LengthPercentageAuto {
  fn from(value: ffi::LengthPercentageAuto) -> Self {
    match value.tag {
      ffi::LengthPercentageAutoTag::Length => {
        taffy::LengthPercentageAuto::from_length(value.length.value)
      }
      ffi::LengthPercentageAutoTag::Percentage => {
        taffy::LengthPercentageAuto::from_percent(value.percentage.percent)
      }
      ffi::LengthPercentageAutoTag::Auto => taffy::LengthPercentageAuto::auto(),
      _ => unreachable!(),
    }
  }
}

impl From<taffy::Dimension> for ffi::Dimension {
  fn from(value: taffy::Dimension) -> Self {
    let value = value.into_raw();
    if value.is_auto() {
      Self::auto()
    } else if value.is_length_or_percentage() {
      match value.tag() {
        taffy::CompactLength::LENGTH_TAG => Self::length(value.value()),
        taffy::CompactLength::PERCENT_TAG => Self::percentage(value.value()),
        _ => unreachable!(),
      }
    } else {
      unreachable!()
    }
  }
}

impl From<ffi::Dimension> for taffy::Dimension {
  fn from(value: ffi::Dimension) -> Self {
    match value.tag {
      ffi::DimensionTag::Length => taffy::Dimension::from_length(value.length.value),
      ffi::DimensionTag::Percentage => taffy::Dimension::from_percent(value.percentage.percent),
      ffi::DimensionTag::Auto => taffy::Dimension::auto(),
      _ => {
        println!("Invalid dimension tag: {:?}", value.tag);
        unreachable!()
      }
    }
  }
}

impl ffi::DimensionRange {
  pub fn new(min: taffy::Dimension, max: taffy::Dimension) -> Self {
    Self {
      min: min.into(),
      max: max.into(),
    }
  }
}

macro_rules! impl_rect_casting {
  ($name:ident) => {
    paste! {
      impl From<taffy::Rect<taffy::$name>> for ffi::[<$name Rect>] {
        fn from(rect: taffy::Rect<taffy::$name>) -> Self {
          Self {
            top: rect.top.into(),
            right: rect.right.into(),
            bottom: rect.bottom.into(),
            left: rect.left.into(),
          }
        }
      }
      impl From<ffi::[<$name Rect>]> for taffy::Rect<taffy::$name> {
        fn from(rect: ffi::[<$name Rect>]) -> Self {
          Self {
            top: rect.top.into(),
            right: rect.right.into(),
            bottom: rect.bottom.into(),
            left: rect.left.into(),
          }
        }
      }
    }
  };
}

impl_rect_casting!(LengthPercentage);
impl_rect_casting!(LengthPercentageAuto);

macro_rules! impl_align_items_like_from_taffy {
  ($name:ident) => {
    impl_type_casting!(ffi::$name, taffy::AlignItems, {
      Start, End, FlexStart, FlexEnd, Center, Baseline, Stretch
    }, Stretch);
  };
}

macro_rules! impl_justify_items_like_from_taffy {
  ($name:ident) => {
    impl_type_casting!(ffi::$name, taffy::JustifyItems, {
      Start, End, FlexStart, FlexEnd, Center, Baseline, Stretch
    }, Stretch);
  };
}

macro_rules! impl_align_or_justify_content_from_taffy {
  ($name:ident) => {
    impl_type_casting!(ffi::$name, taffy::AlignContent, {
      Start, End, FlexStart, FlexEnd, Center, Stretch, SpaceBetween, SpaceEvenly, SpaceAround
    }, Stretch);
  };
}

impl_align_items_like_from_taffy!(AlignItems);
impl_align_items_like_from_taffy!(AlignSelf);
impl_align_or_justify_content_from_taffy!(AlignContent);
impl_justify_items_like_from_taffy!(JustifyItems);
impl_justify_items_like_from_taffy!(JustifySelf);
impl_align_or_justify_content_from_taffy!(JustifyContent);

impl_default_for!(AlignItems, Stretch);
impl_default_for!(AlignSelf, Auto);
impl_default_for!(AlignContent, Stretch);
impl_default_for!(JustifyItems, Stretch);
impl_default_for!(JustifySelf, Stretch);
impl_default_for!(JustifyContent, Stretch);

// Option from

impl From<Option<taffy::AlignSelf>> for ffi::AlignSelf {
  fn from(value: Option<taffy::AlignSelf>) -> Self {
    match value {
      Some(value) => value.into(),
      None => ffi::AlignSelf::Auto,
    }
  }
}

impl From<ffi::AlignSelf> for Option<taffy::AlignSelf> {
  fn from(value: ffi::AlignSelf) -> Self {
    match value {
      ffi::AlignSelf::Auto => None,
      _ => Some(value.into()),
    }
  }
}

impl From<taffy::Size<taffy::LengthPercentage>> for ffi::LengthPercentageXY {
  fn from(value: taffy::Size<taffy::LengthPercentage>) -> Self {
    Self {
      x: value.width.into(),
      y: value.height.into(),
    }
  }
}

impl From<ffi::LengthPercentageXY> for taffy::Size<taffy::LengthPercentage> {
  fn from(value: ffi::LengthPercentageXY) -> Self {
    Self {
      width: value.x.into(),
      height: value.y.into(),
    }
  }
}

impl From<taffy::Point<taffy::LengthPercentage>> for ffi::LengthPercentageXY {
  fn from(value: taffy::Point<taffy::LengthPercentage>) -> Self {
    Self {
      x: value.x.into(),
      y: value.y.into(),
    }
  }
}

impl From<ffi::LengthPercentageXY> for taffy::Point<taffy::LengthPercentage> {
  fn from(value: ffi::LengthPercentageXY) -> Self {
    Self {
      x: value.x.into(),
      y: value.y.into(),
    }
  }
}

impl_type_casting_simple!(FlexDirection, { Row, Column, RowReverse, ColumnReverse }, Row);
impl_type_casting_simple!(FlexWrap, { NoWrap, Wrap, WrapReverse}, NoWrap);

impl From<taffy::Style> for ffi::Style {
  fn from(style: taffy::Style) -> Self {
    ffi::Style {
      display: style.display.into(),
      box_sizing: style.box_sizing.into(),
      overflow: style.overflow.into(),
      scrollbar_width: style.scrollbar_width,
      position: style.position.into(),
      width: style.size.width.into(),
      height: style.size.height.into(),
      width_range: ffi::DimensionRange::new(style.min_size.width, style.max_size.width),
      height_range: ffi::DimensionRange::new(style.min_size.height, style.max_size.height),
      margin: style.margin.into(),
      padding: style.padding.into(),
      border: style.border.into(),
      align_items: style
        .align_items
        .unwrap_or(taffy::AlignItems::Stretch)
        .into(),
      align_self: style.align_self.into(),
      align_content: style
        .align_content
        .unwrap_or(taffy::AlignContent::Stretch)
        .into(),
      justify_items: style
        .justify_items
        .unwrap_or(taffy::JustifyItems::Stretch)
        .into(),
      justify_self: style
        .justify_self
        .unwrap_or(taffy::JustifySelf::Stretch)
        .into(),
      justify_content: style
        .justify_content
        .unwrap_or(taffy::JustifyContent::Stretch)
        .into(),
      gap: style.gap.into(),

      flex_direction: style.flex_direction.into(),
      flex_wrap: style.flex_wrap.into(),
      flex_basis: style.flex_basis.into(),
      flex_grow: style.flex_grow,
      flex_shrink: style.flex_shrink,

      grid_template_rows: "".to_string(),
      grid_template_columns: "".to_string(),
      grid_auto_rows: "".to_string(),
      grid_auto_columns: "".to_string(),
      grid_auto_flow: "".to_string(),
      grid_row_start: "".to_string(),
      grid_row_end: "".to_string(),
      grid_column_start: "".to_string(),
      grid_column_end: "".to_string(),
    }
  }
}

#[inline]
pub fn length_percentage(val: &StyloSpecifiedValues::LengthPercentage) -> taffy::LengthPercentage {
  match val {
    StyloSpecifiedValues::LengthPercentage::Length(length) => {
      taffy::LengthPercentage::from_length(length.unitless_value())
    }
    StyloSpecifiedValues::LengthPercentage::Percentage(percent) => {
      taffy::LengthPercentage::from_percent(percent.0)
    }
    _ => {
      // TODO(yorkie): support calc
      taffy::LengthPercentage::from_percent(0.0)
    }
  }
}

#[inline]
fn min_track(input: &StyloSpecifiedValues::TrackBreadth) -> taffy::MinTrackSizingFunction {
  use StyloSpecifiedValues::length::LengthPercentage;

  match input {
    StyloSpecifiedValues::TrackBreadth::Breadth(lp) => match lp {
      LengthPercentage::Length(l) => taffy::MinTrackSizingFunction::from_length(l.unitless_value()),
      LengthPercentage::Percentage(p) => taffy::MinTrackSizingFunction::from_percent(p.0),
      LengthPercentage::Calc(_) => {
        // TODO(yorkie): support calc
        taffy::MinTrackSizingFunction::auto()
      }
    },
    StyloSpecifiedValues::TrackBreadth::Fr(_) => taffy::MinTrackSizingFunction::auto(),
    StyloSpecifiedValues::TrackBreadth::Auto => taffy::MinTrackSizingFunction::auto(),
    StyloSpecifiedValues::TrackBreadth::MaxContent => taffy::MinTrackSizingFunction::max_content(),
    StyloSpecifiedValues::TrackBreadth::MinContent => taffy::MinTrackSizingFunction::min_content(),
  }
}

#[inline]
fn max_track(input: &StyloSpecifiedValues::TrackBreadth) -> taffy::MaxTrackSizingFunction {
  use StyloSpecifiedValues::length::LengthPercentage;

  match input {
    StyloSpecifiedValues::TrackBreadth::Breadth(lp) => match lp {
      LengthPercentage::Length(l) => taffy::MaxTrackSizingFunction::from_length(l.unitless_value()),
      LengthPercentage::Percentage(p) => taffy::MaxTrackSizingFunction::from_percent(p.0),
      LengthPercentage::Calc(_) => {
        // TODO(yorkie): support calc
        taffy::MaxTrackSizingFunction::auto()
      }
    },
    StyloSpecifiedValues::TrackBreadth::Fr(v) => taffy::MaxTrackSizingFunction::from_fr(v.clone()),
    StyloSpecifiedValues::TrackBreadth::Auto => taffy::MaxTrackSizingFunction::auto(),
    StyloSpecifiedValues::TrackBreadth::MaxContent => taffy::MaxTrackSizingFunction::max_content(),
    StyloSpecifiedValues::TrackBreadth::MinContent => taffy::MaxTrackSizingFunction::min_content(),
  }
}

#[inline]
fn track_size(input: &StyloSpecifiedValues::TrackSize) -> taffy::NonRepeatedTrackSizingFunction {
  match input {
    StyloSpecifiedValues::TrackSize::Breadth(breadth) => taffy::MinMax {
      min: min_track(breadth),
      max: max_track(breadth),
    },
    StyloSpecifiedValues::TrackSize::Minmax(min, max) => taffy::MinMax {
      min: min_track(min),
      max: max_track(max),
    },
    StyloSpecifiedValues::TrackSize::FitContent(limit) => taffy::MinMax {
      min: taffy::MinTrackSizingFunction::auto(),
      max: taffy::MaxTrackSizingFunction::fit_content(match limit {
        StyloSpecifiedValues::TrackBreadth::Breadth(lp) => length_percentage(lp),

        StyloSpecifiedValues::TrackBreadth::Fr(_) => unreachable!(),
        StyloSpecifiedValues::TrackBreadth::Auto => unreachable!(),
        StyloSpecifiedValues::TrackBreadth::MaxContent => unreachable!(),
        StyloSpecifiedValues::TrackBreadth::MinContent => unreachable!(),
      }),
    },
  }
}

#[inline]
fn track_repeat(
  count: &StyloGenericsValues::grid::RepeatCount<StyloSpecifiedValues::Integer>,
) -> taffy::GridTrackRepetition {
  match count {
    StyloGenericsValues::grid::RepeatCount::Number(v) => {
      taffy::GridTrackRepetition::Count(v.value().try_into().unwrap())
    }
    StyloGenericsValues::grid::RepeatCount::AutoFill => taffy::GridTrackRepetition::AutoFill,
    StyloGenericsValues::grid::RepeatCount::AutoFit => taffy::GridTrackRepetition::AutoFit,
  }
}

#[inline]
fn grid_template_tracks(input_str: &str) -> Vec<taffy::TrackSizingFunction> {
  use style::values::generics::grid::GenericTrackListValue;
  use style::values::specified::GridTemplateComponent;

  let parser = crate::css_parser::CSSParser::default();
  parser
    .parse_grid_template_component(input_str)
    .map(|input| match input.as_handle() {
      GridTemplateComponent::None => vec![],
      GridTemplateComponent::TrackList(list) => list
        .values
        .iter()
        .map(|track| match track {
          GenericTrackListValue::TrackSize(size) => {
            taffy::TrackSizingFunction::Single(track_size(size))
          }
          GenericTrackListValue::TrackRepeat(repeat) => taffy::TrackSizingFunction::Repeat(
            track_repeat(&repeat.count),
            repeat.track_sizes.iter().map(track_size).collect(),
          ),
        })
        .collect(),
      GridTemplateComponent::Subgrid(_) => vec![],
      GridTemplateComponent::Masonry => vec![],
    })
    .unwrap_or(vec![])
}

#[inline]
fn grid_auto_tracks(input_str: &str) -> Vec<taffy::NonRepeatedTrackSizingFunction> {
  crate::css_parser::CSSParser::default()
    .parse_grid_implicit_tracks(input_str)
    .map(|list| list.0.iter().map(track_size).collect())
    .unwrap_or(vec![])
}

#[inline]
fn grid_auto_flow(input_str: &str) -> taffy::GridAutoFlow {
  use StyloSpecifiedValues::GridAutoFlow as StyloGridAutoFlow;

  match crate::css_parser::CSSParser::default()
    .parse_grid_auto_flow(input_str)
    .bits()
  {
    x if x == StyloGridAutoFlow::ROW.bits() => taffy::GridAutoFlow::Row,
    x if x == StyloGridAutoFlow::COLUMN.bits() => taffy::GridAutoFlow::Column,
    x if x == (StyloGridAutoFlow::ROW | StyloGridAutoFlow::DENSE).bits() => {
      taffy::GridAutoFlow::RowDense
    }
    x if x == (StyloGridAutoFlow::COLUMN | StyloGridAutoFlow::DENSE).bits() => {
      taffy::GridAutoFlow::ColumnDense
    }
    _ => taffy::GridAutoFlow::Row,
  }
}

#[inline]
fn grid_line(input_str: &str) -> taffy::GridPlacement {
  use style::Zero;

  let parser = crate::css_parser::CSSParser::default();
  parser
    .parse_grid_line(input_str)
    .map(|input| {
      let input = input.as_handle();
      if input.is_auto() {
        taffy::GridPlacement::Auto
      } else if input.is_span {
        taffy::GridPlacement::Span(input.line_num.value().try_into().unwrap())
      } else if input.line_num.is_zero() {
        taffy::GridPlacement::Auto
      } else {
        taffy::style_helpers::line(input.line_num.value().try_into().unwrap())
      }
    })
    .unwrap_or(taffy::GridPlacement::Auto)
}

impl From<ffi::Style> for taffy::Style {
  fn from(value: ffi::Style) -> Self {
    taffy::Style {
      display: value.display.into(),
      box_sizing: value.box_sizing.into(),
      overflow: value.overflow.into(),
      scrollbar_width: value.scrollbar_width,
      position: value.position.into(),
      size: taffy::Size {
        width: value.width.into(),
        height: value.height.into(),
      },
      min_size: taffy::Size {
        width: value.width_range.min.into(),
        height: value.height_range.min.into(),
      },
      max_size: taffy::Size {
        width: value.width_range.max.into(),
        height: value.height_range.max.into(),
      },
      margin: value.margin.into(),
      padding: value.padding.into(),
      border: value.border.into(),
      align_items: Some(value.align_items.into()),
      align_self: value.align_self.into(),
      align_content: Some(value.align_content.into()),
      justify_items: Some(value.justify_items.into()),
      justify_self: Some(value.justify_self.into()),
      justify_content: Some(value.justify_content.into()),
      gap: value.gap.into(),
      flex_direction: value.flex_direction.into(),
      flex_wrap: value.flex_wrap.into(),
      flex_basis: value.flex_basis.into(),
      flex_grow: value.flex_grow,
      flex_shrink: value.flex_shrink,

      grid_template_rows: grid_template_tracks(&value.grid_template_rows),
      grid_template_columns: grid_template_tracks(&value.grid_template_columns),
      grid_auto_columns: grid_auto_tracks(&value.grid_auto_columns),
      grid_auto_rows: grid_auto_tracks(&value.grid_auto_rows),
      grid_auto_flow: grid_auto_flow(&value.grid_auto_flow),
      grid_row: taffy::Line {
        start: grid_line(&value.grid_row_start),
        end: grid_line(&value.grid_row_end),
      },
      grid_column: taffy::Line {
        start: grid_line(&value.grid_column_start),
        end: grid_line(&value.grid_column_end),
      },

      ..Default::default()
    }
  }
}

impl ffi::LayoutOutput {
  pub fn new(tree: &TaffyTree, node: taffy::NodeId) -> Self {
    let taffy = tree.handle.borrow();
    let layout = taffy.layout(node).expect("Failed to get layout");
    layout.clone().into()
  }
}

impl From<taffy::Layout> for ffi::LayoutOutput {
  fn from(layout: taffy::Layout) -> Self {
    Self {
      width: layout.size.width,
      height: layout.size.height,
      content_width: layout.content_size.width,
      content_height: layout.content_size.height,
      x: layout.location.x,
      y: layout.location.y,
      border: layout.border.into(),
      padding: layout.padding.into(),
    }
  }
}

fn create_allocator() -> Box<TaffyTree> {
  Box::new(TaffyTree::new())
}

fn create_node(tree: &TaffyTree) -> Box<TaffyNode> {
  Box::new(TaffyNode::new(tree))
}

fn remove_node(node: &mut TaffyNode) {
  node.remove();
}

fn add_child(parent: &mut TaffyNode, child: &TaffyNode) {
  parent.add_child(child);
}

fn remove_child(parent: &mut TaffyNode, child: &TaffyNode) {
  parent.remove_child(child);
}

fn remove_child_at_index(parent: &mut TaffyNode, index: usize) {
  parent.remove_child_at_index(index);
}

fn insert_child(parent: &mut TaffyNode, child: &TaffyNode, before_child: &TaffyNode) {
  parent.insert_child(child, before_child);
}

fn replace_child(
  parent: &mut TaffyNode,
  old_child: &TaffyNode,
  new_child: &TaffyNode,
  copy_children: bool,
) {
  parent.replace_child(old_child, new_child, copy_children);
}

fn replace_child_at_index(parent: &mut TaffyNode, index: usize, child: &TaffyNode) {
  parent.replace_child_at_index(index, child);
}

fn mark_node_dirty(node: &mut TaffyNode) {
  node.mark_dirty();
}

fn is_node_dirty(node: &TaffyNode) -> bool {
  node.is_dirty()
}

fn is_node_childless(node: &TaffyNode) -> bool {
  node.is_childless()
}

fn get_child_count(node: &TaffyNode) -> usize {
  node.child_count()
}

fn get_node_style(node: &TaffyNode) -> ffi::Style {
  node
    .tree
    .handle
    .borrow()
    .style(node.node)
    .expect("Failed to get style")
    .clone()
    .into()
}

fn set_node_style(node: &mut TaffyNode, style: ffi::Style) {
  let mut tree_handle = node.tree.handle.borrow_mut();
  let _ = tree_handle.set_style(node.node, style.into());
}

fn get_layout_output(node: &TaffyNode) -> ffi::LayoutOutput {
  ffi::LayoutOutput::new(&node.tree, node.node)
}

fn compute_layout(node: &mut TaffyNode, width: f32, height: f32) {
  node.compute_layout(width, height);
}

fn print_node(node: &mut TaffyNode) {
  node
    .print()
    .map_err(|e| {
      eprintln!("Failed to print node: {:?}", e);
    })
    .ok();
}
