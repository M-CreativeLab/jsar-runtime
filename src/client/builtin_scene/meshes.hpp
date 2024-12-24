#pragma once

#include <memory>
#include <client/graphics/webgl_context.hpp>

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
          handle_(handle)
    {
    }

  public:
    /**
     * @returns The vertex array object.
     */
    std::shared_ptr<client_graphics::WebGLVertexArray> vertexArrayObject() { return vao_; }
    /**
     * Set if the mesh3d is initialized.
     *
     * @param vao The vertex array object.
     */
    inline void initialize(std::shared_ptr<client_graphics::WebGLVertexArray> vao)
    {
      if (vao == nullptr)
        throw std::runtime_error("The vertex array object is not initialized.");

      vao_ = vao;
      initialized_ = true;
    }
    /**
     * @returns If the mesh3d is initialized.
     */
    inline bool initialized() { return initialized_; }
    /**
     * @returns The primitive topology of the mesh.
     */
    inline PrimitiveTopology primitiveTopology() const { return handle_->primitiveTopology; }
    /**
     * @returns The indices of the mesh.
     */
    inline Indices<uint32_t> indices() { return handle_->indices(); }
    /**
     * @returns The vertex attributes of the mesh.
     */
    inline std::unordered_map<MeshVertexAttributeId, std::shared_ptr<IMeshVertexAttributeData>> &attributes() { return handle_->attributes(); }

  private:
    std::shared_ptr<Mesh> handle_ = nullptr;
    std::shared_ptr<client_graphics::WebGLVertexArray> vao_;
    bool initialized_ = false;
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
