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
        , dimensions(0.0f, 0.0f)
        , borderRadius(0.0f, 0.0f, 0.0f, 0.0f)
        , borderStyle(0.0f)
    {
    }
    glm::mat4 transform;    /** element transformation */
    glm::vec4 color;        /** background color */
    glm::vec2 texUvOffset;  /** Left or default view texture coordinates */
    glm::vec2 texUvOffsetR; /** Right eye texture coordinates */
    glm::vec2 texUvScale;   /** Shared texture scale for both eyes */
    uint32_t texLayerIndex; /** Shared texture layer for both eyes */
    glm::vec2 dimensions;   /** The dimensions */
    glm::vec4 borderRadius; /** Border radius for each corner (top-left, top-right, bottom-right, bottom-left) */
    uint32_t borderStyle;   /** Border style (0=none, 1=solid, 2=dashed) */

    friend std::ostream &operator<<(std::ostream &os, const InstanceData &data)
    {
      os << "InstanceData(" << std::endl
         << "  transform=" << math3d::to_string(data.transform) << std::endl
         << "  color=" << math3d::to_string(data.color) << std::endl
         << "  texUvOffset=" << math3d::to_string(data.texUvOffset) << std::endl
         << "  texUvScale=" << math3d::to_string(data.texUvScale) << std::endl
         << "  texLayerIndex=" << data.texLayerIndex << std::endl
         << "  texUvOffsetR=" << math3d::to_string(data.texUvOffsetR) << std::endl
         << "  dimensions=" << math3d::to_string(data.dimensions) << std::endl
         << "  borderRadius=" << math3d::to_string(data.borderRadius) << std::endl
         << "  borderStyle=" << data.borderStyle << std::endl
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
    void setDimensions(float width, float height, bool &hasChanged);
    void setBorderRadius(glm::vec4 borderRadius, bool &hasChanged);
    void setBorderRadius(float topLeft,
                         float topRight,
                         float bottomRight,
                         float bottomLeft,
                         bool &hasChanged);
    void setBorderWidth(glm::vec4 borderWidth, bool &hasChanged);
    void setBorderWidth(float top,
                        float right,
                        float bottom,
                        float left,
                        bool &hasChanged);
    void setBorderColor(glm::vec4 borderColor, bool &hasChanged);
    void setBorderColor(float r, float g, float b, float a, bool &hasChanged);
    void setBorderStyle(float borderStyle, bool &hasChanged);

#define IMPL_SETTER(NAME, PRIV_FIELD, TYPE) \
  inline bool set##NAME(TYPE value)         \
  {                                         \
    if (PRIV_FIELD != value)                \
    {                                       \
      PRIV_FIELD = value;                   \
      notifyStructureChanged();             \
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

    // Getter for instance data
    inline const InstanceData &data() const
    {
      return data_;
    }

    // Getters for border fields
    inline const glm::vec4 &getBorderWidths() const
    {
      return borderWidths_;
    }

    inline const glm::vec4 &getBorderColor(int side) const
    {
      return borderColors_[side % 4];
    }

    inline const glm::vec4 *getBorderColors() const
    {
      return borderColors_;
    }

  private:
    // Add a holder to the instance.
    void addHolder(std::shared_ptr<RenderableInstancesList> holder);
    // Remove a holder from the instance.
    void removeHolder(std::shared_ptr<RenderableInstancesList> holder);
    // Notify the holders that the instance data is updated.
    void notifyHolders();
    // Notify the holders that border data has changed.
    void notifyBorderDataChanged();
    // Notify the holders that structure has changed (enable/disable, opaque/transparent).
    void notifyStructureChanged();
    // Returns `true` if the instance should be skipped to draw.
    bool skipToDraw() const;

  private:
    InstanceData data_;
    glm::vec4 borderWidths_;
    glm::vec4 borderColors_[4];
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
    inline bool isStructureDirty() const
    {
      return structureDirty_;
    }
    inline bool isBorderDataDirty() const
    {
      return borderDataDirty_;
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
    void beforeInstancedDraw(client_graphics::WebGL2Context &glContext,
                             class CSSBorderDataTexture *borderDataTexture = nullptr);
    /**
     * Called after the instanced draw.
     */
    void afterInstancedDraw(client_graphics::WebGL2Context &glContext);

    /**
     * Get the current instances as a vector (for border data updates).
     */
    std::vector<std::shared_ptr<Instance>> getInstances() const;

  private:
    // Clear the instances.
    void clearInstances();
    // Add an instance to the list.
    void addInstance(std::shared_ptr<Instance> instance);
    inline void markStructureAsDirty()
    {
      structureDirty_ = true;
    }
    inline void markBorderDataAsDirty()
    {
      borderDataDirty_ = true;
    }

  public:
    InstanceFilter filter;
    std::shared_ptr<client_graphics::WebGLVertexArray> vao;
    std::shared_ptr<client_graphics::WebGLBuffer> instanceVbo;

  private:
    std::vector<std::weak_ptr<Instance>> list_;
    bool structureDirty_ = true;
    bool borderDataDirty_ = true;
  };

  class InstancedMeshBase
  {
    friend class SceneRenderer;
    friend class RenderSystem;

  public:
    static constexpr size_t STRIDE = sizeof(InstanceData);
    static inline std::vector<std::string> INSTANCE_ATTRIBUTES = {"instanceTransform",
                                                                  "instanceColor",
                                                                  "instanceTexUvOffset",
                                                                  "instanceTexUvOffsetR",
                                                                  "instanceTexUvScale",
                                                                  "instanceLayerIndex",
                                                                  "instanceDimensions",
                                                                  "instanceBorderRadius",
                                                                  "instanceBorderStyle"};

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
    inline void markStructureAsDirty()
    {
      structureDirty_ = true;
    }

  protected:
    mutable std::shared_mutex mutex_;
    InstanceMap idToInstanceMap_;
    std::shared_ptr<RenderableInstancesList> opaqueInstances_;
    std::shared_ptr<RenderableInstancesList> transparentInstances_;

  private:
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool isDepthOnlyPassEnabled_ = false;
    bool structureDirty_ = true;
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
