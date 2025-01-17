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
     * @param glContext The WebGL context.
     * @param vao The vertex array object.
     */
    inline void initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                           std::shared_ptr<client_graphics::WebGLVertexArray> vao)
    {
      if (vao == nullptr)
        throw std::runtime_error("The vertex array object is not initialized.");

      vao_ = vao;
      glContext_ = glContext;
      initialized_ = true;
    }
    /**
     * @returns If the mesh3d is initialized.
     */
    inline bool initialized() { return initialized_; }
    /**
     * @returns If the mesh3d is disabled for rendering.
     */
    inline bool isRenderingDisabled() { return disableRendering_; }
    /**
     * Disable rendering of the mesh, it causes the mesh not to be rendered.
     */
    inline void disableRendering() { disableRendering_ = true; }
    /**
     * Resume rendering of the mesh.
     */
    inline void resumeRendering() { disableRendering_ = false; }
    /**
     * @returns The primitive topology of the mesh.
     */
    inline PrimitiveTopology primitiveTopology() const { return handle_->primitiveTopology; }
    /**
     * @returns The indices of the mesh.
     */
    inline const Indices<uint32_t> &indices() const { return handle_->indices(); }
    /**
     * @returns The vertex buffer of the mesh.
     */
    inline MeshVertexBuffer &vertexBuffer() { return handle_->vertexBuffer(); }
    /**
     * Iterate the enabled attributes of the mesh.
     *
     * @param callback The callback to call for each attribute.
     * @returns The number of enabled attributes.
     */
    inline size_t iterateEnabledAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                           std::function<void(const IVertexAttribute &, int, size_t, size_t)> callback)
    {
      auto glContext = glContext_.lock();
      assert(glContext != nullptr);
      size_t stride = handle_->attributesStride();
      size_t offset = 0;

      auto configureAttrib = [callback, glContext, program,
                              stride, &offset](const IVertexAttribute &attrib)
      {
        auto loc = glContext->getAttribLocation(program, attrib.name());
        if (loc.has_value())
          callback(attrib, loc.value(), stride, offset);
        offset += attrib.byteLength();
      };
      return handle_->iterateEnabledAttributes(configureAttrib);
    }

  private:
    std::shared_ptr<Mesh> handle_ = nullptr;
    std::shared_ptr<client_graphics::WebGLVertexArray> vao_;
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool initialized_ = false;
    bool disableRendering_ = false;
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
     * Create a plane mesh with the given normal and half size.
     *
     * @param normal The normal of the plane.
     * @param halfSize The half size of the plane.
     * @return The created plane mesh.
     */
    static inline std::shared_ptr<meshes::Plane> CreatePlane(math::Dir3 normal, glm::vec2 halfSize)
    {
      return CreateAndBuild<meshes::Plane>(normal, halfSize);
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
