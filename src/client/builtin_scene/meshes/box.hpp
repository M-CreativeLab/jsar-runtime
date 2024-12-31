#pragma once

#include <glm/glm.hpp>
#include "../mesh_base.hpp"

namespace builtin_scene::meshes
{
  class Box : public Mesh,
              public MeshBuilder,
              public Primitive3d
  {
  public:
    Box(float width, float height, float depth)
        : Mesh("Box", PrimitiveTopology::kTriangleList),
          width_(width),
          height_(height),
          depth_(depth)
    {
    }
    Box(float size) : Box(size, size, size)
    {
    }

  public:
    inline float width() { return width_; }
    inline float height() { return height_; }
    inline float depth() { return depth_; }

  public:
    float area() override { return 2.0f * (width_ * height_ + width_ * depth_ + height_ * depth_); }
    float volume() override { return width_ * height_ * depth_; }
    void build() override final
    {
      auto max = glm::vec3(width_ / 2.0f, height_ / 2.0f, depth_ / 2.0f);
      auto min = -max;

      size_t faces = 6;
      auto vertexCount = 4 * faces;
      std::vector<glm::vec3> positions = std::vector<glm::vec3>(vertexCount);
      std::vector<glm::vec3> normals = std::vector<glm::vec3>(vertexCount);
      std::vector<glm::vec2> uvs = std::vector<glm::vec2>(vertexCount);

      // Front
      {
        positions[0] = glm::vec3(min.x, min.y, max.z);
        positions[1] = glm::vec3(max.x, min.y, max.z);
        positions[2] = glm::vec3(max.x, max.y, max.z);
        positions[3] = glm::vec3(min.x, max.y, max.z);
        normals[0] = glm::vec3(0.0f, 0.0f, 1.0f);
        normals[1] = glm::vec3(0.0f, 0.0f, 1.0f);
        normals[2] = glm::vec3(0.0f, 0.0f, 1.0f);
        normals[3] = glm::vec3(0.0f, 0.0f, 1.0f);
        uvs[0] = glm::vec2(0.0f, 0.0f);
        uvs[1] = glm::vec2(1.0f, 0.0f);
        uvs[2] = glm::vec2(1.0f, 1.0f);
        uvs[3] = glm::vec2(0.0f, 1.0f);
      }

      // Back
      {
        positions[4] = glm::vec3(min.x, max.y, min.z);
        positions[5] = glm::vec3(max.x, max.y, min.z);
        positions[6] = glm::vec3(max.x, min.y, min.z);
        positions[7] = glm::vec3(min.x, min.y, min.z);
        normals[4] = glm::vec3(0.0f, 0.0f, -1.0f);
        normals[5] = glm::vec3(0.0f, 0.0f, -1.0f);
        normals[6] = glm::vec3(0.0f, 0.0f, -1.0f);
        normals[7] = glm::vec3(0.0f, 0.0f, -1.0f);
        uvs[4] = glm::vec2(1.0f, 0.0f);
        uvs[5] = glm::vec2(0.0f, 0.0f);
        uvs[6] = glm::vec2(0.0f, 1.0f);
        uvs[7] = glm::vec2(1.0f, 1.0f);
      }

      // Right
      {
        positions[8] = glm::vec3(max.x, min.y, min.z);
        positions[9] = glm::vec3(max.x, max.y, min.z);
        positions[10] = glm::vec3(max.x, max.y, max.z);
        positions[11] = glm::vec3(max.x, min.y, max.z);
        normals[8] = glm::vec3(1.0f, 0.0f, 0.0f);
        normals[9] = glm::vec3(1.0f, 0.0f, 0.0f);
        normals[10] = glm::vec3(1.0f, 0.0f, 0.0f);
        normals[11] = glm::vec3(1.0f, 0.0f, 0.0f);
        uvs[8] = glm::vec2(0.0f, 0.0f);
        uvs[9] = glm::vec2(1.0f, 0.0f);
        uvs[10] = glm::vec2(1.0f, 1.0f);
        uvs[11] = glm::vec2(0.0f, 1.0f);
      }

      // Left
      {
        positions[12] = glm::vec3(min.x, min.y, max.z);
        positions[13] = glm::vec3(min.x, max.y, max.z);
        positions[14] = glm::vec3(min.x, max.y, min.z);
        positions[15] = glm::vec3(min.x, min.y, min.z);
        normals[12] = glm::vec3(-1.0f, 0.0f, 0.0f);
        normals[13] = glm::vec3(-1.0f, 0.0f, 0.0f);
        normals[14] = glm::vec3(-1.0f, 0.0f, 0.0f);
        normals[15] = glm::vec3(-1.0f, 0.0f, 0.0f);
        uvs[12] = glm::vec2(1.0f, 0.0f);
        uvs[13] = glm::vec2(0.0f, 0.0f);
        uvs[14] = glm::vec2(0.0f, 1.0f);
        uvs[15] = glm::vec2(1.0f, 1.0f);
      }

      // Top
      {
        positions[16] = glm::vec3(max.x, max.y, min.z);
        positions[17] = glm::vec3(min.x, max.y, min.z);
        positions[18] = glm::vec3(min.x, max.y, max.z);
        positions[19] = glm::vec3(max.x, max.y, max.z);
        normals[16] = glm::vec3(0.0f, 1.0f, 0.0f);
        normals[17] = glm::vec3(0.0f, 1.0f, 0.0f);
        normals[18] = glm::vec3(0.0f, 1.0f, 0.0f);
        normals[19] = glm::vec3(0.0f, 1.0f, 0.0f);
        uvs[16] = glm::vec2(1.0f, 0.0f);
        uvs[17] = glm::vec2(0.0f, 0.0f);
        uvs[18] = glm::vec2(0.0f, 1.0f);
        uvs[19] = glm::vec2(1.0f, 1.0f);
      }

      // Bottom
      {
        positions[20] = glm::vec3(max.x, min.y, max.z);
        positions[21] = glm::vec3(min.x, min.y, max.z);
        positions[22] = glm::vec3(min.x, min.y, min.z);
        positions[23] = glm::vec3(max.x, min.y, min.z);
        normals[20] = glm::vec3(0.0f, -1.0f, 0.0f);
        normals[21] = glm::vec3(0.0f, -1.0f, 0.0f);
        normals[22] = glm::vec3(0.0f, -1.0f, 0.0f);
        normals[23] = glm::vec3(0.0f, -1.0f, 0.0f);
        uvs[20] = glm::vec2(0.0f, 0.0f);
        uvs[21] = glm::vec2(1.0f, 0.0f);
        uvs[22] = glm::vec2(1.0f, 1.0f);
        uvs[23] = glm::vec2(0.0f, 1.0f);
      }

      // Indices
      Indices<uint32_t> indices = {
          0, 1, 2, 2, 3, 0,       // front
          4, 5, 6, 6, 7, 4,       // back
          8, 9, 10, 10, 11, 8,    // right
          12, 13, 14, 14, 15, 12, // left
          16, 17, 18, 18, 19, 16, // top
          20, 21, 22, 22, 23, 20, // bottom
      };

      updateIndices(indices);
      insertAttribute(MeshVertexAttributeData<float, 3>::Make(Mesh::ATTRIBUTE_POSITION, positions));
      insertAttribute(MeshVertexAttributeData<float, 3>::Make(Mesh::ATTRIBUTE_NORMAL, normals));
      insertAttribute(MeshVertexAttributeData<float, 2>::Make(Mesh::ATTRIBUTE_UV0, uvs));
    }

  private:
    float width_;
    float height_;
    float depth_;
  };

  class Cube : public Box
  {
  public:
    Cube(float size) : Box(size), halfSize(size / 2.0f)
    {
    }

  public:
    inline float size() { return halfSize * 2.0f; }

  public:
    float area() override { return 6.0f * halfSize * halfSize; }
    float volume() override { return std::pow(halfSize, 3); }

  private:
    float halfSize;
  };
}
