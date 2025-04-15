use paste::paste;
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
        .new_leaf(taffy::Style::default())
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
    self.tree.handle.borrow().child_count(self.node)
  }

  pub fn is_childless(&self) -> bool {
    self.child_count() == 0
  }

  pub fn compute_layout(&mut self, width: f32, height: f32) {
    let mut tree = self.tree.handle.borrow_mut();
    let _ = tree.compute_layout(
      self.node,
      taffy::geometry::Size {
        width: taffy::style::AvailableSpace::Definite(width),
        height: taffy::style::AvailableSpace::Definite(height),
      },
    );
  }

  pub fn print(&mut self) {
    self.tree.handle.borrow_mut().print_tree(self.node)
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

  #[derive(Clone, Copy, Debug)]
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
    match value {
      taffy::LengthPercentage::Length(length) => Self::length(length),
      taffy::LengthPercentage::Percent(percent) => Self::percentage(percent),
    }
  }
}

impl From<ffi::LengthPercentage> for taffy::LengthPercentage {
  fn from(value: ffi::LengthPercentage) -> Self {
    match value.tag {
      ffi::LengthPercentageTag::Length => taffy::LengthPercentage::Length(value.length.value),
      ffi::LengthPercentageTag::Percentage => {
        taffy::LengthPercentage::Percent(value.percentage.percent)
      }
      _ => unreachable!(),
    }
  }
}

impl From<taffy::LengthPercentageAuto> for ffi::LengthPercentageAuto {
  fn from(value: taffy::LengthPercentageAuto) -> Self {
    match value {
      taffy::LengthPercentageAuto::Length(length) => Self::length(length),
      taffy::LengthPercentageAuto::Percent(percent) => Self::percentage(percent),
      taffy::LengthPercentageAuto::Auto => Self::auto(),
    }
  }
}

impl From<ffi::LengthPercentageAuto> for taffy::LengthPercentageAuto {
  fn from(value: ffi::LengthPercentageAuto) -> Self {
    match value.tag {
      ffi::LengthPercentageAutoTag::Length => {
        taffy::LengthPercentageAuto::Length(value.length.value)
      }
      ffi::LengthPercentageAutoTag::Percentage => {
        taffy::LengthPercentageAuto::Percent(value.percentage.percent)
      }
      ffi::LengthPercentageAutoTag::Auto => taffy::LengthPercentageAuto::Auto,
      _ => unreachable!(),
    }
  }
}

impl From<taffy::Dimension> for ffi::Dimension {
  fn from(value: taffy::Dimension) -> Self {
    match value {
      taffy::Dimension::Length(length) => Self::length(length),
      taffy::Dimension::Percent(percent) => Self::percentage(percent),
      taffy::Dimension::Auto => Self::auto(),
    }
  }
}

impl From<ffi::Dimension> for taffy::Dimension {
  fn from(value: ffi::Dimension) -> Self {
    match value.tag {
      ffi::DimensionTag::Length => taffy::Dimension::Length(value.length.value),
      ffi::DimensionTag::Percentage => taffy::Dimension::Percent(value.percentage.percent),
      ffi::DimensionTag::Auto => taffy::Dimension::Auto,
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
    }
  }
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
  let _ = node
    .tree
    .handle
    .borrow_mut()
    .set_style(node.node, style.into());
}

fn get_layout_output(node: &TaffyNode) -> ffi::LayoutOutput {
  ffi::LayoutOutput::new(&node.tree, node.node)
}

fn compute_layout(node: &mut TaffyNode, width: f32, height: f32) {
  node.compute_layout(width, height);
}

fn print_node(node: &mut TaffyNode) {
  node.print();
}
