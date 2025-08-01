#pragma once

#include <concepts>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <shared_mutex>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <common/math3d/utils.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./ecs.hpp"
#include "./meshes/builder.hpp"
#include "./mesh_base.hpp"
#include "./render_queue.hpp"
#include "./render_layer.hpp"

namespace builtin_scene
{
  // Forward declarations
  class Mesh3d;
  class RenderableInstancesList;

  struct InstanceData
  {
    InstanceData()
        : transform(1.0f)
        , color(1.0f, 1.0f, 1.0f, 0.0f)
        , texUvOffset(0.0f, 0.0f)
        , texUvOffsetR(0.0f, 0.0f)
        , texUvScale(1.0f, 1.0f)
        , texLayerIndex(0)
    {
    }
    glm::mat4 transform;    /** 16 */
    glm::vec4 color;        /** 20 */
    glm::vec2 texUvOffset;  /** 22 - Left or default view texture coordinates */
    glm::vec2 texUvOffsetR; /** 24 - Right eye texture coordinates */
    glm::vec2 texUvScale;   /** 26 - Shared texture scale for both eyes */
    uint32_t texLayerIndex; /** Shared texture layer for both eyes */

    friend std::ostream &operator<<(std::ostream &os, const InstanceData &data)
    {
      os << "InstanceData(" << std::endl
         << "  transform=" << math3d::to_string(data.transform) << std::endl
         << "  color=" << math3d::to_string(data.color) << std::endl
         << "  texUvOffset=" << math3d::to_string(data.texUvOffset) << std::endl
         << "  texUvScale=" << math3d::to_string(data.texUvScale) << std::endl
         << "  texLayerIndex=" << data.texLayerIndex << std::endl
         << "  texUvOffsetR=" << math3d::to_string(data.texUvOffsetR) << std::endl
         << ")";
      return os;
    }

    // If the instance is transparent(alpha = 0.0f).
    inline bool isTransparent() const
    {
      return color.a == 0.0f;
    }

    // If the instance own texture to draw.
    inline bool ownTexture() const
    {
      return (texUvScale.x > 0.0f || texUvScale.y > 0.0f);
    }
  };

  class Instance
  {
    friend class InstancedMeshBase;
    friend class RenderableInstancesList;

  private:
    class TextureCoordBase : public std::array<float, 2>
    {
    public:
      TextureCoordBase()
          : std::array<float, 2>({0.0f, 0.0f})
      {
      }
      TextureCoordBase(const std::array<float, 2> &array)
          : std::array<float, 2>(array)
      {
      }

    public:
      inline float u() const
      {
        return (*this)[0];
      }
      inline float v() const
      {
        return (*this)[1];
      }

      operator glm::vec2() const
      {
        return glm::vec2(u(), v());
      }
      bool operator==(const TextureCoordBase &other) const
      {
        return (u() == other.u() && v() == other.v());
      }
      bool operator==(const glm::vec2 &other) const
      {
        return (u() == other.x && v() == other.y);
      }
    };

  public:
    class TextureScale : public TextureCoordBase
    {
      using TextureCoordBase::TextureCoordBase;

    public:
      void setHalfWidth()
      {
        (*this)[0] *= 0.5f;
      }
    };
    class TextureOffset : public TextureCoordBase
    {
      using TextureCoordBase::TextureCoordBase;

    public:
      void setForRight(const TextureScale &scale)
      {
        (*this)[0] += scale.u() * 0.5f; // Offset right eye UVs by half the width
      }
    };

  public:
    Instance() = default;

  public:
    void randomColor();
    bool setColor(const glm::vec4 &color, bool &hasChanged);
    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void setTransform(const glm::mat4 &transformationMatrix, bool &hasChanged);
    void setTexture(TextureOffset uvOffset,
                    TextureOffset uvOffsetR,
                    TextureScale uvScale,
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
    IMPL_SETTER(RenderQueue, renderQueue_, RenderQueue)
#undef IMPL_BOOL_SETTER
#undef IMPL_SETTER

  private:
    // Add a holder to the instance.
    void addHolder(std::shared_ptr<RenderableInstancesList> holder);
    // Remove a holder from the instance.
    void removeHolder(std::shared_ptr<RenderableInstancesList> holder);
    // Notify the holders that the instance data is updated.
    void notifyHolders();
    // Returns `true` if the instance should be skipped to draw.
    bool skipToDraw() const;

  private:
    InstanceData data_;
    RenderQueue renderQueue_;
    RenderLayer renderLayer_;
    bool enabled_ = false;
    bool isOpaque_ = false;

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
    inline size_t count() const
    {
      return list_.size();
    }
    inline bool isDirty() const
    {
      return isDirty_;
    }
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
    inline void markAsDirty()
    {
      isDirty_ = true;
    }

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
    friend class SceneRenderer;
    friend class RenderSystem;

  public:
    static constexpr size_t STRIDE = sizeof(float) * 26 + sizeof(uint32_t) * 1;
    static inline std::vector<std::string> INSTANCE_ATTRIBUTES = {"instanceTransform",
                                                                  "instanceColor",
                                                                  "instanceTexUvOffset",
                                                                  "instanceTexUvOffsetR",
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
                                     std::function<void(const IVertexAttribute &,
                                                        int,
                                                        size_t,
                                                        size_t)> callback) const;
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

    inline RenderableInstancesList &getOpaqueInstancesList() const
    {
      return *opaqueInstances_;
    }
    inline RenderableInstancesList &getTransparentInstancesList() const
    {
      return *transparentInstances_;
    }

    /**
     * Whether to dispatch a depth-only pass for transparent objects which writes the transparent objects' depth
     * to the depth buffer.
     * 
     * It's mostly used for the collision detection of transparent objects such as GUI elements, we do a depth buffer
     * based collision detection to search for the hit point of the opaque objects and GUI elements, within the flag,
     * we can write the depth buffer of the transparent objects after rendering them, so that the collision detectior
     * can read the correct depth value for the GUI elements.
     */
    inline bool isDepthOnlyPassEnabled() const
    {
      return isDepthOnlyPassEnabled_;
    }
    inline void enableDepthOnlyPass(bool value = true)
    {
      isDepthOnlyPassEnabled_ = value;
    }

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
     * Update the internal `idToInstanceMap_` into the opaque and transparent `RenderableInstancesList`.
     *
     * @param ignoreDirty Whether to ignore the dirty flag, `true` means force update.
     */
    void updateInstancesList(bool ignoreDirty = false);

  private:
    inline void markAsDirty()
    {
      isDirty_ = true;
    }

  protected:
    mutable std::shared_mutex mutex_;
    InstanceMap idToInstanceMap_;
    std::shared_ptr<RenderableInstancesList> opaqueInstances_;
    std::shared_ptr<RenderableInstancesList> transparentInstances_;

  private:
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool isDepthOnlyPassEnabled_ = false;
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
        : InstancedMeshBase()
        , MeshType(std::forward<InitMeshArgs>(args)...)
    {
    }

  public:
    float area() override
    {
      return 0.0f;
    }
    float volume() override
    {
      return 0.0f;
    }
  };
}
