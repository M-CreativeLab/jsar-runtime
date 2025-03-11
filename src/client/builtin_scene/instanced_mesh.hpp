#pragma once

#include <concepts>
#include <unordered_map>
#include <shared_mutex>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./ecs.hpp"
#include "./meshes/builder.hpp"
#include "./mesh_base.hpp"

namespace builtin_scene
{
  // Forward declarations
  class Mesh3d;
  class RenderableInstancesList;

  struct InstanceData
  {
    InstanceData()
        : transform(1.0f),
          color(1.0f, 1.0f, 1.0f, 0.0f),
          texUvOffset(0.0f, 0.0f),
          texUvScale(1.0f, 1.0f),
          texLayerIndex(0)
    {
    }
    glm::mat4 transform;   /** 16 */
    glm::vec4 color;       /** 20 */
    glm::vec2 texUvOffset; /** 22 */
    glm::vec2 texUvScale;  /** 24 */
    uint32_t texLayerIndex;
  };

  class Instance
  {
    friend class InstancedMeshBase;
    friend class RenderableInstancesList;

  public:
    Instance() = default;

  public:
    void randomColor();
    bool setColor(const glm::vec4 &color, bool &hasChanged);
    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void setTransform(const glm::mat4 &transformationMatrix);
    void setTexture(std::array<float, 2> uvOffset,
                    std::array<float, 2> uvScale,
                    uint32_t layerIndex,
                    bool &hasChanged);
    void disableTexture(bool &hasChanged);

#define IMPL_SETTER(NAME, PRIV_FIELD, TYPE) \
  inline bool set##NAME(TYPE value)         \
  {                                         \
    if (PRIV_FIELD != value)                \
    {                                       \
      PRIV_FIELD = value;                   \
      notifyHolders();                      \
      return true;                          \
    }                                       \
    else                                    \
      return false;                         \
  }
#define IMPL_BOOL_SETTER(NAME, PRIV_FIELD) \
  IMPL_SETTER(NAME, PRIV_FIELD, bool)

    IMPL_BOOL_SETTER(Enabled, enabled_)
    IMPL_BOOL_SETTER(Opaque, isOpaque_)
    IMPL_SETTER(ZIndex, zIndex_, uint32_t)
#undef IMPL_BOOL_SETTER
#undef IMPL_SETTER

  private:
    // Add a holder to the instance.
    void addHolder(std::shared_ptr<RenderableInstancesList> holder);
    // Remove a holder from the instance.
    void removeHolder(std::shared_ptr<RenderableInstancesList> holder);
    // Notify the holders that the instance data is updated.
    void notifyHolders();

  private:
    InstanceData data_;
    bool enabled_ = false;
    bool isOpaque_ = false;
    uint32_t zIndex_ = 0;

  private:
    std::vector<std::weak_ptr<RenderableInstancesList>> holders_;
  };

  enum class InstanceFilter
  {
    kAll,
    kOpaque,
    kTransparent
  };

  using InstanceMap = std::unordered_map<ecs::EntityId, std::shared_ptr<Instance>>;
  class RenderableInstancesList : public std::enable_shared_from_this<RenderableInstancesList>
  {
    friend class Instance;

  public:
    /**
     * The sorting order of the instances.
     */
    enum SortingOrder
    {
      kNone,
      kFrontToBack,
      kBackToFront
    };

  public:
    RenderableInstancesList(InstanceFilter filter,
                            std::shared_ptr<client_graphics::WebGLVertexArray> vao,
                            std::shared_ptr<client_graphics::WebGLBuffer> instanceVbo);

  public:
    inline size_t count() const { return list_.size(); }
    inline bool isDirty() const { return isDirty_; }
    /**
     * Update the renderable instances list with the given instances.
     *
     * @param instances The instances to update.
     * @param sortingOrder The sorting order of the instances.
     */
    void update(const InstanceMap &instances, SortingOrder sortingOrder = SortingOrder::kNone);
    size_t copyToArrayData(vector<InstanceData> &dst);
    /**
     * Called before the instanced draw.
     */
    void beforeInstancedDraw(client_graphics::WebGL2Context &glContext);
    /**
     * Called after the instanced draw.
     */
    void afterInstancedDraw(client_graphics::WebGL2Context &glContext);

  private:
    // Clear the instances.
    void clearInstances();
    // Add an instance to the list.
    void addInstance(std::shared_ptr<Instance> instance);
    inline void markAsDirty() { isDirty_ = true; }

  public:
    InstanceFilter filter;
    std::shared_ptr<client_graphics::WebGLVertexArray> vao;
    std::shared_ptr<client_graphics::WebGLBuffer> instanceVbo;

  private:
    std::vector<std::weak_ptr<Instance>> list_;
    bool isDirty_ = true;
  };

  class InstancedMeshBase
  {
    friend class Renderer;
    friend class RenderSystem;

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
    inline RenderableInstancesList &getOpaqueInstancesList() const { return *opaqueInstances_; }
    inline RenderableInstancesList &getTransparentInstancesList() const { return *transparentInstances_; }

  protected:
    /**
     * Setup the instanced mesh with the given instance VBO.
     *
     * @param glContext The WebGL2 context to setup.
     * @param opaqueVao The instance VBO to setup.
     */
    void setup(std::shared_ptr<client_graphics::WebGL2Context> glContext,
               std::shared_ptr<client_graphics::WebGLVertexArray> opaqueVao,
               std::shared_ptr<client_graphics::WebGLBuffer> opaqueInstanceVbo,
               std::shared_ptr<client_graphics::WebGLVertexArray> transparentVao,
               std::shared_ptr<client_graphics::WebGLBuffer> transparentInstanceVao);
    /**
     * Update the internal `idToInstanceMap_` into the opaque and transparent `RenderableInstancesList` the queues.
     *
     * @param ignoreDirty Whether to ignore the dirty flag, `true` means force update.
     */
    void updateRenderQueues(bool ignoreDirty = false);

  private:
    inline void markAsDirty() { isDirty_ = true; }

  protected:
    mutable std::shared_mutex mutex_;
    InstanceMap idToInstanceMap_;
    std::shared_ptr<RenderableInstancesList> opaqueInstances_;
    std::shared_ptr<RenderableInstancesList> transparentInstances_;

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
