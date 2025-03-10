#pragma once

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
    TextureLayout(const holocron::texture_atlas::TextureLayout &layout, int atlasWidth, int atlasHeight)
        : id(layout.id),
          width(layout.w),
          height(layout.h),
          x(layout.x),
          y(layout.y),
          layer(layout.layer),
          atlasWidth_(atlasWidth),
          atlasHeight_(atlasHeight)
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
    inline std::array<float, 2> getUvOffset() const
    {
      return {static_cast<float>(x) / atlasWidth_,
              (static_cast<float>(y)) / atlasHeight_};
    }
    inline std::array<float, 2> getUvScale() const
    {
      return {static_cast<float>(width) / atlasWidth_,
              static_cast<float>(height) / atlasHeight_};
    }

  public:
    uint32_t id;
    int width;
    int height;
    int x;
    int y;
    int layer;

  private:
    int atlasWidth_;
    int atlasHeight_;
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
