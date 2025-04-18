use style::servo::selector_parser::SelectorImpl;
use style_traits::ToCss;

type SelectorListImpl = selectors::parser::SelectorList<SelectorImpl>;
type ComponentImpl = selectors::parser::Component<SelectorImpl>;

#[derive(Clone, Debug)]
pub(crate) struct Component {
  pub tag: crate::css_parser::ffi::SelectorComponentType,
  pub combinator: Option<crate::css_parser::ffi::SelectorComponentCombinator>,
  pub name: Option<String>,
}

impl Component {
  pub fn new(handle: &ComponentImpl) -> Self {
    use crate::css_parser::ffi::SelectorComponentType;

    Self {
      tag: match handle {
        ComponentImpl::LocalName(_) => SelectorComponentType::LocalName,
        ComponentImpl::ID(_) => SelectorComponentType::ID,
        ComponentImpl::Class(_) => SelectorComponentType::Class,
        ComponentImpl::Root => SelectorComponentType::Root,
        ComponentImpl::Empty => SelectorComponentType::Empty,
        ComponentImpl::Scope => SelectorComponentType::Scope,
        ComponentImpl::Host(_) => SelectorComponentType::Host,
        _ => SelectorComponentType::Unsupported,
      },
      combinator: match handle {
        ComponentImpl::Combinator(combinator) => Some(combinator.into()),
        _ => None,
      },
      name: match handle {
        ComponentImpl::LocalName(local_name) => Some(local_name.name.to_css_string()),
        ComponentImpl::ID(id) => Some(id.to_css_string()),
        ComponentImpl::Class(class) => Some(class.to_css_string()),
        _ => None,
      },
    }
  }
}

#[derive(Clone, Debug)]
pub(crate) struct ComponentList {
  components: Vec<Component>,
}

impl ComponentList {
  pub fn new(selector_handle: &selectors::parser::Selector<SelectorImpl>) -> Self {
    let components: Vec<Component> = selector_handle
      .iter_raw_match_order()
      .map(|component| Component::new(component))
      .collect();

    assert_eq!(selector_handle.len(), components.len());
    Self { components }
  }

  pub fn len(&self) -> usize {
    self.components.len()
  }

  pub fn item(&self, index: usize) -> Option<&Component> {
    self.components.get(index).map_or(None, |c| Some(c))
  }
}

#[derive(Clone, Debug)]
pub(crate) struct Selector {
  pub components: ComponentList,
}

impl Selector {
  pub fn new(handle: &selectors::parser::Selector<SelectorImpl>) -> Self {
    Self {
      components: ComponentList::new(handle),
    }
  }
}

#[derive(Clone, Debug)]
pub(crate) struct SelectorList {
  pub items: Vec<Selector>,
}

impl SelectorList {
  pub fn new(handle: &SelectorListImpl) -> Self {
    let items: Vec<Selector> = handle.slice().iter().map(|s| Selector::new(s)).collect();

    assert_eq!(handle.len(), items.len());
    Self { items }
  }

  #[inline]
  pub fn len(&self) -> usize {
    self.items.len()
  }

  #[inline]
  pub fn is_empty(&self) -> bool {
    self.items.is_empty()
  }

  #[inline]
  pub fn item(&self, index: usize) -> Option<Selector> {
    self.items.get(index).map_or(None, |s| Some(s.clone()))
  }
}
