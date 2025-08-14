#include <array>
#include <chrono>
#include <client/dom/node.hpp>
#include <client/dom/element.hpp>
#include <client/cssom/units.hpp>

#include "./client_renderer.hpp"
#include "./hierarchy.hpp"
#include "./web_content.hpp"
#include "./materials.hpp"
#include "./camera.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace ecs;

  void RenderStartupSystem::onExecute()
  {
    auto meshes = getResource<Meshes>();
    auto materials = getResource<Materials>();
    auto renderer = getResource<SceneRenderer>();

    // Create a mesh for the volume mask
    auto entity = spawn(
      Mesh3d(meshes->add(MeshBuilder::CreateBox(1.0f, 1.0f, 0.05f))),
      MeshMaterial3d(materials->add(materials::ColorMaterial::Red())),
      Transform::FromXYZ(0.0f, 0.0f, 0.0f)
        .FromScale(client_cssom::pixelToMeter(renderer->volumeSize())));
    renderer->setVolumeMask(entity);
  }

  void RenderSystem::onExecute()
  {
    if (renderer_ == nullptr) [[unlikely]]
    {
      renderer_ = getResource<SceneRenderer>();
      assert(renderer_ != nullptr); // The renderer must be valid.
    }

    if (xrExperience_ == nullptr) [[unlikely]]
    {
      xrExperience_ = getResource<WebXRExperience>();
    }
    if (xrExperience_ != nullptr)
    {
      auto xrViewerPose = xrExperience_->viewerPose();
      if (xrViewerPose != nullptr)
      {
        auto &views = xrViewerPose->views();
        if (!xrExperience_->multiviewEnabled())
        {
          for (auto view : views)
            render(XRRenderTarget(view));
        }
        else
        {
          render(XRRenderTarget(views));
        }
        return;
      }
    }

    // Fallback to the default rendering
    render(nullopt);
  }

  void RenderSystem::render(optional<XRRenderTarget> renderTarget)
  {
    auto roots = queryEntities<hierarchy::Root>([](const hierarchy::Root &root) -> bool
                                                { return root.renderable == true; });
    if (roots.size() > 0)
    {
      if (renderTarget != nullopt)
      {
        if (!renderTarget->isMultiview())
          renderer_->setViewport(renderTarget->view()->viewport());
      }

      // Update data for meshes(including instanced), materials, particles, etc.
      for (const auto &root : roots)
        traverseAndUpdate(root, renderTarget);

      // Render the objects (opaques, transparents).
      renderPass(roots, RenderPass::kOpaques, renderTarget);
      renderPass(roots, RenderPass::kTransparents, renderTarget);

      // TODO(yorkie): Render the particles and other post-processing effects.
    }
  }

  struct RenderableMeshEntity
  {
    EntityId id;
    shared_ptr<Mesh3d> meshComponent;
    shared_ptr<MeshMaterial3d> materialComponent;
  };

  void RenderSystem::renderPass(vector<EntityId> &roots, RenderPass renderPass, optional<XRRenderTarget> renderTarget)
  {
    assert(roots.size() > 0 && "The roots must not be empty.");

    vector<RenderableMeshEntity> entities; // TODO(yorkie): cache this?
    auto addRenderableItem = [this, &renderPass, &entities](EntityId entity) -> bool
    {
      // Render the mesh if it exists
      auto mesh = getComponent<Mesh3d>(entity);
      if (mesh != nullptr)
      {
        // If the mesh exists but rendering is disabled, we need to skip its rendering and its children.
        if (mesh->isRenderingDisabled())
          return false;

        auto material = getComponent<MeshMaterial3d>(entity);
        if (material && material->matchesPass(renderPass))
          entities.push_back({entity, mesh, material});
      }

      // TODO: support other renderable components (e.g., particles, etc.)
      return true;
    };

    // Search for renderable items in the hierarchy
    for (const auto &root : roots)
      traverse(root, addRenderableItem);

    // Render items
    if (entities.size() > 0)
    {
      renderVolumeMask(renderPass, renderTarget);
      onBeforeRender(renderPass, renderTarget);
      for (const auto &entity : entities)
      {
        renderMesh(entity.id,
                   entity.meshComponent,
                   entity.materialComponent,
                   renderPass,
                   renderTarget);
      }
      onAfterRender(renderPass, renderTarget);
    }
  }

  void RenderSystem::renderVolumeMask(RenderPass renderPass, optional<XRRenderTarget> renderTarget)
  {
    if (renderer_->isVolumeMaskEnabled())
    {
      auto renderVolume = [this, &renderPass, &renderTarget](ecs::EntityId entity, SceneRenderer &renderer)
      {
        auto meshComponent = getComponent<Mesh3d>(entity);
        auto materialComponent = getComponent<MeshMaterial3d>(entity);
        renderMesh(entity, meshComponent, materialComponent, renderPass, renderTarget);
      };
      renderer_->addVolumeMask(renderVolume);
    }
  }

  void RenderSystem::renderMesh(const EntityId &entity,
                                shared_ptr<Mesh3d> meshComponent,
                                shared_ptr<MeshMaterial3d> materialComponent,
                                RenderPass renderPass,
                                optional<XRRenderTarget> renderTarget)
  {
    if (!meshComponent->initialized())
      renderer_->initializeMesh3d(meshComponent);
    if (!materialComponent->initialized())
      renderer_->initializeMeshMaterial3d(meshComponent, materialComponent);

    // Update the mesh3d and material if needed
    renderer_->tryUpdateMeshMaterial3d(meshComponent, materialComponent);

    // Draw
    shared_ptr<Transform> parentTransform = nullptr;
    auto parentComponent = getComponent<hierarchy::Parent>(entity);
    if (parentComponent != nullptr)
      parentTransform = getComponent<Transform>(parentComponent->parent());
    renderer_->drawMesh3d(entity,
                          meshComponent,
                          materialComponent,
                          getComponent<Transform>(entity),
                          parentTransform,
                          renderPass,
                          renderTarget);
  }

  void RenderSystem::onBeforeRender(const RenderPass renderPass, optional<XRRenderTarget> renderTarget)
  {
    renderer_->onBeforeRender(renderPass, renderTarget);
  }

  void RenderSystem::onAfterRender(const RenderPass renderPass, optional<XRRenderTarget> renderTarget)
  {
    renderer_->onAfterRender(renderPass, renderTarget);
  }

  void RenderSystem::traverse(ecs::EntityId root, std::function<bool(ecs::EntityId)> &&exec)
  {
    // Pre-order traversal
    if (!exec(root))
      return;

    auto children = getComponent<hierarchy::Children>(root);
    if (children != nullptr)
    {
      for (const auto &child : children->children())
        traverse(child, std::move(exec));
    }
  }

  size_t RenderSystem::traverseAndUpdate(ecs::EntityId root, std::optional<XRRenderTarget> renderTarget)
  {
    size_t num = 0;
    auto updateEntity = [this, &renderTarget, &num](EntityId entity) -> bool
    {
      auto mesh = getComponent<Mesh3d>(entity);
      if (mesh != nullptr)
      {
        // Skip updating data if the mesh rendering is disabled.
        if (mesh->isRenderingDisabled())
          return false;

        // Update the data for renderable mesh components.
        // TODO(yorkie): update transformation matrix here?
        if (mesh->is<InstancedMeshBase>())
          updateInstancedMeshData(*mesh, renderTarget);

        num++;
      }
      return true; // Continue traversing children
    };

    traverse(root, updateEntity);
    return num;
  }

  glm::mat4 RenderSystem::getTransformationMatrix(ecs::EntityId id)
  {
    Transform &transform = const_cast<Transform &>(getComponentChecked<Transform>(id));
    shared_ptr<Transform> parentTransform = nullptr;
    shared_ptr<hierarchy::Parent> parentComponent = getComponent<hierarchy::Parent>(id);
    if (parentComponent != nullptr)
      parentTransform = getComponent<Transform>(parentComponent->parent());

    // The world-space transformation matrix for this entity.
    glm::mat4 baseMatrixInWorldSpace = transform.matrix();

    // Compute the final post transform.
    //
    // The post transform is updated by the CSS `transform` project, it's stored as reference-space. The following block
    // is used to compute the final post transform in the node chain from the current node.
    glm::mat4 postMat = glm::mat4(1.0f);
    {
      if (parentTransform != nullptr)
        postMat = parentTransform->getOrInitPostTransform().accumulatedMatrix();
      auto &postTransform = transform.getOrInitPostTransform();
      postMat = postTransform.matrix() * postMat;
      postTransform.setAccumulatedMatrix(postMat);
    }

    // Returns the transformation matrix is the world-space base matrix with the post transformation.
    return postMat * baseMatrixInWorldSpace;
  }

  void RenderSystem::updateInstancedMeshData(const Mesh3d &meshComponent, optional<XRRenderTarget> renderTarget)
  {
    InstancedMeshBase &instancedMesh = meshComponent.getHandleCheckedAsRef<InstancedMeshBase>();

    /**
     * This function should return a `boolean` value, that indicates whether the `InstancedMesh` needs to refresh its
     * renderable lists (opaque and transparent) after this iteration. Refreshing the renderable lists is used to update
     * the instances for opaque and transparent rendering passes, so we only need to achieve it when the lists might be
     * changed, the following conditions might change the lists:
     * 
     * 1. When there is a new instance added or an existing instance removed, this is handled by the `InstancedMesh` 
     *    itself.
     * 2. When an instance's `enabled` state changes, it might affect whether the instance should be rendered.
     * 3. When an instance's `isOpaque` state changes, it might affect which render pass (opaque or transparent) the
     *    instance belongs to.
     * 4. When an instance's `renderQueue` changes, it might affect the order of rendering within its render pass.
     * 5. When an instance's `maybeInvisible` state changes, it might affect whether the instance should be rendered.
     * 
     * Other property changes, such as transformation matrix, texture, color, border, etc., do not affect the
     * renderable lists, so they do not require refreshing the lists.
     * 
     * The `maybeInvisible` state is dynamically determined based on the instance's properties, such as if it has a
     * transparent color, no texture, and no borders to draw. If an instance's `maybeInvisible` state changes,
     * it might affect whether the instance should be rendered, so we need to refresh the renderable lists in that case.
     */
    auto updateInstanceData = [this, &renderTarget](ecs::EntityId id, Instance &instance) -> bool
    {
      bool needsUpdate = false;
      auto transformComponent = getComponent<Transform>(id);
      auto webContentComponent = getComponent<WebContent>(id);

      // Record the maybe invisible state before updating
      bool maybeInvisibleBeforeUpdating = instance.maybeInvisible();

      // Update for instance transformation
      // TODO(yorkie): consider improving the performance of getting transformation matrix
      if (TR_LIKELY(transformComponent != nullptr))
      {
        auto currentMatrix = getTransformationMatrix(id);
        instance.setTransform(currentMatrix);
        transformComponent->setComputedMatrix(currentMatrix);
      }

      // Update for web content component
      if (TR_LIKELY(webContentComponent != nullptr))
      {
        if (instance.setEnabled(true))
          needsUpdate = true;
        if (instance.setOpaque(webContentComponent->isOpaque()))
          needsUpdate = true;

        // Update instance render queue
        auto elementComponent = getComponent<hierarchy::Element>(id);
        if (elementComponent != nullptr &&
            elementComponent->node != nullptr &&
            elementComponent->node->isElementOrText())
        {
          const auto &elementRenderQueue = elementComponent->node->getRenderQueue();
          if (instance.setRenderQueue(elementComponent->node->getRenderQueue()))
            needsUpdate = true;
        }

        // Update instance border data from computed style
        const auto &style = webContentComponent->style();
        const auto &fragment = webContentComponent->fragment();
        const auto &borderWidth = fragment->border();

        // Update instance basic shape data
        instance.setBorderRadius(webContentComponent->borderRadius());
        instance.setDimensions(fragment->contentWidth() + borderWidth.left() + borderWidth.right(),
                               fragment->contentHeight() + borderWidth.top() + borderWidth.bottom());

        // Extract border width (top, right, bottom, left)
        instance.setBorderWidth(borderWidth.top(),
                                borderWidth.right(),
                                borderWidth.bottom(),
                                borderWidth.left());

        // Extract border color (use top border color for now, could be extended for per-side colors)
        const auto &borderColor = style.borderColor();
        const auto &topBorderColor = borderColor.top();
        if (topBorderColor.isAbsolute())
        {
          SkColor skColor = topBorderColor.getAbsoluteColor();
          instance.setBorderColor(SkColorGetR(skColor) / 255.0f,
                                  SkColorGetG(skColor) / 255.0f,
                                  SkColorGetB(skColor) / 255.0f,
                                  SkColorGetA(skColor) / 255.0f);
        }
        else
        {
          // Default to transparent if color is not absolute
          instance.setBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
        }

        // Extract border style (use top border style, convert to float)
        // TODO(yorkie): support per-side border styles
        const auto &borderStyle = style.borderStyle().top();
        uint32_t borderStyleValue = 0; // none
        if (borderStyle.isSolid())
          borderStyleValue = 1; // solid
        else if (borderStyle.isDashed())
          borderStyleValue = 2; // dashed
        instance.setBorderStyle(borderStyleValue);

        // Update instance texture data
        auto textureRect = webContentComponent->textureRect();
        if (textureRect != nullptr)
        {
          int texturePad = webContentComponent->texturePad();
          instance.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

          // Calculate left and right eye texture coordinates for spatial images
          Instance::TextureOffset uvOffset = textureRect->getUvOffset(texturePad);
          Instance::TextureOffset uvOffsetR = uvOffset;
          Instance::TextureScale uvScale = textureRect->getUvScale(texturePad);

          // For spatial images, the texture atlas system handles left/right eye regions
          // via instance data coordinates set in setSpatialTexture method
          if (webContentComponent->isSpatialized())
          {
            assert(renderTarget != nullopt &&
                   "The render target must be valid for spatialized images.");

            if (renderTarget->isMultiview())
            {
              uvOffsetR.setForRight(uvScale);
              uvScale.setHalfWidth();
            }
            else
            {
              auto view = renderTarget->view();
              if (view != nullptr && view->eye() == client_xr::XREye::kRight)
                uvOffset.setForRight(uvScale);
              uvScale.setHalfWidth();
            }
          }
          instance.setTexture(uvOffset,
                              uvOffsetR,
                              uvScale,
                              textureRect->layer);
        }
        else
        {
          instance.setColor(webContentComponent->backgroundColor());
          instance.disableTexture();
        }

        // Update SDF texture state from WebContent
        instance.setUseSDFTexture(webContentComponent->isSDFTexture());
      }

      // When the visible state changes, we need to update the renderable list.
      if (maybeInvisibleBeforeUpdating != instance.maybeInvisible())
        needsUpdate = true;

      // Return if the instance data needs to trigger the renderable list update.
      return needsUpdate;
    };
    instancedMesh.iterateInstances(updateInstanceData);
  }
}
