use crate::node::{Node, NodeData};
use atomic_refcell::{AtomicRef, AtomicRefMut};
use html5ever::{local_name, LocalName, Namespace};
use selectors::{
  matching::{ElementSelectorFlags, MatchingContext, VisitedHandlingMode},
  sink::Push,
  Element, OpaqueElement,
};
use style::{
  context::{QuirksMode, SharedStyleContext, StyleContext},
  dom::{LayoutIterator, NodeInfo, OpaqueNode, TDocument, TElement, TNode, TShadowRoot},
  properties::{
    style_structs::{Box as BoxStyle, Position},
    PropertyDeclarationBlock,
  },
  selector_parser::{NonTSPseudoClass, SelectorImpl},
  servo_arc::{Arc, ArcBorrow},
  shared_lock::{Locked, SharedRwLock, StylesheetGuards},
  values::{AtomIdent, GenericAtomIdent},
  Atom, CaseSensitivityExt,
};
use style_traits::dom::ElementState;

/// A handle to a node that Servo's style traits are implemented against
///
/// Since BlitzNodes are not persistent (IE we don't keep the pointers around between frames), we choose to just implement
/// the tree structure in the nodes themselves, and temporarily give out pointers during the layout phase.
type BlitzNode<'a> = &'a Node;

impl<'a> TDocument for BlitzNode<'a> {
  type ConcreteNode = BlitzNode<'a>;

  fn as_node(&self) -> Self::ConcreteNode {
    self
  }

  fn is_html_document(&self) -> bool {
    true
  }

  fn quirks_mode(&self) -> QuirksMode {
    QuirksMode::NoQuirks
  }

  fn shared_lock(&self) -> &SharedRwLock {
    &self.guard
  }
}

impl<'a> TShadowRoot for BlitzNode<'a> {
  type ConcreteNode = BlitzNode<'a>;

  fn as_node(&self) -> Self::ConcreteNode {
    self
  }

  fn host(&self) -> <Self::ConcreteNode as TNode>::ConcreteElement {
    todo!("Shadow roots not implemented")
  }

  fn style_data<'b>(&self) -> Option<&'b style::stylist::CascadeData>
  where
    Self: 'b,
  {
    todo!("Shadow roots not implemented")
  }
}

impl<'a> NodeInfo for BlitzNode<'a> {
  fn is_element(&self) -> bool {
    Node::is_element(self)
  }

  fn is_text_node(&self) -> bool {
    Node::is_text_node(self)
  }
}

impl<'a> TNode for BlitzNode<'a> {
  type ConcreteElement = BlitzNode<'a>;
  type ConcreteDocument = BlitzNode<'a>;
  type ConcreteShadowRoot = BlitzNode<'a>;

  fn parent_node(&self) -> Option<Self> {
    self.parent.map(|id| self.with(id))
  }

  fn first_child(&self) -> Option<Self> {
    self.children.first().map(|id| self.with(*id))
  }

  fn last_child(&self) -> Option<Self> {
    self.children.last().map(|id| self.with(*id))
  }

  fn prev_sibling(&self) -> Option<Self> {
    self.backward(1)
  }

  fn next_sibling(&self) -> Option<Self> {
    self.forward(1)
  }

  fn owner_doc(&self) -> Self::ConcreteDocument {
    self.with(1)
  }

  fn is_in_document(&self) -> bool {
    true
  }

  // I think this is the same as parent_node only in the cases when the direct parent is not a real element, forcing us
  // to travel upwards
  //
  // For the sake of this demo, we're just going to return the parent node ann
  fn traversal_parent(&self) -> Option<Self::ConcreteElement> {
    self.parent_node().and_then(|node| node.as_element())
  }

  fn opaque(&self) -> OpaqueNode {
    OpaqueNode(self as *const _ as usize)
  }

  fn debug_id(self) -> usize {
    self.id
  }

  fn as_element(&self) -> Option<Self::ConcreteElement> {
    match self.raw_dom_data {
      NodeData::Element { .. } => Some(self),
      _ => None,
    }
  }

  fn as_document(&self) -> Option<Self::ConcreteDocument> {
    match self.raw_dom_data {
      NodeData::Document { .. } => Some(self),
      _ => None,
    }
  }

  fn as_shadow_root(&self) -> Option<Self::ConcreteShadowRoot> {
    todo!("Shadow roots aren't real, yet")
  }
}

pub struct Traverser<'a> {
  // dom: &'a Slab<Node>,
  parent: BlitzNode<'a>,
  child_index: usize,
}

impl<'a> Iterator for Traverser<'a> {
  type Item = BlitzNode<'a>;

