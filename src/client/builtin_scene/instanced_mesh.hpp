#pragma once

#include <concepts>
#include <unordered_map>
#include <shared_mutex>
#include <glm/glm.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./ecs.hpp"
#include "./meshes/builder.hpp"
#include "./mesh_base.hpp"

namespace builtin_scene
{
  class Mesh3d;

  struct Instance
  {
    Instance()
        : row1(1.0f, 0.0f, 0.0f, 0.0f),
          row2(0.0f, 1.0f, 0.0f, 0.0f),
          row3(0.0f, 0.0f, 1.0f, 0.0f),
          row4(0.0f, 0.0f, 0.0f, 1.0f),
          color(1.0f, 1.0f, 1.0f, 1.0f),
          texUvOffset(0.0f, 0.0f),
          texUvScale(1.0f, 1.0f),
          texLayerIndex(0)
    {
    }

    void translate(float tx, float ty, float tz)
    {
      row4.x += tx;
      row4.y += ty;
      row4.z += tz;
    }
    void scale(float sx, float sy, float sz)
    {
      row1.x *= sx;
      row2.y *= sy;
      row3.z *= sz;
    }
    void setTransform(const glm::mat4 &transformationMatrix)
    {
      row1 = transformationMatrix[0];
      row2 = transformationMatrix[1];
      row3 = transformationMatrix[2];
      row4 = transformationMatrix[3];
    }

    glm::vec4 row1;        /** 4 */
    glm::vec4 row2;        /** 8 */
    glm::vec4 row3;        /** 12 */
    glm::vec4 row4;        /** 16 */
    glm::vec4 color;       /** 20 */
    glm::vec2 texUvOffset; /** 22 */
    glm::vec2 texUvScale;  /** 24 */
    uint32_t texLayerIndex;
  };

  class InstancedMeshBase
  {
    friend class Renderer;

  public:
    static constexpr size_t STRIDE = sizeof(float) * 24 + sizeof(uint32_t) * 1;
    static inline std::vector<std::string> INSTANCE_ATTRIBUTES = {"instanceTransform",
                                                                  "instanceColor",
                                                                  "instanceTexUvOffset",
                                                                  "instanceTexUvScale",
                                                                  "instanceLayerIndex"};

  public:
    InstancedMeshBase() = default;
    virtual ~InstancedMeshBase() = default;

  public:
    /**
     * Iterate the instance attributes with the given WebGL program.
     *
     * @param program The WebGL program to iterate the instance attributes with.
     * @param callback The callback to call for each instance attribute.
     * @returns The number of instance attributes.
     */
    size_t iterateInstanceAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                     std::function<void(const IVertexAttribute &, int, size_t, size_t)> callback) const;
    /**
     * Get the instance count of this mesh.
     *
     * @returns The instance count.
     */
    int instanceCount() const;
    /**
     * Iterate the instances with the given callback.
     */
    void iterateInstances(std::function<bool(ecs::EntityId, Instance &)> callback);
    /**
     * Get the instance with the given entity id.
     *
     * @param id The entity id of the instance.
     * @returns The instance reference with the given entity id.
     */
    Instance &getInstance(ecs::EntityId id);
    /**
     * Get the instance with the given entity id.
     *
     * @param id The entity id of the instance.
     * @returns The `const` instance reference with the given entity id.
     */
    const Instance &getInstance(ecs::EntityId id) const;
    /**
     * Add a new instance to the mesh.
     *
     * @param id The entity id of the instance.
     * @throws std::invalid_argument If the instance with the given entity id already exists.
     */
    Instance &addInstance(ecs::EntityId id);
    /**
     * Remove the instance with the given entity id.
     */
    bool removeInstance(ecs::EntityId id);
    /**
     * Set the transformation matrix of the instance with the given entity id.
     */
    inline void setInstanceTransform(ecs::EntityId id, const glm::mat4 &transformationMatrix)
    {
      getInstance(id).setTransform(transformationMatrix);
    }

  protected:
    /**
     * Setup the instanced mesh with the given instance VBO.
     *
     * @param glContext The WebGL2 context to setup.
     * @param instanceVbo The instance VBO to setup.
     */
    void setup(std::shared_ptr<client_graphics::WebGL2Context> glContext,
               std::shared_ptr<client_graphics::WebGLBuffer> instanceVbo);
    std::vector<Instance> instancesArray() const;
    /**
     * Upload the instance data to the instance VBO if there are any changes.
     *
     * @returns Whether the instance data is uploaded.
     */
    bool uploadInstanceData();

  private:
    inline void markAsDirty() { isDirty_ = true; }

  protected:
    mutable std::shared_mutex mutex_;
    std::shared_ptr<client_graphics::WebGLBuffer> instanceVbo_;
    std::unordered_map<ecs::EntityId, Instance> idToInstanceMap_;

  private:
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool isDirty_ = true;
  };

  /**
   * An instanced mesh which is a collection of sub-meshes that can be rendered together.
   */
  template <typename MeshType>
    requires std::is_base_of<Mesh, MeshType>::value
  class InstancedMesh final : public InstancedMeshBase,
                              public MeshType

  {
  public:
    template <typename... InitMeshArgs>
    InstancedMesh(const std::string &name, InitMeshArgs &&...args)
        : InstancedMeshBase(),
          MeshType(std::forward<InitMeshArgs>(args)...)
    {
    }

  public:
    float area() override { return 0.0f; }
    float volume() override { return 0.0f; }
  };
}
