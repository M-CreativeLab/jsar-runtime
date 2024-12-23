#pragma once

#include <memory>
#include "./asset.hpp"
#include "./ecs.hpp"
#include "./mesh_base.hpp"
#include "./mesh_material.hpp"
#include "./meshes/builder.hpp"
#include "./meshes/box.hpp"
#include "./meshes/plane.hpp"
#include "./meshes/sphere.hpp"

namespace builtin_scene
{
  class Meshes : public asset::Assets<Mesh>
  {
  public:
    using asset::Assets<Mesh>::Assets;
  };

  class Mesh3d : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    Mesh3d(std::shared_ptr<Mesh> handle)
        : ecs::Component(),
          handle(handle)
    {
    }

  public:
    std::shared_ptr<Mesh> handle = nullptr;
  };

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
    static inline std::shared_ptr<meshes::Box> CreateBox(float width, float height, float depth)
    {
      return CreateAndBuild<meshes::Box>(width, height, depth);
    }
    /**
     * Create a box mesh with the given size.
     *
     * @param size The size of the box.
     * @return The created box mesh.
     */
    static inline std::shared_ptr<meshes::Box> CreateBox(float size)
    {
      return CreateAndBuild<meshes::Box>(size);
    }
    /**
     * Create a cube mesh with the given size.
     *
     * @param size The size of the cube.
     * @return The created cube mesh.
     */
    static inline std::shared_ptr<meshes::Cube> CreateCube(float size)
    {
      return CreateAndBuild<meshes::Cube>(size);
    }
    /**
     * Create a (UV) sphere mesh with the given parameters.
     *
     * @param radius The radius of the sphere.
     * @param sectors The number of longitudinal sectors, aka horizontal resolution. The default is 32.
     * @param stacks The number of latitudinal stacks, aka vertical resolution. The default is 18.
     */
    static inline std::shared_ptr<meshes::UvSphere> CreateSphere(float radius, uint32_t sectors = 32, uint32_t stacks = 18)
    {
      return CreateAndBuild<meshes::UvSphere>(radius, sectors, stacks);
    }

  private:
    /**
     * Create a new mesh and build it.
     *
     * @tparam MeshType The type of the mesh to create.
     * @tparam Args The types of the arguments to pass to the mesh
     * @param args The arguments to pass to the mesh
     * @return The created mesh.
     */
    template <typename MeshType, typename... Args>
    static std::shared_ptr<MeshType> CreateAndBuild(Args &&...args)
    {
      auto newMesh = std::make_shared<MeshType>(std::forward<Args>(args)...);
      newMesh->build();
      return newMesh;
    }
  };
}
