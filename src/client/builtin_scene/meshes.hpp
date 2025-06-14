#pragma once

#include <concepts>
#include <memory>
#include <client/graphics/webgl_context.hpp>

#include "./asset.hpp"
#include "./ecs.hpp"
#include "./mesh_base.hpp"
#include "./mesh_material.hpp"
#include "./instanced_mesh.hpp"
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
    /**
     * Construct a mesh3d with the given mesh handle.
     *
     * @param handle The mesh handle.
     * @param disableRendering Whether to disable rendering of the mesh, by default it's disabled.
     */
    Mesh3d(std::shared_ptr<Mesh> handle, bool disableRendering = true)
        : ecs::Component()
        , handle_(handle)
        , disableRendering_(disableRendering)
    {
    }

  public:
    /**
     * Get if the mesh's handle is the given type.
     *
     * @tparam MeshType The type of the mesh to check.
     * @returns If the mesh's handle is the given type.
     */
    template <typename MeshType>
      requires std::is_base_of<Mesh, MeshType>::value
    inline bool is() const
    {
      return std::dynamic_pointer_cast<MeshType>(handle_) != nullptr;
    }
    /**
     * Get if the mesh's handle is an instanced mesh.
     */
    inline bool isInstancedMesh() const
    {
      return std::dynamic_pointer_cast<InstancedMeshBase>(handle_) != nullptr;
    }
    /**
     * Get the handle of the mesh as the given type.
     *
     * @tparam MeshType The type of the mesh to get the handle as.
     * @returns The handle of the mesh as the given type.
     */
    template <typename MeshType = Mesh>
      requires std::is_same<InstancedMeshBase, MeshType>::value ||
               std::is_same<Mesh, MeshType>::value ||
               std::is_base_of<Mesh, MeshType>::value
    inline std::shared_ptr<MeshType> getHandleAs() const
    {
      if constexpr (std::is_same<MeshType, Mesh>::value)
        return handle_;
      else
        return std::dynamic_pointer_cast<MeshType>(handle_);
    }
    /**
     * Get the handle reference of the mesh as the given type, it will returns the object reference to the mesh, and
     * throws an exception if the mesh is not valid.
     *
     * @tparam MeshType The type of the mesh to get the handle as.
     * @returns The handle of the mesh as the given type.
     */
    template <typename MeshType = Mesh>
      requires std::is_same<InstancedMeshBase, MeshType>::value ||
               std::is_same<Mesh, MeshType>::value ||
               std::is_base_of<Mesh, MeshType>::value
    MeshType &getHandleCheckedAsRef() const
    {
      auto mesh = getHandleAs<MeshType>();
      if (mesh == nullptr)
        throw std::runtime_error("The mesh is not valid.");
      return *mesh;
    }
    /**
     * @returns The vertex array object.
     */
    inline std::shared_ptr<client_graphics::WebGLVertexArray> vertexArrayObject() const
    {
      return vao_;
    }
    /**
     * @returns The vertex buffer object.
     */
    inline std::shared_ptr<client_graphics::WebGLBuffer> vertexBufferObject() const
    {
      return vbo_;
    }
    /**
     * Set if the mesh3d is initialized.
     *
     * @param glContext The WebGL context.
     * @param vao The vertex array object.
     * @param vbo The vertex buffer object.
     */
    inline void initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                           std::shared_ptr<client_graphics::WebGLVertexArray> vao,
                           std::shared_ptr<client_graphics::WebGLBuffer> vbo)
    {
      if (vao == nullptr)
        throw std::runtime_error("The vertex array object is not initialized.");

      vao_ = vao;
      vbo_ = vbo;
      glContext_ = glContext;
      initialized_ = true;
    }
    /**
     * @returns If the mesh3d is initialized.
     */
    inline bool initialized()
    {
      return initialized_;
    }
    /**
     * @returns If the mesh3d needs to update the underlying vertex buffer data.
     */
    inline bool needsUpdate() const
    {
      return handle_->isDirty();
    }
    /**
     * @returns If the mesh3d is disabled for rendering.
     */
    inline bool isRenderingDisabled()
    {
      return disableRendering_;
    }
    /**
     * Disable rendering of the mesh, it causes the mesh not to be rendered.
     */
    inline void disableRendering()
    {
      disableRendering_ = true;
    }
    /**
     * Resume rendering of the mesh.
     */
    inline void resumeRendering()
    {
      disableRendering_ = false;
    }
    /**
     * @returns The primitive topology of the mesh.
     */
    inline PrimitiveTopology primitiveTopology() const
    {
      return handle_->primitiveTopology;
    }
    /**
     * @returns The indices of the mesh.
     */
    inline const Indices<uint32_t> &indices() const
    {
      return handle_->indices();
    }
    /**
     * @returns The vertex buffer of the mesh.
     */
    inline MeshVertexBuffer &vertexBuffer()
    {
      return handle_->vertexBuffer();
    }
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

      auto configureAttrib = [callback, glContext, program, stride, &offset](const IVertexAttribute &attrib)
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
    std::shared_ptr<client_graphics::WebGLBuffer> vbo_;
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool initialized_ = false;
    bool disableRendering_ = true;
  };

  class MeshBuilder
  {
  public:
    /**
     * Create an instanced mesh with the given name.
     *
     * @tparam MeshType The type of the mesh to create.
     * @tparam Args The types of the arguments to pass to the mesh
     *
     * @param name The name of the instanced mesh.
     * @param args The arguments to pass to the mesh
     * @returns The created instanced mesh.
     */
    template <typename MeshType, typename... Args>
      requires std::is_base_of<Mesh, MeshType>::value
    static inline std::shared_ptr<InstancedMesh<MeshType>> CreateInstancedMesh(const std::string &name, Args &&...args)
    {
      return CreateAndBuild<InstancedMesh<MeshType>>(name, std::forward<Args>(args)...);
    }
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