  fn next(&mut self) -> Option<Self::Item> {
    let node = self.parent.children.get(self.child_index)?;
    let node = self.parent.with(*node);
    self.child_index += 1;
    Some(node)
  }
}

impl std::hash::Hash for BlitzNode<'_> {
  fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
    state.write_usize(self.id)
  }
}

impl<'a> selectors::Element for BlitzNode<'a> {
  type Impl = SelectorImpl;

  fn opaque(&self) -> selectors::OpaqueElement {
    // FIXME: this is wrong in the case where pushing new elements casuses reallocations.
    // We should see if selectors will accept a PR that allows creation from a usize
    OpaqueElement::new(self)
  }

  fn parent_element(&self) -> Option<Self> {
    TElement::traversal_parent(self)
  }

  fn parent_node_is_shadow_root(&self) -> bool {
    false
  }

  fn containing_shadow_host(&self) -> Option<Self> {
    None
  }

  fn is_pseudo_element(&self) -> bool {
    false
  }

  // These methods are implemented naively since we only threaded real nodes and not fake nodes
  // we should try and use `find` instead of this foward/backward stuff since its ugly and slow
  fn prev_sibling_element(&self) -> Option<Self> {
    let mut n = 1;
    while let Some(node) = self.backward(n) {
      if node.is_element() {
        return Some(node);
      }
      n += 1;
    }

    None
  }

  fn next_sibling_element(&self) -> Option<Self> {
    let mut n = 1;
    while let Some(node) = self.forward(n) {
      if node.is_element() {
        return Some(node);
      }
      n += 1;
    }

    None
  }

  fn first_element_child(&self) -> Option<Self> {
    let mut children = self.dom_children();
    children.find(|child| child.is_element())
  }

  fn is_html_element_in_html_document(&self) -> bool {
    true // self.has_namespace(ns!(html))
  }

  fn has_local_name(&self, local_name: &LocalName) -> bool {
    self.raw_dom_data.is_element_with_tag_name(local_name)
  }

  fn has_namespace(&self, ns: &Namespace) -> bool {
    self.element_data().expect("Not an element").name.ns == *ns
  }

  fn is_same_type(&self, _other: &Self) -> bool {
    // FIXME: implementing this correctly currently triggers a debug_assert ("Invalid cache") in selectors
    //self.local_name() == other.local_name() && self.namespace() == other.namespace()
    false
  }

  fn attr_matches(
    &self,
    _ns: &selectors::attr::NamespaceConstraint<
      &<Self::Impl as selectors::SelectorImpl>::NamespaceUrl,
    >,
    local_name: &<Self::Impl as selectors::SelectorImpl>::LocalName,
    _operation: &selectors::attr::AttrSelectorOperation<
      &<Self::Impl as selectors::SelectorImpl>::AttrValue,
    >,
  ) -> bool {
    // println!("attr matches  {}", self.id);
    let mut has_attr = false;
    self.each_attr_name(|f| {
      if f.as_ref() == local_name.as_ref() {
        has_attr = true;
      }
    });
    has_attr
  }

  fn match_non_ts_pseudo_class(
    &self,
    psuedo_class: &<Self::Impl as selectors::SelectorImpl>::NonTSPseudoClass,
    _context: &mut MatchingContext<Self::Impl>,
  ) -> bool {
    match *psuedo_class {
      NonTSPseudoClass::Active => false,
      NonTSPseudoClass::AnyLink => false,
      NonTSPseudoClass::Checked => false,
      NonTSPseudoClass::Valid => false,
      NonTSPseudoClass::Invalid => false,
      NonTSPseudoClass::Defined => false,
      NonTSPseudoClass::Disabled => false,
      NonTSPseudoClass::Enabled => false,
      NonTSPseudoClass::Focus => false,
      NonTSPseudoClass::Fullscreen => false,
      NonTSPseudoClass::Hover => self.is_hovered,
      NonTSPseudoClass::Indeterminate => false,
      NonTSPseudoClass::Lang(_) => false,
      NonTSPseudoClass::CustomState(_) => false,
      NonTSPseudoClass::Link => false,
      NonTSPseudoClass::PlaceholderShown => false,
      NonTSPseudoClass::ReadWrite => false,
      NonTSPseudoClass::ReadOnly => false,
      NonTSPseudoClass::ServoNonZeroBorder => false,
      NonTSPseudoClass::Target => false,
      NonTSPseudoClass::Visited => false,
    }
  }

  fn match_pseudo_element(
    &self,
    _pe: &<Self::Impl as selectors::SelectorImpl>::PseudoElement,
    _context: &mut MatchingContext<Self::Impl>,
  ) -> bool {
    false
  }

