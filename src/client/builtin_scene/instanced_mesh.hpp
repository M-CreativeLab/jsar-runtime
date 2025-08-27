#pragma once

#include <concepts>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <shared_mutex>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <common/math3d/utils.hpp>
#include <client/graphics/webgl_context.hpp>
#include <client/logger.hpp>

#include "./ecs.hpp"
#include "./meshes/builder.hpp"
#include "./mesh_base.hpp"
#include "./render_queue.hpp"
#include "./render_layer.hpp"
#include "./css_border_data_texture.hpp"
#include "./css_scroll_shadow_texture.hpp"

namespace builtin_scene
{
  // Forward declarations
  class Mesh3d;
  class InstanceListBase;
  namespace materials
  {
    class WebContentInstancedMaterial;
  }

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
        , enableSDFTexture(0.0f)
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
    float enableSDFTexture; /** Whether to use SDF texture rendering (0.0=regular, 1.0=SDF) */

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
         << "  enableSDFTexture=" << data.enableSDFTexture << std::endl
         << ")";
      return os;
    }

    // If the instance is transparent(alpha = 0.0f).
    inline bool isTransparent() const
    {
      return color.a == 0.0f;
    }

    // If the instance has a none border style.
    inline bool isBorderNone() const
    {
      return borderStyle == 0;
    }

    // If the instance own texture to draw.
    inline bool ownTexture() const
    {
      return texUvScale.x > 0.0f || texUvScale.y > 0.0f;
    }
  };

  class Instance
  {
    friend class InstancedMeshBase;
    friend class InstanceListBase;
    friend class ContainerInstance;
    friend class ContentInstancesList;

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
    bool setColor(const glm::vec4 &color);
    void setTransform(const glm::mat4 &transformationMatrix);
    void setTexture(TextureOffset uvOffset,
                    TextureOffset uvOffsetR,
                    TextureScale uvScale,
                    uint32_t layerIndex);
    void disableTexture();
    void setDimensions(float width, float height);
    void setBorderRadius(glm::vec4 borderRadius);
    void setBorderRadius(float topLeft, float topRight, float bottomRight, float bottomLeft);
    void setBorderWidth(glm::vec4 borderWidth);
    void setBorderWidth(float top, float right, float bottom, float left);
    void setBorderColor(glm::vec4 borderColor);
    void setBorderColor(float r, float g, float b, float a);
    void setBorderStyle(float borderStyle);
    void setSDFTextureEnabled(bool);
    void setScrollShadowColor(glm::vec4 shadowColor);
    void setScrollShadowColor(float r, float g, float b, float a);
    void setScrollShadowMaxHeight(float maxHeight);
    void setScrollOffset(glm::vec2 offset);
    void setScrollOffset(float x, float y);
    void setContentSize(glm::vec2 size);
    void setContentSize(float width, float height);

#define IMPL_SETTER(NAME, PRIV_FIELD, TYPE) \
  inline bool set##NAME(TYPE value)         \
  {                                         \
    if (PRIV_FIELD != value)                \
    {                                       \
      PRIV_FIELD = value;                   \
      notifyBufferDataChanged();            \
      return true;                          \
    }                                       \
    else                                    \
      return false;                         \
  }
#define IMPL_BOOL_SETTER(NAME, PRIV_FIELD) \
  IMPL_SETTER(NAME, PRIV_FIELD, bool)
#define IMPL_U32_SETTER(NAME, PRIV_FIELD) \
  IMPL_SETTER(NAME, PRIV_FIELD, uint32_t)

    IMPL_BOOL_SETTER(Enabled, enabled_)
    IMPL_BOOL_SETTER(Opaque, isOpaque_)
    IMPL_BOOL_SETTER(IsContainer, isContainer_)
    IMPL_U32_SETTER(BelongsToContainerId, belongsToContainerId_)
    IMPL_SETTER(RenderQueue, renderQueue_, RenderQueue)
    IMPL_SETTER(RenderLayer, renderLayer_, RenderLayer)
#undef IMPL_U32_SETTER
#undef IMPL_BOOL_SETTER
#undef IMPL_SETTER

    // Getter for RenderQueue
    inline const RenderQueue &getRenderQueue() const
    {
      return renderQueue_;
    }

    // Returns if the instance might be invisible based on its properties.
    inline bool maybeInvisible() const
    {
      return maybeInvisible_;
    }

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

    // Getters for scroll shadow fields
    inline const glm::vec4 &getScrollShadowColor() const
    {
      return scrollShadowColor_;
    }

    inline float getScrollShadowMaxHeight() const
    {
      return scrollShadowMaxHeight_;
    }

    inline const glm::vec2 &getScrollOffset() const
    {
      return scrollOffset_;
    }

    inline const glm::vec2 &getContentSize() const
    {
      return contentSize_;
    }

    // Returns if this instance has no borders to draw.
    bool hasNoBorders() const;

  private:
    // Add a holder to the instance.
    void addHolder(std::shared_ptr<InstanceListBase> holder);
    // Remove a holder from the instance.
    void removeHolder(std::shared_ptr<InstanceListBase> holder);
    // Notify the holders that buffer data has changed.
    void notifyBufferDataChanged();
    // Notify the holders that texture data has changed.
    void notifyTextureDataChanged();
    // Notify the holders that border data has changed.
    void notifyBorderDataChanged();
    // Notify the holders that scroll shadow data has changed.
    void notifyScrollShadowDataChanged();
    // Returns `true` if the instance should be skipped to draw.
    bool skipToDraw() const;
    // Set the instance as maybe invisible based on the state of the instance, such as if it has a transparent color,
    // no texture, and no borders to draw, etc.
    //
    // This method should be called when related properties are changed, such as color, texture, border, etc.
    void setMaybeInvisible();

  private:
    InstanceData data_;
    glm::vec4 borderWidths_;
    glm::vec4 borderColors_[4];

    // Scroll shadow data (managed separately from InstanceData for texture-based rendering)
    glm::vec4 scrollShadowColor_{0.0f, 0.0f, 0.0f, 0.0f};
    float scrollShadowMaxHeight_{0.2f};
    glm::vec2 scrollOffset_{0.0f, 0.0f};
    glm::vec2 contentSize_{0.0f, 0.0f};

    RenderQueue renderQueue_;
    RenderLayer renderLayer_;
    bool enabled_ = false;
    bool maybeInvisible_ = true;
    bool isOpaque_ = false;

    bool isContainer_ = false;
    uint32_t belongsToContainerId_ = 0;

  private:
    std::vector<std::weak_ptr<InstanceListBase>> holders_;
  };

  enum class InstanceFilter
  {
    kAll,
    kOpaque,
    kTransparent
  };

  using InstanceMap = std::unordered_map<ecs::EntityId, std::shared_ptr<Instance>>;

  class InstanceListBase : public std::enable_shared_from_this<InstanceListBase>
  {
    friend class Instance;
    friend class InstancedMeshBase;

  public:
    InstanceListBase(std::shared_ptr<client_graphics::WebGLVertexArray> vao,
                     std::shared_ptr<client_graphics::WebGLBuffer> vbo);
    virtual ~InstanceListBase() = default;

  public:
    virtual bool isContainerInstance() const
    {
      return false;
    }
    virtual bool isContentInstancesList() const
    {
      return false;
    }

    inline size_t count() const
    {
      return list_.size();
    }
    inline bool isBufferDataDirty() const
    {
      return bufferDataDirty_;
    }

    /**
     * Configure the instance attributes for the given WebGL program.
     */
    size_t configureAttribs(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                            std::shared_ptr<client_graphics::WebGLProgram> program,
                            std::shared_ptr<Mesh3d> mesh3d);


    size_t copyToArrayData(vector<InstanceData> &dst);

    void beforeInstancedDraw(client_graphics::WebGL2Context &glContext);
    void afterInstancedDraw(client_graphics::WebGL2Context &glContext);

    /**
     * Get the current instances as a vector (for border data updates).
     */
    std::vector<std::shared_ptr<Instance>> getInstances() const;

  protected:
    // Clear the instances.
    virtual void clearInstances();
    // Add an instance to the list.
    void addInstance(std::shared_ptr<Instance> instance);

    inline void markBufferAsDirty()
    {
      bufferDataDirty_ = true;
    }

  public:
    std::shared_ptr<client_graphics::WebGLVertexArray> vao;
    std::shared_ptr<client_graphics::WebGLBuffer> instanceVbo;

  protected:
    std::vector<std::weak_ptr<Instance>> list_;

  private:
    bool bufferDataDirty_ = true;
  };

  // Derived class for container instances (with container ID)
  class ContainerInstance : public InstanceListBase
  {
  public:
    ContainerInstance(uint32_t containerIndex,
                      std::shared_ptr<client_graphics::WebGLVertexArray> vao,
                      std::shared_ptr<client_graphics::WebGLBuffer> vbo)
        : InstanceListBase(vao, vbo)
        , containerIndex_(containerIndex)
        , belongsToContainerId_(std::nullopt)
    {
    }

    bool isContainerInstance() const override
    {
      return true;
    }

    /**
     * Set a single instance for this container.
     */
    void setInstance(std::shared_ptr<Instance> instance);

    uint32_t getContainerIndex() const
    {
      return containerIndex_;
    }
    std::optional<uint32_t> getBelongsToContainerId() const
    {
      return belongsToContainerId_;
    }
    void setBelongsToContainerId(uint32_t id)
    {
      belongsToContainerId_ = id;
    }

  private:
    uint32_t containerIndex_;
    std::optional<uint32_t> belongsToContainerId_;
  };

  // Derived class for content instances list
  class ContentInstancesList : public InstanceListBase
  {
    friend struct LayeredInstancesData;

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
    ContentInstancesList(InstanceFilter filter,
                         std::shared_ptr<client_graphics::WebGLVertexArray> vao,
                         std::shared_ptr<client_graphics::WebGLBuffer> vbo)
        : InstanceListBase(vao, vbo)
        , borderDataDirty_(true)
        , scrollShadowDataDirty_(true)
    {
    }

    friend std::ostream &operator<<(std::ostream &os, const ContentInstancesList &list)
    {
      os << "ContentInstancesList(count=" << list.count() << ")"
         << std::endl;
      return os;
    }

    bool isContentInstancesList() const override
    {
      return true;
    }
    inline bool isBorderDataDirty() const
    {
      return borderDataDirty_;
    }
    inline bool isScrollShadowDataDirty() const
    {
      return scrollShadowDataDirty_;
    }
    inline void markTextureDataAsDirty()
    {
      borderDataDirty_ = true;
      scrollShadowDataDirty_ = true;
    }
    inline void markBorderDataAsDirty()
    {
      borderDataDirty_ = true;
    }
    inline void markScrollShadowDataAsDirty()
    {
      scrollShadowDataDirty_ = true;
    }

    /**
     * Update the renderable instances list with the given instances.
     *
     * @param instances The instances to update.
     * @param sortingOrder The sorting order of the instances.
     */
    void update(const InstanceMap &instances, SortingOrder sortingOrder = SortingOrder::kNone);
    void beforeInstancedDraw(client_graphics::WebGL2Context &glContext,
                             CSSBorderDataTexture *borderDataTexture,
                             CSSScrollShadowTexture *scrollShadowDataTexture);

    void clearInstances() override;
    void sortInstances(SortingOrder sortingOrder);

  public:
    InstanceFilter filter;

  private:
    bool borderDataDirty_;
    bool scrollShadowDataDirty_;
  };

  /**
   * LayeredInstancesData represents a single container and its content for isolated rendering
   * 
   * This structure supports per-container stencil rendering to prevent content leakage:
   * - Each container gets its own LayeredInstancesData entry
   * - Contains the layer, container ID, container instance and content instances
   * - Enables isolated mask/content rendering per container
   */
  struct LayeredInstancesData
  {
    LayeredInstancesData() = default;
    LayeredInstancesData(RenderLayer layer)
        : layer(layer)
    {
    }

    RenderLayer layer;
    std::shared_ptr<ContainerInstance> containerInstance;   // Single container mask
    std::shared_ptr<ContentInstancesList> contentInstances; // Content for this container

    void sortContentInstances()
    {
      if (contentInstances)
        contentInstances->sortInstances(ContentInstancesList::SortingOrder::kFrontToBack);
    }
  };

  class InstancedMeshBase
  {
    friend class RenderSystem;
    friend class materials::WebContentInstancedMaterial;

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
                                                                  "instanceBorderStyle",
                                                                  "instanceUseSDFTexture",
                                                                  "instanceScrollShadowColor",
                                                                  "instanceScrollShadowMaxHeight",
                                                                  "instanceScrollOffset",
                                                                  "instanceContentSize"};

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
     * 
     * @param callback The callback to call for each instance. The callback should return `true` if the instance needs 
     *                 to update the structure, otherwise `false`.
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

    inline ContentInstancesList &getDepthOnlyInstancesList() const
    {
      return *depthOnlyInstances_;
    }
    inline size_t countLayers() const
    {
      return layeredInstances_.size();
    }

    using LayerCallback = std::function<void(RenderLayer layer,
                                             ContainerInstance *containerInstance,
                                             ContentInstancesList *contentInstances)>;
    inline void iterateLayers(LayerCallback callback) const
    {
      shared_lock<shared_mutex> lock(mutex_);
      for (const auto *layerData : layeredInstances_)
      {
        ContainerInstance *containerInstance =
          (layerData->containerInstance && layerData->containerInstance->count() > 0)
            ? layerData->containerInstance.get()
            : nullptr;

        if (containerInstance)
        {
          callback(layerData->layer, containerInstance, layerData->contentInstances.get());
        }
      }
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
               std::shared_ptr<Mesh3d> mesh3d = nullptr);
    void configureInstanceAttribs(std::shared_ptr<client_graphics::WebGLProgram> program,
                                  std::shared_ptr<Mesh3d> mesh3d);
    /**
     * Update the internal `idToInstanceMap_` into the `layeredInstances_` and `depthOnlyInstances_`.
     *
     * @param ignoreDirty Whether to ignore the dirty flag, `true` means force update.
     */
    void updateInstancesList(std::shared_ptr<client_graphics::WebGLProgram> program, bool ignoreDirty = false);

  private:
    inline void markStructureAsDirty()
    {
      isStructureDirty_ = true;
    }

  protected:
    mutable std::shared_mutex mutex_;
    InstanceMap idToInstanceMap_;
    std::vector<LayeredInstancesData *> layeredInstances_; // Store pointers to active LayeredInstancesData objects
    std::shared_ptr<ContentInstancesList> depthOnlyInstances_;

  private:
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    std::weak_ptr<Mesh3d> mesh3d_;
    bool isDepthOnlyPassEnabled_ = false;
    bool isStructureDirty_ = true;

    // Caching/pooling system for LayeredInstancesData objects
    std::optional<LayeredInstancesData> defaultLayerData_;                // Default layer for non-container instances
    std::unordered_map<std::string, LayeredInstancesData> layerDataPool_; // Pool for layer+container combinations
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

  public:
    void onMesh3dInitialized(std::shared_ptr<Mesh3d>, std::shared_ptr<client_graphics::WebGL2Context>) override;
    void onConfigureInstanceAttribs(std::shared_ptr<Mesh3d>, std::shared_ptr<client_graphics::WebGLProgram>) override;
  };
}
