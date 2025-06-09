#pragma once

#include <assert.h>
#include <array>
#include <iostream>
#include <unordered_map>
#include "./holocron_texture_atlas.autogen.hpp"

namespace crates::texture_atlas
{
  using namespace ::rust;

  class TextureAtlasLayout;
  class TextureLayout
  {
  public:
    TextureLayout(const holocron::texture_atlas::TextureLayout &layout, int atlas_width, int atlas_height)
        : id(layout.id),
          width(layout.w),
          height(layout.h),
          x(layout.x),
          y(layout.y),
          layer(layout.layer),
          atlas_width_(atlas_width),
          atlas_height_(atlas_height)
    {
    }

  public:
    friend std::ostream &operator<<(std::ostream &os, const TextureLayout &texture)
    {
      os << "Texture(" << texture.id << ") {";
      os << " w: " << texture.width << ",";
      os << " h: " << texture.height << ",";
      os << " x: " << texture.x << ",";
      os << " y: " << texture.y << ",";
      os << " layer: " << texture.layer << "}";
      return os;
    }

  public:
    inline std::array<float, 2> getUvOffset(float pad = 0.0f) const
    {
      float offset_u = static_cast<float>(x + pad) / atlas_width_;
      float offset_v = static_cast<float>(y + pad) / atlas_height_;
      return {offset_u, offset_v};
    }
    inline std::array<float, 2> getUvScale(float pad = 0.0f) const
    {
      float scale_u = static_cast<float>(width - 2.0f * pad) / atlas_width_;
      float scale_v = static_cast<float>(height - 2.0f * pad) / atlas_height_;
      assert(scale_u >= 0.0f && scale_v >= 0.0f &&
             "The scale must be non-negative.");
      return {scale_u, scale_v};
    }

  public:
    uint32_t id;
    int width;
    int height;
    int x;
    int y;
    int layer;

  private:
    int atlas_width_;
    int atlas_height_;
  };

  /**
   * GLSLPatcher is used to patch a GLSL source string.
   */
  class TextureAtlasLayout
  {
  public:
    TextureAtlasLayout(int width, int height, int maxLayerCount)
        : width_(width),
          height_(height),
          maxLayerCount_(maxLayerCount),
          allocator_(holocron::texture_atlas::createTextureAtlasLayoutAllocator(width, height, maxLayerCount))
    {
    }

  public:
    inline size_t size() const { return textures_.size(); }
    inline bool shouldDownscale(int width, int height) const
    {
      return holocron::texture_atlas::shouldDownscaleTexture(*allocator_, width, height);
    }
    inline std::shared_ptr<TextureLayout> addTexture(int width, int height)
    {
      using namespace holocron::texture_atlas;

      auto layout = addTextureToAtlas(*allocator_, width, height);
      if (layout.valid)
      {
        auto texture = std::make_shared<TextureLayout>(layout, this->width_, this->height_);
        textures_[texture->id] = texture;
        return texture;
      }
      else
      {
        return nullptr;
      }
    }
    inline void removeTexture(const TextureLayout &texture)
    {
      using namespace holocron::texture_atlas;

      removeTextureFromAtlas(*allocator_, texture.id);
      textures_.erase(texture.id);
    }

  private:
    int width_;
    int height_;
    int maxLayerCount_;
    Box<holocron::texture_atlas::TextureAtlasLayoutAllocator> allocator_;
    std::unordered_map<uint32_t, std::shared_ptr<TextureLayout>> textures_;
  };
}
