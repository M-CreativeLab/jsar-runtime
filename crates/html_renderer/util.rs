use crate::node::{Node, NodeData};

/// It prints a RcDom tree for debugging purpose
pub fn walk_tree(indent: usize, node: &Node) {
  // Skip all-whitespace text nodes entirely
  if let NodeData::Text(data) = &node.raw_dom_data {
    if data.content.chars().all(|c| c.is_ascii_whitespace()) {
      return;
    }
  }

  print!("{}", " ".repeat(indent));
  match &node.raw_dom_data {
    NodeData::Document => println!("#Document"),
    NodeData::Text(data) => {
      if data.content.chars().all(|c| c.is_ascii_whitespace()) {
        println!("#text: <whitespace>");
      } else {
        let content = data.content.trim();
        let max_text_len = 30;
        if content.len() > max_text_len {
          println!(
            "#text: {}...",
            content
              .split_at(content.char_indices().take(max_text_len).last().unwrap().0)
              .0
              .escape_default()
          )
        } else {
          println!("#text: {}", data.content.trim().escape_default())
        }
      }
    }
    NodeData::Comment => println!("<!-- COMMENT -->"),
    NodeData::AnonymousBlock(_) => println!("AnonymousBlock"),
    NodeData::Element(data) => {
      print!("<{}", data.name.local);
      for attr in data.attrs.iter() {
        print!(" {}=\"{}\"", attr.name.local, attr.value);
      }
      if !node.children.is_empty() {
        println!(">");
      } else {
        println!("/>");
      }
    }
    NodeData::ProcessingInstruction { .. } => unreachable!(),
  }

  if !node.children.is_empty() {
    for child_id in node.children.iter() {
      walk_tree(indent + 2, node.with(*child_id));
    }
    if let NodeData::Element(data) = &node.raw_dom_data {
      println!("{}</{}>", " ".repeat(indent), data.name.local);
    }
  }
}
