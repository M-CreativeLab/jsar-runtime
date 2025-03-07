use euclid::default::Size2D;
use guillotiere::*;

#[derive(Debug, Clone)]
pub(crate) struct TextureLayout {
  pub id: u32,
  pub w: i32,
  pub h: i32,
  pub x: i32,
  pub y: i32,
  pub layer: u32,
}

impl From<guillotiere::Allocation> for TextureLayout {
  fn from(rect: guillotiere::Allocation) -> Self {
    Self {
      id: rect.id.serialize(),
      w: rect.rectangle.width(),
      h: rect.rectangle.height(),
      x: rect.rectangle.min.x,
      y: rect.rectangle.min.y,
      layer: 0,
    }
  }
}

pub(crate) struct TextureAtlasLayoutAllocator {
  pub size: Size2D<i32>,
  textures: Vec<TextureLayout>,
  layers: Vec<AtlasAllocator>,
}

impl TextureAtlasLayoutAllocator {
  pub fn new(w: i32, h: i32, layers_len: u32) -> Self {
    let size: Size2D<i32> = size2(w, h);
    Self {
      size,
      textures: Vec::new(),
      layers: (0..layers_len).map(|_| AtlasAllocator::new(size)).collect(),
    }
  }

  pub fn len(&self) -> usize {
    self.textures.len()
  }

  pub fn add_texture(&mut self, w: i32, h: i32) -> Option<TextureLayout> {
    if (w > self.size.width) || (h > self.size.height) {
      return None;
    }
    let mut index = 0;
    let layout = self.layers.iter_mut().find_map(|layer| {
      let current_index = index;
      index += 1;
      layer.allocate(size2(w, h)).map(|allocation| {
        let mut layout: TextureLayout = allocation.into();
        layout.layer = current_index;
        layout
      })
    })?;
    self.textures.push(layout.clone());
    Some(layout)
  }

  pub fn remove_texture(&mut self, id: u32) {
    let index = self.textures.iter().position(|texture| texture.id == id);
    if index.is_some() {
      let texture = self.textures.remove(index.unwrap());
      self.layers[texture.layer as usize].deallocate(AllocId::deserialize(id));
    }
  }

  pub fn should_downscale(&self, w: i32, h: i32) -> bool {
    (w > self.size.width) || (h > self.size.height)
  }
}

#[cxx::bridge(namespace = "holocron::texture_atlas")]
mod ffi {
  struct TextureLayout {
    pub valid: bool,
    pub id: u32,
    pub w: i32,
    pub h: i32,
    pub x: i32,
    pub y: i32,
    pub layer: u32,
  }

  extern "Rust" {
    type TextureAtlasLayoutAllocator;

    #[cxx_name = "createTextureAtlasLayoutAllocator"]
    unsafe fn create_texture_atlas_layout_allocator(
      w: i32,
      h: i32,
      layers_len: u32,
    ) -> Box<TextureAtlasLayoutAllocator>;

    #[cxx_name = "getTextureAtlasChildrenCount"]
    unsafe fn get_texture_atlas_children_count(texture_atlas: &TextureAtlasLayoutAllocator);

    #[cxx_name = "addTextureToAtlas"]
    unsafe fn add_texture_to_atlas(
      texture_atlas: &mut TextureAtlasLayoutAllocator,
      w: i32,
      h: i32,
    ) -> TextureLayout;

    #[cxx_name = "removeTextureFromAtlas"]
    unsafe fn remove_texture_from_atlas(texture_atlas: &mut TextureAtlasLayoutAllocator, id: u32);

    #[cxx_name = "shouldDownscaleTexture"]
    unsafe fn should_downscale_texture(
      texture_atlas: &TextureAtlasLayoutAllocator,
      w: i32,
      h: i32,
    ) -> bool;
  }
}

fn create_texture_atlas_layout_allocator(
  w: i32,
  h: i32,
  layers_len: u32,
) -> Box<TextureAtlasLayoutAllocator> {
  Box::new(TextureAtlasLayoutAllocator::new(w, h, layers_len))
}

fn get_texture_atlas_children_count(texture_atlas: &TextureAtlasLayoutAllocator) {
  texture_atlas.len();
}

fn add_texture_to_atlas(
  texture_atlas: &mut TextureAtlasLayoutAllocator,
  w: i32,
  h: i32,
) -> ffi::TextureLayout {
  let layout = texture_atlas.add_texture(w, h);
  match layout {
    Some(layout) => ffi::TextureLayout {
      valid: true,
      id: layout.id,
      w: layout.w,
      h: layout.h,
      x: layout.x,
      y: layout.y,
      layer: layout.layer,
    },
    None => ffi::TextureLayout {
      valid: false,
      id: 0,
      w: 0,
      h: 0,
      x: 0,
      y: 0,
      layer: 0,
    },
  }
}

fn remove_texture_from_atlas(texture_atlas: &mut TextureAtlasLayoutAllocator, id: u32) {
  texture_atlas.remove_texture(id);
}

fn should_downscale_texture(texture_atlas: &TextureAtlasLayoutAllocator, w: i32, h: i32) -> bool {
  texture_atlas.should_downscale(w, h)
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_texture_altas() {
    let mut texture_atlas = TextureAtlasLayoutAllocator::new(1024, 1024, 4);
    let sub_texture = texture_atlas.add_texture(1920, 1080);
    if sub_texture.is_none() {
      assert_eq!(texture_atlas.should_downscale(1920, 1080), true);
    }
    assert_eq!(texture_atlas.len(), 0);

    let sub_texture_a = texture_atlas.add_texture(512, 512);
    assert_eq!(sub_texture_a.is_some(), true);
    let sub_texture = texture_atlas.add_texture(32, 32);
    assert_eq!(sub_texture.is_some(), true);
    let sub_texture = texture_atlas.add_texture(1024, 512);
    assert_eq!(sub_texture.is_some(), true);
    assert_eq!(sub_texture.unwrap().layer, 1);
    assert_eq!(texture_atlas.len(), 3);

    texture_atlas.remove_texture(sub_texture_a.unwrap().id);
    assert_eq!(texture_atlas.len(), 2);
    {
      let sub_texture = texture_atlas.add_texture(1024, 512);
      assert_eq!(sub_texture.is_some(), true);
      assert_eq!(sub_texture.unwrap().layer, 0);
      assert_eq!(texture_atlas.len(), 3);
    }
  }
}
