#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "../mesh_base.hpp"

namespace builtin_scene
{
  namespace meshes
  {
    template <typename BuilderType>
    class Sphere : public Mesh,
                   public MeshBuilder,
                   public Primitive3d
    {
      friend BuilderType;

    public:
      Sphere(float radius)
          : Mesh("Sphere", PrimitiveTopology::kTriangles)
          , radius_(radius)
      {
      }
      virtual ~Sphere() = default;

    public:
      inline float radius()
      {
        return radius_;
      }
      inline float diameter()
      {
        return radius_ * 2.0f;
      }

    public:
      float area() override
      {
        return 4.0f * glm::pi<float>() * std::pow(radius_, 2);
      }
      float volume() override
      {
        return (4.0f / 3.0f) * glm::pi<float>() * std::pow(radius_, 3);
      }
      void build() override = 0;

    private:
      float radius_;
    };

    class IcoSphere : public Sphere<IcoSphere>
    {
    public:
      IcoSphere(float radius, uint32_t subdivisions)
          : Sphere<IcoSphere>(radius)
          , subdivisions_(subdivisions)
      {
      }

    public:
      void build() override
      {
        throw std::runtime_error("IcoSphere: Not implemented");
      }

    private:
      uint32_t subdivisions_;
    };

    class UvSphere : public Sphere<UvSphere>
    {
    public:
      /**
       * Creates a UV sphere `Mesh` with the given number of longitudinal sectors and latitudinal stacks, aka horizontal and vertical resolution.
       *
       * A good default is 32 sectors and 18 stacks.
       *
       * @param radius The radius of the sphere.
       * @param sectors The number of longitudinal sectors.
       * @param stacks The number of latitudinal stacks.
       */
      UvSphere(float radius, uint32_t sectors, uint32_t stacks)
          : Sphere<UvSphere>(radius)
          , sectors_(sectors)
          , stacks_(stacks)
      {
      }

    public:
      void build() override
      {
        float sectors = sectors_;
        float stacks = stacks_;
        auto invLength = 1.0f / radius();
        auto sectorStep = 2.0f * glm::pi<float>() / sectors;
        auto stackStep = glm::pi<float>() / stacks;

        size_t vertexCount = stacks * sectors;
        Indices<uint32_t> indices(vertexCount * 2 * 3);

        for (uint32_t i = 0; i <= stacks; i++)
        {
          float stackAngle = glm::pi<float>() / 2.0f - (static_cast<float>(i) * stackStep);
          float xy = radius() * std::cos(stackAngle);
          float z = radius() * std::sin(stackAngle);

          for (uint32_t j = 0; j <= sectors; j++)
          {
            float sectorAngle = static_cast<float>(j) * sectorStep;
            float x = xy * std::cos(sectorAngle);
            float y = xy * std::sin(sectorAngle);

            auto pos = glm::vec3(x, y, z);
            auto u = static_cast<float>(j) / sectors;
            auto v = static_cast<float>(i) / stacks;
            insertVertex(pos, pos * invLength, glm::vec2(u, v));
          }
        }

        // indices
        //  k1--k1+1
        //  |  / |
        //  | /  |
        //  k2--k2+1
        for (uint32_t i = 0; i < stacks; i++)
        {
          uint32_t k1 = i * (sectors + 1);
          uint32_t k2 = k1 + sectors + 1;

          for (uint32_t j = 0; j < sectors; j++, k1++, k2++)
          {
            if (i != 0)
            {
              indices.push_back(k1);
              indices.push_back(k2);
              indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1))
            {
              indices.push_back(k1 + 1);
              indices.push_back(k2);
              indices.push_back(k2 + 1);
            }
          }
        }

        updateIndices(indices);
        enableAttribute(Vertex::ATTRIBUTE_POSITION);
        enableAttribute(Vertex::ATTRIBUTE_NORMAL);
        enableAttribute(Vertex::ATTRIBUTE_UV0);
      }

    private:
      uint32_t sectors_;
      uint32_t stacks_;
    };
  }
}
