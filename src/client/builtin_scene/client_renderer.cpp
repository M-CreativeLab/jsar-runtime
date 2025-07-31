#include <array>
#include <chrono>
#include <client/dom/node.hpp>
#include <client/dom/element.hpp>
#include <client/cssom/units.hpp>

#include "./client_renderer.hpp"
#include "./hierarchy.hpp"
#include "./web_content.hpp"
#include "./materials.hpp"

namespace builtin_scene
{
  using namespace std;

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
    auto renderer = getResource<SceneRenderer>();
    assert(renderer != nullptr); // The renderer must be valid.

    auto xrExperience = getResource<WebXRExperience>();
    if (xrExperience != nullptr) // XR rendering
    {
      auto xrViewerPose = xrExperience->viewerPose();
      if (xrViewerPose != nullptr)
      {
        auto &views = xrViewerPose->views();
        if (!xrExperience->multiviewEnabled())
        {
          for (auto view : views)
            render(*renderer, XRRenderTarget(view));
        }
        else
        {
          render(*renderer, XRRenderTarget(views));
        }
        return;
      }
    }

    // Fallback to the default rendering
    render(*renderer, nullopt);
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

  void RenderSystem::tryUpdateInstanceDataForInstancedMesh(const Mesh3d &meshComponent,
                                                           optional<XRRenderTarget> renderTarget)
  {
    if (!meshComponent.isInstancedMesh())
      return;

    InstancedMeshBase &instancedMesh = meshComponent.getHandleCheckedAsRef<InstancedMeshBase>();
    auto updateInstanceData = [this, &renderTarget](ecs::EntityId id, Instance &instance) -> bool
    {
      bool hasChanged = false;
      auto transformComponent = getComponent<Transform>(id);
      auto webContentComponent = getComponent<WebContent>(id);

      if (TR_LIKELY(transformComponent != nullptr))
      {
        auto currentMatrix = getTransformationMatrix(id);
        instance.setTransform(currentMatrix, hasChanged);
        transformComponent->setComputedMatrix(currentMatrix);
      }
      if (TR_LIKELY(webContentComponent != nullptr))
      {
        if (instance.setEnabled(true))
          hasChanged = true;
        if (instance.setOpaque(webContentComponent->isOpaque()))
          hasChanged = true;

        auto elementComponent = getComponent<hierarchy::Element>(id);
        if (elementComponent != nullptr &&
            elementComponent->node != nullptr &&
            elementComponent->node->isElementOrText())
        {
          if (instance.setRenderQueue(elementComponent->node->getRenderQueue()))
            hasChanged = true;
        }

        auto textureRect = webContentComponent->textureRect();
        int texturePad = webContentComponent->texturePad();

        if (textureRect != nullptr)
        {
          instance.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), hasChanged);

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
                              textureRect->layer,
                              hasChanged);
        }
        else
        {
          instance.setColor(webContentComponent->backgroundColor(), hasChanged);
          instance.disableTexture(hasChanged);
        }
      }
      return hasChanged;
    };
    instancedMesh.iterateInstances(updateInstanceData);
  }

  void RenderSystem::render(SceneRenderer &renderer, optional<XRRenderTarget> renderTarget)
  {
    auto roots = queryEntities<hierarchy::Root>([](const hierarchy::Root &root) -> bool
                                                { return root.renderable == true; });
    if (roots.size() <= 0) // No root entities to render
      return;

    if (renderTarget != nullopt)
    {
      if (!renderTarget->isMultiview())
        renderer.setViewport(renderTarget->view()->viewport());
    }

    renderVolumeMask(renderer, renderTarget);
    onBeforeRender(renderer, renderTarget);
    for (const auto &root : roots)
      traverseAndRender(root, renderer, renderTarget);
    onAfterRender(renderer, renderTarget);
  }

  void RenderSystem::renderVolumeMask(SceneRenderer &renderer, optional<XRRenderTarget> renderTarget)
  {
    if (renderer.isVolumeMaskEnabled())
    {
      renderer.addVolumeMask([this, &renderTarget](ecs::EntityId entity, SceneRenderer &renderer)
                             { renderMesh(entity, getComponent<Mesh3d>(entity), renderer, renderTarget); });
    }
  }

  void RenderSystem::onBeforeRender(SceneRenderer &renderer, optional<XRRenderTarget> renderTarget)
  {
    if (renderer.isVolumeMaskEnabled())
      renderer.enableVolumeMask();
  }

  void RenderSystem::onAfterRender(SceneRenderer &renderer, optional<XRRenderTarget> renderTarget)
  {
    if (renderer.isVolumeMaskEnabled())
      renderer.disableVolumeMask();
  }

  void RenderSystem::traverseAndRender(ecs::EntityId entity,
                                       SceneRenderer &renderer,
                                       optional<XRRenderTarget> renderTarget)
  {
    auto renderEntity = [this, &renderer, renderTarget](ecs::EntityId entity) -> bool
    {
      // Render the mesh if it exists
      auto mesh = getComponent<Mesh3d>(entity);
      if (mesh != nullptr)
      {
        // If the mesh exists but rendering is disabled, we need to skip its rendering and its children.
        if (mesh->isRenderingDisabled())
          return false;
        renderMesh(entity, mesh, renderer, renderTarget);
      }

      // TODO: support other renderable components (e.g., particles, etc.)
      return true;
    };

    if (!renderEntity(entity))
      return;

    auto children = getComponent<hierarchy::Children>(entity);
    if (children != nullptr)
    {
      for (const auto &child : children->children())
        traverseAndRender(child, renderer, renderTarget);
    }
  }

  void RenderSystem::renderMesh(ecs::EntityId &entity,
                                shared_ptr<Mesh3d> meshComponent,
                                SceneRenderer &renderer,
                                optional<XRRenderTarget> renderTarget)
  {
    auto materialComponent = getComponent<MeshMaterial3d>(entity);
    if (TR_UNLIKELY(materialComponent == nullptr))
    {
      assert(false && "The material component must be valid.");
      return;
    }

    if (!meshComponent->initialized())
      renderer.initializeMesh3d(meshComponent);
    if (!materialComponent->initialized())
      renderer.initializeMeshMaterial3d(meshComponent, materialComponent);

    // Update the mesh3d and material if needed
    renderer.tryUpdateMeshMaterial3d(meshComponent, materialComponent);

    // Update the instance transformation matrix if it's an instanced mesh
    tryUpdateInstanceDataForInstancedMesh(*meshComponent, renderTarget);

    // Draw
    shared_ptr<Transform> parentTransform = nullptr;
    auto parentComponent = getComponent<hierarchy::Parent>(entity);
    if (parentComponent != nullptr)
      parentTransform = getComponent<Transform>(parentComponent->parent());
    renderer.drawMesh3d(entity,
                        meshComponent,
                        materialComponent,
                        getComponent<Transform>(entity),
                        parentTransform,
                        renderTarget);
  }
}
