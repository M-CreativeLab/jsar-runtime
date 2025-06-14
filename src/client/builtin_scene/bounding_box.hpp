#pragma once

#include <glm/glm.hpp>

namespace builtin_scene
{
  /**
   * The component representing the bounding box of an entity.
   *
   * NOTE: This component used the pixel unit for the length storage, remember to convert it to the meter unit when to use it
   *       in drawing or physics calculations.
   */
  class BoundingBox : public ecs::Component
  {
  public:
    BoundingBox()
        : ecs::Component()
        , width_(0.0f)
        , height_(0.0f)
        , depth_(0.0f)
    {
    }
    BoundingBox(float width, float height, float depth)
        : ecs::Component()
        , width_(width)
        , height_(height)
        , depth_(depth)
    {
      assert(width_ >= 0.0f);
      assert(height_ >= 0.0f);
      assert(depth_ >= 0.0f);
    }
    BoundingBox(glm::vec3 min, glm::vec3 max)
        : BoundingBox(max.x - min.x, max.y - min.y, max.z - min.z)
    {
    }

  public:
    // The width in pixel.
    inline float width() const
    {
      return width_;
    }
    // The height in pixel.
    inline float height() const
    {
      return height_;
    }
    // The depth in pixel.
    inline float depth() const
    {
      return depth_;
    }
    // The size in pixel.
    inline glm::vec3 size() const
    {
      return glm::vec3(width_, height_, depth_);
    }
    // The min point of the bounding box.
    inline glm::vec3 min() const
    {
      return glm::vec3(-width_ / 2.0f, -height_ / 2.0f, -depth_ / 2.0f);
    }
    // The max point of the bounding box.
    inline glm::vec3 max() const
    {
      return glm::vec3(width_ / 2.0f, height_ / 2.0f, depth_ / 2.0f);
    }

    /**
     * Update the size of the bounding box.
     *
     * @param width The width in pixel.
     * @param height The height in pixel.
     * @param depth The depth in pixel.
     */
    void updateSize(float width, float height, float depth)
    {
      width_ = width;
      height_ = height;
      depth_ = depth;
    }
    /**
     * Update the size of the bounding box using a 3D vector.
     * 
     * @param size The size in pixel.
     */
    inline void updateSize(const glm::vec3 &size)
    {
      updateSize(size.x, size.y, size.z);
    }

    glm::vec3 diff(const BoundingBox &other) const
    {
      return glm::vec3(width_ - other.width_, height_ - other.height_, depth_ - other.depth_);
    }

  public:
    /**
     * Calculate the difference between this bounding box and the other, such as: `bbox1 - bbox2` means the difference between `bbox1`
     * and `bbox2`.
     *
     * @returns The difference between this bounding box and the other.
     */
    glm::vec3 operator-(const BoundingBox &other) const
    {
      return diff(other);
    }

  private:
    float width_;
    float height_;
    float depth_;
  };
}