  fn apply_selector_flags(&self, _flags: ElementSelectorFlags) {
    // unimplemented!()
  }

  fn is_link(&self) -> bool {
    self
      .raw_dom_data
      .is_element_with_tag_name(&local_name!("a"))
  }

  fn is_html_slot_element(&self) -> bool {
    false
  }

  fn has_id(
    &self,
    id: &<Self::Impl as selectors::SelectorImpl>::Identifier,
    case_sensitivity: selectors::attr::CaseSensitivity,
  ) -> bool {
    self
      .element_data()
      .and_then(|data| data.id.as_ref())
      .map(|id_attr| case_sensitivity.eq_atom(id_attr, id))
      .unwrap_or(false)
  }

  fn has_class(
    &self,
    search_name: &<Self::Impl as selectors::SelectorImpl>::Identifier,
    case_sensitivity: selectors::attr::CaseSensitivity,
  ) -> bool {
    let class_attr = self.raw_dom_data.attr(local_name!("class"));
    if let Some(class_attr) = class_attr {
      // split the class attribute
      for pheme in class_attr.split_ascii_whitespace() {
        let atom = Atom::from(pheme);
        if case_sensitivity.eq_atom(&atom, search_name) {
          return true;
        }
      }
    }

    false
  }

  fn add_element_unique_hashes(&self, filter: &mut selectors::bloom::BloomFilter) -> bool {
    true
  }

  fn imported_part(
    &self,
    _name: &<Self::Impl as selectors::SelectorImpl>::Identifier,
  ) -> Option<<Self::Impl as selectors::SelectorImpl>::Identifier> {
    None
  }

  fn is_part(&self, _name: &<Self::Impl as selectors::SelectorImpl>::Identifier) -> bool {
    false
  }

  fn is_empty(&self) -> bool {
    self.dom_children().next().is_none()
  }

  fn is_root(&self) -> bool {
    self.parent_node().is_none()
  }

  fn has_custom_state(&self, _name: &<Self::Impl as selectors::SelectorImpl>::Identifier) -> bool {
    false
  }
}

impl<'a> TElement for BlitzNode<'a> {
  type ConcreteNode = BlitzNode<'a>;
  type TraversalChildrenIterator = Traverser<'a>;

  fn as_node(&self) -> Self::ConcreteNode {
    self
  }

  fn traversal_children(&self) -> style::dom::LayoutIterator<Self::TraversalChildrenIterator> {
    LayoutIterator(Traverser {
      // dom: self.tree(),
      parent: self,
      child_index: 0,
    })
  }

  fn is_html_element(&self) -> bool {
    self.is_element()
  }

  // not implemented.....
  fn is_mathml_element(&self) -> bool {
    false
  }

  // need to check the namespace
  fn is_svg_element(&self) -> bool {
    false
  }

  fn style_attribute(&self) -> Option<ArcBorrow<Locked<PropertyDeclarationBlock>>> {
    // self
    //   .element_data()
    //   .expect("Not an element")
    //   .style_attribute
    //   .as_ref()
    //   .map(|f| f.borrow_arc())
    None
  }

  fn animation_rule(
    &self,
    _: &SharedStyleContext,
  ) -> Option<Arc<Locked<PropertyDeclarationBlock>>> {
    None
  }

  fn transition_rule(
    &self,
    _context: &SharedStyleContext,
  ) -> Option<Arc<Locked<PropertyDeclarationBlock>>> {
    None
  }

  fn state(&self) -> ElementState {
    // todo: we should track this
    ElementState::empty()
  }

  fn has_part_attr(&self) -> bool {
    false
  }

  fn exports_any_part(&self) -> bool {
    false
  }

  fn id(&self) -> Option<&style::Atom> {
    self.element_data().and_then(|data| data.id.as_ref())
  }

  fn each_class<F>(&self, mut callback: F)
  where
    F: FnMut(&style::values::AtomIdent),
  {
    let class_attr = self.raw_dom_data.attr(local_name!("class"));
    if let Some(class_attr) = class_attr {
      // split the class attribute
      for pheme in class_attr.split_ascii_whitespace() {
        let atom = Atom::from(pheme); // interns the string
        callback(AtomIdent::cast(&atom));
      }
    }
  }

  fn each_attr_name<F>(&self, mut callback: F)
  where
    F: FnMut(&style::LocalName),
  {
    if let Some(attrs) = self.raw_dom_data.attrs() {
      for attr in attrs.iter() {
        callback(&GenericAtomIdent(attr.name.local.clone()));
      }
    }
  }

  fn has_dirty_descendants(&self) -> bool {
    true
  }

  fn has_snapshot(&self) -> bool {
    // todo: We want to implement snapshots at some point
    false
  }

