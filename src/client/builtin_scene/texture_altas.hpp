#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <crates/bindings.texture_atlas.hpp>
#include <client/graphics/webgl_texture.hpp>
#include <client/graphics/webgl_context.hpp>
#include <idgen.hpp>

namespace builtin_scene
{
  using Texture = crates::texture_atlas::TextureLayout;

  class TextureAtlas
  {
    static constexpr int kMaxLayerCount = 4;
    static constexpr int kDefaultSize = 1024;

  public:
    TextureAtlas(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                 client_graphics::WebGLTextureUnit unit = client_graphics::WebGLTextureUnit::kTexture0,
                 int width = kDefaultSize, int height = kDefaultSize);
    ~TextureAtlas();

  public:
    std::shared_ptr<Texture> addTexture(int width, int height, bool autoDownscale = false);
    std::shared_ptr<Texture> resizeTexture(std::shared_ptr<Texture> texture, int width, int height, bool autoDownscale = false);
    void removeTexture(const Texture &texture);
    void updateTexture(const Texture &texture, const unsigned char *pixels);

  public:
    void onBeforeDraw();
    void onAfterDraw();

  private:
    int width_;
    int height_;
    client_graphics::WebGLTextureUnit unit_;
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_graphics::WebGLTexture> glTexture_;
    std::unique_ptr<crates::texture_atlas::TextureAtlasLayout> handle_;
  };
}
