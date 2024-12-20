#pragma once

#include <memory>
#include "../mesh_base.hpp"
#include "./box.hpp"
#include "./sphere.hpp"
#include "./plane.hpp"

namespace builtin_scene::meshes
{
  class MeshBuilder
  {
  public:
    /**
     * Create a box mesh with the given width, height, and depth.
     * 
     * @param width The width of the box.
     * @param height The height of the box.
     * @param depth The depth of the box.
     * @return The created box mesh.
     */
    static inline std::shared_ptr<Box> CreateBox(float width, float height, float depth)
    {
      return std::make_shared<Box>(width, height, depth);
    }
    /**
     * Create a box mesh with the given size.
     * 
     * @param size The size of the box.
     * @return The created box mesh.
     */
    static inline std::shared_ptr<Box> CreateBox(float size)
    {
      return std::make_shared<Box>(size);
    }
    /**
     * Create a cube mesh with the given size.
     * 
     * @param size The size of the cube.
     * @return The created cube mesh.
     */
    static inline std::shared_ptr<Cube> CreateCube(float size)
    {
      return std::make_shared<Cube>(size);
    }
  };
}