  fn handled_snapshot(&self) -> bool {
    unimplemented!()
  }

  unsafe fn set_handled_snapshot(&self) {
    unimplemented!()
  }

  unsafe fn set_dirty_descendants(&self) {}

  unsafe fn unset_dirty_descendants(&self) {}

  fn store_children_to_process(&self, _n: isize) {
    unimplemented!()
  }

  fn did_process_child(&self) -> isize {
    unimplemented!()
  }

  unsafe fn ensure_data(&self) -> AtomicRefMut<style::data::ElementData> {
    // let mut stylo_data = self.stylo_element_data.borrow_mut();
    // if stylo_data.is_none() {
    //   *stylo_data = Some(Default::default());
    // }
    // AtomicRefMut::map(stylo_data, |sd| sd.as_mut().unwrap())
    todo!()
  }

  unsafe fn clear_data(&self) {
    // *self.stylo_element_data.borrow_mut() = None;
  }

  fn has_data(&self) -> bool {
    // self.stylo_element_data.borrow().is_some()
    false
  }

  fn borrow_data(&self) -> Option<AtomicRef<style::data::ElementData>> {
    // let stylo_data = self.stylo_element_data.borrow();
    // if stylo_data.is_some() {
    //   Some(AtomicRef::map(stylo_data, |sd| sd.as_ref().unwrap()))
    // } else {
    //   None
    // }
    None
  }

  fn mutate_data(&self) -> Option<AtomicRefMut<style::data::ElementData>> {
    // let stylo_data = self.stylo_element_data.borrow_mut();
    // if stylo_data.is_some() {
    //   Some(AtomicRefMut::map(stylo_data, |sd| sd.as_mut().unwrap()))
    // } else {
    //   None
    // }
    None
  }

  fn skip_item_display_fixup(&self) -> bool {
    false
  }

  fn may_have_animations(&self) -> bool {
    false
  }

  fn has_animations(&self, _context: &SharedStyleContext) -> bool {
    false
  }

  fn has_css_animations(
    &self,
    _context: &SharedStyleContext,
    _pseudo_element: Option<style::selector_parser::PseudoElement>,
  ) -> bool {
    false
  }

  fn has_css_transitions(
    &self,
    _context: &SharedStyleContext,
    _pseudo_element: Option<style::selector_parser::PseudoElement>,
  ) -> bool {
    false
  }

  fn shadow_root(&self) -> Option<<Self::ConcreteNode as TNode>::ConcreteShadowRoot> {
    None
  }

  fn containing_shadow(&self) -> Option<<Self::ConcreteNode as TNode>::ConcreteShadowRoot> {
    None
  }

  fn lang_attr(&self) -> Option<style::selector_parser::AttrValue> {
    None
  }

  fn match_element_lang(
    &self,
    _override_lang: Option<Option<style::selector_parser::AttrValue>>,
    _value: &style::selector_parser::Lang,
  ) -> bool {
    false
  }

  fn is_html_document_body_element(&self) -> bool {
    // Check node is a <body> element
    let is_body_element = self
      .raw_dom_data
      .is_element_with_tag_name(&local_name!("body"));

    // If it isn't then return early
    if !is_body_element {
      return false;
    }

    // If it is then check if it is a child of the root (<html>) element
    let root_node = &self.tree()[0];
    let root_element = TDocument::as_node(&root_node)
      .first_element_child()
      .unwrap();
    let is_child_of_root_element = root_element.children.contains(&self.id);
    is_child_of_root_element
  }

  fn synthesize_presentational_hints_for_legacy_attributes<V>(
    &self,
    _visited_handling: VisitedHandlingMode,
    _hints: &mut V,
  ) where
    V: Push<style::applicable_declarations::ApplicableDeclarationBlock>,
  {
  }

  fn local_name(&self) -> &LocalName {
    &self.element_data().expect("Not an element").name.local
  }

  fn namespace(&self) -> &Namespace {
    &self.element_data().expect("Not an element").name.ns
  }

  fn query_container_size(
    &self,
    _display: &style::values::specified::Display,
  ) -> euclid::default::Size2D<Option<app_units::Au>> {
    // FIXME: Implement container queries. For now this effectively disables them without panicking.
    Default::default()
  }

  fn each_custom_state<F>(&self, _callback: F)
  where
    F: FnMut(&AtomIdent),
  {
    todo!()
  }

  fn has_selector_flags(&self, flags: ElementSelectorFlags) -> bool {
    false
  }

  fn relative_selector_search_direction(&self) -> ElementSelectorFlags {
    ElementSelectorFlags::HAS_EMPTY_SELECTOR
  }
}
