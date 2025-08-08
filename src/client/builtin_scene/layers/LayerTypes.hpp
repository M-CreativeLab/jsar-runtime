#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <glm/glm.hpp>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkRRect.h>
#include <client/layout/fragment.hpp>
#include "../ecs.hpp"

namespace builtin_scene::layers
{
  /**
   * Represents a layer node in the hierarchical layer tree.
   * Each layer corresponds to a WebContent element and maintains
   * its rendering order, clipping information, and scroll state.
   */
  struct LayerNode
  {
    ecs::EntityId entityId;
    int layerIndex;
    int zIndex;
    float translateZ;

    // Clipping and positioning
    std::optional<SkRect> clipRect;
    std::optional<SkRRect> roundedClipRect;
    glm::vec3 scrollOffset;
    glm::mat4 transform;

    // Hierarchy
    std::weak_ptr<LayerNode> parent;
    std::vector<std::shared_ptr<LayerNode>> children;

    // State flags
    bool isScrollable;
    bool hasOverflow;
    bool needsClipping;

    LayerNode(ecs::EntityId id, int layer = 0)
        : entityId(id)
        , layerIndex(layer)
        , zIndex(0)
        , translateZ(0.0f)
        , scrollOffset(0.0f)
        , transform(1.0f)
        , isScrollable(false)
        , hasOverflow(false)
        , needsClipping(false)
    {
    }

    /**
     * Calculate the absolute transform from the root to this layer,
     * accumulating parent transforms and scroll offsets.
     */
    glm::mat4 getAbsoluteTransform() const;

    /**
     * Calculate the absolute clip rectangle in world space,
     * intersecting with parent clip regions.
     */
    std::optional<SkRect> getAbsoluteClipRect() const;

    /**
     * Check if this layer is clipped by any ancestor.
     */
    bool isClippedByAncestor() const;

    /**
     * Get the effective scroll offset accumulated from all ancestors.
     */
    glm::vec3 getAccumulatedScrollOffset() const;
  };

  /**
   * Represents a single render command in the ordered render list.
   * Contains all necessary information to render a WebContent layer
   * with the correct transforms, clipping, and state.
   */
  struct RenderCommand
  {
    enum Type
    {
      kPushClip,
      kPopClip,
      kPushTransform,
      kPopTransform,
      kRenderWebContent
    };

    Type type;
    ecs::EntityId entityId;
    std::shared_ptr<LayerNode> layerNode;

    // Clipping information
    std::optional<SkRect> clipRect;
    std::optional<SkRRect> roundedClipRect;
    bool useScissor;
    bool useStencil;

    // Transform information
    glm::mat4 transform;
    glm::vec3 scrollOffset;

    RenderCommand(Type t, ecs::EntityId id = ecs::EntityId::Invalid())
        : type(t)
        , entityId(id)
        , useScissor(false)
        , useStencil(false)
        , transform(1.0f)
        , scrollOffset(0.0f)
    {
    }
  };

  /**
   * Configuration for layer rendering behavior.
   */
  struct LayerRenderConfig
  {
    bool enableDebugOverlay;
    bool preferScissorClipping;
    bool enableStencilClipping;
    bool enableBatching;
    float devicePixelRatio;

    LayerRenderConfig()
        : enableDebugOverlay(false)
        , preferScissorClipping(true)
        , enableStencilClipping(true)
        , enableBatching(true)
        , devicePixelRatio(1.0f)
    {
    }
  };
}