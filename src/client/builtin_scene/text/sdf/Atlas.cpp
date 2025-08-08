#include "./Atlas.hpp"
#include <algorithm>
#include <cstring>
#include <cassert>

namespace builtin_scene::text::sdf
{
  SDFAtlas::SDFAtlas(int width, int height)
      : width_(width)
      , height_(height)
      , textureData_(width * height, 0)
      , root_(std::make_unique<PackingNode>(0, 0, width, height))
      , usedArea_(0)
  {
  }

  std::unique_ptr<GlyphUV> SDFAtlas::addGlyph(const SDFGlyph &glyph)
  {
    if (glyph.width <= 0 || glyph.height <= 0)
      return nullptr;

    PackingNode *node = findNode(root_.get(), glyph.width, glyph.height);
    if (!node)
      return nullptr;

    PackingNode *fit = splitNode(node, glyph.width, glyph.height);
    if (!fit)
      return nullptr;

    // Copy glyph data to atlas
    copyGlyphData(glyph, fit->x, fit->y);
    usedArea_ += glyph.width * glyph.height;

    // Calculate UV coordinates
    float u0 = static_cast<float>(fit->x) / width_;
    float v0 = static_cast<float>(fit->y) / height_;
    float u1 = static_cast<float>(fit->x + glyph.width) / width_;
    float v1 = static_cast<float>(fit->y + glyph.height) / height_;

    return std::make_unique<GlyphUV>(u0, v0, u1, v1, fit->x, fit->y, glyph.width, glyph.height);
  }

  std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> SDFAtlas::addGlyphs(
    const std::vector<std::unique_ptr<SDFGlyph>> &glyphs)
  {
    std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> result;

    for (const auto &glyph : glyphs)
    {
      if (!glyph)
        continue;

      auto uv = addGlyph(*glyph);
      if (uv)
        result[glyph->codepoint] = std::move(uv);
    }

    return result;
  }

  bool SDFAtlas::hasSpace(int width, int height) const
  {
    return findNode(root_.get(), width, height) != nullptr;
  }

  float SDFAtlas::getFillRatio() const
  {
    return static_cast<float>(usedArea_) / (width_ * height_);
  }

  void SDFAtlas::clear()
  {
    std::fill(textureData_.begin(), textureData_.end(), 0);
    root_ = std::make_unique<PackingNode>(0, 0, width_, height_);
    usedArea_ = 0;
  }

  SDFAtlas::PackingNode *SDFAtlas::findNode(PackingNode *node, int width, int height)
  {
    if (!node)
      return nullptr;

    if (node->used)
    {
      // Node is used, try children
      PackingNode *result = findNode(node->right.get(), width, height);
      if (result)
        return result;
      return findNode(node->down.get(), width, height);
    }
    else if (width <= node->width && height <= node->height)
    {
      // Perfect fit
      return node;
    }

    return nullptr;
  }

  SDFAtlas::PackingNode *SDFAtlas::splitNode(PackingNode *node, int width, int height)
  {
    if (!node || node->used)
      return nullptr;

    node->used = true;

    // Calculate remaining space
    int remainderHorizontal = node->width - width;
    int remainderVertical = node->height - height;

    if (remainderHorizontal > 0)
    {
      node->right = std::make_unique<PackingNode>(
        node->x + width, node->y, remainderHorizontal, node->height);
    }

    if (remainderVertical > 0)
    {
      node->down = std::make_unique<PackingNode>(
        node->x, node->y + height, width, remainderVertical);
    }

    return node;
  }

  void SDFAtlas::copyGlyphData(const SDFGlyph &glyph, int x, int y)
  {
    assert(x + glyph.width <= width_);
    assert(y + glyph.height <= height_);

    for (int row = 0; row < glyph.height; ++row)
    {
      int srcOffset = row * glyph.width;
      int dstOffset = (y + row) * width_ + x;
      std::memcpy(&textureData_[dstOffset], &glyph.data[srcOffset], glyph.width);
    }
  }

  // SDFAtlasBuilder implementation

  SDFAtlasBuilder::SDFAtlasBuilder(const SDFParams &params, int atlasWidth, int atlasHeight)
      : sdfParams_(params)
      , atlasWidth_(atlasWidth)
      , atlasHeight_(atlasHeight)
      , sdfGenerator_(params)
  {
  }

  SDFAtlasBuilder::AtlasResult SDFAtlasBuilder::createAtlasForText(const std::string &text)
  {
    AtlasResult result;

    // Extract unique codepoints from text
    auto codepoints = extractCodepoints(text);
    if (codepoints.empty())
      return result;

    // Generate SDF glyphs
    auto glyphs = sdfGenerator_.generateGlyphs(codepoints);
    if (glyphs.empty())
      return result;

    // Create atlas and pack glyphs
    result.atlas = std::make_unique<SDFAtlas>(atlasWidth_, atlasHeight_);
    result.glyphUVs = result.atlas->addGlyphs(glyphs);

    return result;
  }

  std::vector<uint32_t> SDFAtlasBuilder::extractCodepoints(const std::string &text)
  {
    std::vector<uint32_t> codepoints;

    // Simple UTF-8 to UTF-32 conversion
    // This is a basic implementation - a full implementation would handle all UTF-8 cases
    const char *str = text.c_str();
    size_t len = text.length();

    for (size_t i = 0; i < len;)
    {
      uint32_t codepoint = 0;
      unsigned char c = str[i];

      if (c < 0x80)
      {
        // Single byte character
        codepoint = c;
        i += 1;
      }
      else if ((c & 0xE0) == 0xC0)
      {
        // Two byte character
        if (i + 1 < len)
        {
          codepoint = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
          i += 2;
        }
        else
        {
          break;
        }
      }
      else if ((c & 0xF0) == 0xE0)
      {
        // Three byte character
        if (i + 2 < len)
        {
          codepoint = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
          i += 3;
        }
        else
        {
          break;
        }
      }
      else if ((c & 0xF8) == 0xF0)
      {
        // Four byte character
        if (i + 3 < len)
        {
          codepoint = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) |
                      ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
          i += 4;
        }
        else
        {
          break;
        }
      }
      else
      {
        // Invalid UTF-8, skip
        i += 1;
        continue;
      }

      // Add to list if not already present
      if (std::find(codepoints.begin(), codepoints.end(), codepoint) == codepoints.end())
      {
        codepoints.push_back(codepoint);
      }
    }

    return codepoints;
  }
}