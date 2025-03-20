#include <array>
#include <chrono>
#include <client/dom/node.hpp>

#include "./client_renderer.hpp"
#include "./hierarchy.hpp"
#include "./web_content.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  void Renderer::initializeMesh3d(shared_ptr<Mesh3d> mesh3d)
  {
    auto vao = glContext_->createVertexArray();
    auto vbo = glContext_->createBuffer();
    auto ebo = glContext_->createBuffer();

    {
      // Bind the vertex array object, vertex buffer object, and element buffer object.
      WebGLVertexArrayScope vaoScope(glContext_, vao);
      glContext_->bindBuffer(WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
    }
    mesh3d->initialize(glContext_, vao, vbo);

    /**
     * If a mesh is instanced, we will use:
     *
     * - vao as the opaque mesh vertex array object.
     * - creating a new VAO as the transparent mesh vertex array object.
     */
    if (mesh3d->isInstancedMesh())
    {
      auto &instancedMesh = mesh3d->getHandleCheckedAsRef<InstancedMeshBase>();
      auto transparentVao = glContext_->createVertexArray();
      {
        WebGLVertexArrayScope vaoScope(glContext_, transparentVao);
        glContext_->bindBuffer(WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
      }
      instancedMesh.setup(glContext_,
                          vao, glContext_->createBuffer(),
                          transparentVao, glContext_->createBuffer());
    }
  }

  void Renderer::configureMeshVertexData(shared_ptr<Mesh3d> mesh3d, shared_ptr<WebGLProgram> program)
  {
    /**
     * Configure the vertext attributes.
     */
    auto configureAttribute = [this](const IVertexAttribute &attrib, int index, size_t stride, size_t offset)
    {
      glContext_->vertexAttribPointer(index,
                                      attrib.size(),
                                      attrib.type(),
                                      attrib.normalized(),
                                      stride,
                                      offset);
      glContext_->enableVertexAttribArray(index);
    };

    auto vao = mesh3d->vertexArrayObject();
    {
      WebGLVertexArrayScope vaoScope(glContext_, vao);

      // Configure the vertex attributes
      glContext_->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, mesh3d->vertexBufferObject());
      mesh3d->iterateEnabledAttributes(program, configureAttribute);

      // Configure the vertex buffer data
      auto &vertexBufferData = mesh3d->vertexBuffer().data();
      glContext_->bufferData(WebGLBufferBindingTarget::kArrayBuffer,
                             vertexBufferData.size(),
                             const_cast<uint8_t *>(vertexBufferData.data()),
                             WebGLBufferUsage::kStaticDraw);

      // Configure the element buffer object
      auto indices = mesh3d->indices();
      glContext_->bufferData(WebGLBufferBindingTarget::kElementArrayBuffer,
                             indices.dataSize(),
                             indices.dataBuffer(),
                             WebGLBufferUsage::kStaticDraw);
    }

    // Configure the instance vbo and related attributes if it's an instanced mesh.
    if (mesh3d->isInstancedMesh())
    {
      auto &instancedMesh = mesh3d->getHandleCheckedAsRef<InstancedMeshBase>();
      /**
       * Configure the instance attributes.
       */
      auto configureInstanceAttribute = [this](const IVertexAttribute &attrib, int index, size_t stride, size_t offset)
      {
        glContext_->enableVertexAttribArray(index);
        glContext_->vertexAttribPointer(index,
                                        attrib.size(),
                                        attrib.type(),
                                        attrib.normalized(),
                                        stride,
                                        offset);
        glContext_->vertexAttribDivisor(index, 1);
      };

      // Configure for the opaque instances.
      {
        auto &opaqueInstancesList = instancedMesh.getOpaqueInstancesList();
        WebGLVertexArrayScope vaoScope(glContext_, opaqueInstancesList.vao);

        glContext_->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, opaqueInstancesList.instanceVbo);
        instancedMesh.iterateInstanceAttributes(program, configureInstanceAttribute);
      }

      // Configure for the transparent instances.
      {
        auto &transparentInstancesList = instancedMesh.getTransparentInstancesList();
        WebGLVertexArrayScope vaoScope(glContext_, transparentInstancesList.vao);

        glContext_->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, mesh3d->vertexBufferObject());
        mesh3d->iterateEnabledAttributes(program, configureAttribute);

        glContext_->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, transparentInstancesList.instanceVbo);
        instancedMesh.iterateInstanceAttributes(program, configureInstanceAttribute);
      }
    }
  }

  void Renderer::updateMeshVertexData(shared_ptr<Mesh3d> mesh3d, shared_ptr<WebGLProgram> program)
  {
  }

  void Renderer::initializeMeshMaterial3d(shared_ptr<Mesh3d> mesh3d, shared_ptr<MeshMaterial3d> meshMaterial3d)
  {
    auto program = glContext_->createProgram();
    auto vertexShader = glContext_->createShader(WebGLShaderType::kVertex);
    auto fragmentShader = glContext_->createShader(WebGLShaderType::kFragment);
    glContext_->shaderSource(vertexShader,
                             meshMaterial3d->getShaderSource(WebGLShaderType::kVertex));
    glContext_->shaderSource(fragmentShader,
                             meshMaterial3d->getShaderSource(WebGLShaderType::kFragment));
    glContext_->compileShader(vertexShader);
    glContext_->compileShader(fragmentShader);
    glContext_->attachShader(program, vertexShader);
    glContext_->attachShader(program, fragmentShader);
    glContext_->linkProgram(program);

    // Configure the vertex data: vertex array object, vertex buffer object, and element buffer object.
    // Configure the vertex attributes and the vertex buffer data.
    configureMeshVertexData(mesh3d, program);

    // Configure the initial uniform values
    {
      WebGLProgramScope programScope(glContext_, program);
      updateTransformationMatrix(program, nullptr, nullptr, true); // forcily update the transformation matrix.

      meshMaterial3d->initialize(glContext_, program, mesh3d);
    }
  }

  void Renderer::tryUpdateMeshMaterial3d(shared_ptr<Mesh3d> mesh3d, shared_ptr<MeshMaterial3d> meshMaterial3d)
  {
    auto program = meshMaterial3d->program();
    if (TR_UNLIKELY(program == nullptr))
      return;

    // Update the vertex data if it's dirty
    updateMeshVertexData(mesh3d, program);
    // TODO: update the instance data
  }

  void Renderer::drawMesh3d(const ecs::EntityId &entity, shared_ptr<Mesh3d> mesh, shared_ptr<MeshMaterial3d> material,
                            shared_ptr<Transform> transform,
                            shared_ptr<Transform> parentTransform,
                            optional<XRRenderTarget> renderTarget)
  {
    glContext_->enable(WEBGL_DEPTH_TEST);
    glContext_->depthMask(true);

    assert(mesh != nullptr && material != nullptr);
    assert(mesh->initialized());
    assert(material->initialized());
    WebGLProgramScope programScope(glContext_, material->program());

    // Call lifecycle methods
    material->onBeforeDrawMesh(mesh);

    // Update matrices
    updateViewProjectionMatrix(programScope.program(), renderTarget);
    updateTransformationMatrix(programScope.program(), transform, parentTransform);

    // Draw the mesh
    {
      WebGLVertexArrayScope vaoScope(glContext_, mesh->vertexArrayObject());
      if (mesh->isInstancedMesh())
      {
        auto &instancedMesh = mesh->getHandleCheckedAsRef<InstancedMeshBase>();
        if (instancedMesh.instanceCount() > 0)
        {
          WebGL2Context &glContext = *glContext_;
          instancedMesh.updateRenderQueues(); // Update the render queues for opaque and transparent instances.

          auto meshIndicesCount = mesh->indices().size();
          auto opaqueInstances = instancedMesh.getOpaqueInstancesList();
          if (opaqueInstances.count() > 0)
          {
            glContext.depthMask(true);
            glContext.disable(WEBGL_BLEND);

            auto loc = glContext.getUniformLocation(programScope.program(), "modelMatrix");
            glContext.uniformMatrix4fv(loc.value(), false, glm::mat4(1.0f));

            opaqueInstances.beforeInstancedDraw(glContext);
            glContext.drawElementsInstanced(mesh->primitiveTopology(),
                                            meshIndicesCount,
                                            WEBGL_UNSIGNED_INT,
                                            0,
                                            opaqueInstances.count());
            opaqueInstances.afterInstancedDraw(glContext);
          }

          /**
           * TODO: does this need to be moved global transparent rendering queue?
           */
          auto transparentInstances = instancedMesh.getTransparentInstancesList();
          if (transparentInstances.count() > 0)
          {
            WebGLVertexArrayScope vaoScope(glContext_, transparentInstances.vao);
            glContext.depthMask(false);
            glContext.enable(WEBGL_BLEND);
            glContext.blendFunc(WEBGL_SRC_ALPHA, WEBGL_ONE_MINUS_SRC_ALPHA);

            // Set the base matrix, move the transparent objects +z 0.001
            auto loc = glContext.getUniformLocation(programScope.program(), "modelMatrix");
            glm::mat4 matToUpdate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.001f));
            glContext.uniformMatrix4fv(loc.value(), false, matToUpdate);

            // Draw
            transparentInstances.beforeInstancedDraw(glContext);
            glContext.drawElementsInstanced(mesh->primitiveTopology(),
                                            meshIndicesCount,
                                            WEBGL_UNSIGNED_INT,
                                            0,
                                            transparentInstances.count());
            transparentInstances.afterInstancedDraw(glContext);
          }
        }
      }
      else
      {
        glContext_->drawElements(mesh->primitiveTopology(),
                                 mesh->indices().size(),
                                 WEBGL_UNSIGNED_INT,
                                 0);
      }
    }

    // Call lifecycle methods
    material->onAfterDrawMesh(mesh);
  }

  void Renderer::updateViewProjectionMatrix(shared_ptr<WebGLProgram> program,
                                            optional<XRRenderTarget> renderTarget)
  {
    assert(program != nullptr);

    auto viewProjection = glContext_->getUniformLocation(program, "viewProjection");
    if (!viewProjection.has_value())
      throw runtime_error("The viewProjection uniform location is not found.");

    auto handedness = MatrixHandedness::MATRIX_RIGHT_HANDED; // focily set to right-handed.
    if (renderTarget != nullopt)
    {
      if (renderTarget->isMultiview())
      {
        auto viewProjectionR = glContext_->getUniformLocation(program, "viewProjectionR");
        if (!viewProjectionR.has_value())
          throw runtime_error("The viewProjectionR uniform location is not found in multiview mode.");

        {
          MatrixComputationGraph graph(WebGLMatrixPlaceholderId::ViewProjectionMatrix, handedness);
          glContext_->uniformMatrix4fv(viewProjection.value(), false, graph);
        }
        {
          MatrixComputationGraph graph(WebGLMatrixPlaceholderId::ViewProjectionMatrixForRightEye, handedness);
          glContext_->uniformMatrix4fv(viewProjectionR.value(), false, graph);
        }
        return;
      }
      else
      {
        auto view = renderTarget->view();
        assert(view != nullptr);

        if (view->eye() == client_xr::XREye::kRight)
        {
          MatrixComputationGraph graph(WebGLMatrixPlaceholderId::ViewProjectionMatrixForRightEye, handedness);
          glContext_->uniformMatrix4fv(viewProjection.value(), false, graph);
          return;
        }
      }
    }

    // Default view projection matrix
    {
      MatrixComputationGraph graph(WebGLMatrixPlaceholderId::ViewProjectionMatrix, handedness);
      glContext_->uniformMatrix4fv(viewProjection.value(), false, graph);
    }
  }

  optional<glm::mat4> Renderer::updateTransformationMatrix(
      shared_ptr<WebGLProgram> program,
      shared_ptr<Transform> transform,
      shared_ptr<Transform> parentTransform,
      bool forceUpdate)
  {
    assert(program != nullptr);

    glm::mat4 matToUpdate;
    if (transform == nullptr || !transform->isDirty())
    {
      if (!forceUpdate)
        return nullopt;

      if (transform != nullptr)
        matToUpdate = transform->matrix();
      else
        matToUpdate = Transform::Identity().matrix();
    }
    else
      matToUpdate = transform->matrix();

    // Handle the post transform
    glm::mat4 postMat = glm::mat4(1.0f);
    if (parentTransform != nullptr && parentTransform->hasPostTransform())
    {
      auto &parentPostTransform = parentTransform->getOrInitPostTransform();
      postMat = parentPostTransform.accumulatedMatrix();
    }
    if (transform != nullptr && transform->hasPostTransform())
    {
      auto &postTransform = transform->getOrInitPostTransform();
      postMat = postTransform.matrix() * postMat;
      postTransform.setAccumulatedMatrix(postMat);
    }
    matToUpdate = postMat * matToUpdate;

    auto loc = glContext_->getUniformLocation(program, "modelMatrix");
    if (!loc.has_value())
      throw runtime_error("The modelMatrix uniform location is not found.");
    glContext_->uniformMatrix4fv(loc.value(), false, matToUpdate);
    return matToUpdate;
  }

  void RenderSystem::onExecute()
  {
    auto renderer = getResource<Renderer>();
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
            render(*renderer, Renderer::XRRenderTarget(view));
        }
        else
        {
          render(*renderer, Renderer::XRRenderTarget(views));
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

    glm::mat4 matToUpdate = transform.matrix();

    // Handle the post transform
    glm::mat4 postMat = glm::mat4(1.0f);
    if (parentTransform != nullptr && parentTransform->hasPostTransform())
    {
      auto &parentPostTransform = parentTransform->getOrInitPostTransform();
      postMat = parentPostTransform.accumulatedMatrix();
    }
    if (transform.hasPostTransform())
    {
      auto &postTransform = transform.getOrInitPostTransform();
      postMat = postTransform.matrix() * postMat;
      postTransform.setAccumulatedMatrix(postMat);
    }
    matToUpdate = postMat * matToUpdate;
    return matToUpdate;
  }

  void RenderSystem::tryUpdateInstanceDataForInstancedMesh(const Mesh3d &meshComponent)
  {
    if (!meshComponent.isInstancedMesh())
      return;

    auto &instancedMesh = meshComponent.getHandleCheckedAsRef<InstancedMeshBase>();
    auto updateInstanceData = [this](ecs::EntityId id, Instance &instance) -> bool
    {
      bool hasChanged = false;
      if (hasComponent<Transform>(id))
      {
        auto currentMatrix = getTransformationMatrix(id);
        instance.setTransform(currentMatrix, hasChanged);
      }
      if (hasComponent<WebContent>(id))
      {
        auto &webContent = getComponentChecked<WebContent>(id);
        if (instance.setEnabled(true))
          hasChanged = true;
        if (instance.setOpaque(webContent.isOpaque()))
          hasChanged = true;

        // Only transparent content needs to update it's z-index
        if (webContent.isTransparent() && hasComponent<hierarchy::Element>(id))
        {
          auto& element = getComponentChecked<hierarchy::Element>(id);
          auto index = element.node->depth(); // FIXME: using the node depth as the z-index currently.
          if (instance.setZIndex(index))
            hasChanged = true;
        }

        auto textureRect = webContent.textureRect();
        if (textureRect != nullptr)
        {
          instance.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), hasChanged);
          instance.setTexture(textureRect->getUvOffset(),
                              textureRect->getUvScale(),
                              textureRect->layer,
                              hasChanged);
        }
        else
        {
          instance.setColor(webContent.backgroundColor(), hasChanged);
          instance.disableTexture(hasChanged);
        }
      }
      return hasChanged;
    };
    instancedMesh.iterateInstances(updateInstanceData);
  }

  void RenderSystem::render(Renderer &renderer, optional<Renderer::XRRenderTarget> renderTarget)
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

    for (auto root : roots)
      traverseAndRender(root, renderer, renderTarget);
  }

  void RenderSystem::traverseAndRender(ecs::EntityId entity, Renderer &renderer,
                                       optional<Renderer::XRRenderTarget> renderTarget)
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
      for (auto child : children->children())
        traverseAndRender(child, renderer, renderTarget);
    }
  }

  void RenderSystem::renderMesh(ecs::EntityId &entity, shared_ptr<Mesh3d> meshComponent, Renderer &renderer,
                                optional<Renderer::XRRenderTarget> renderTarget)
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
    tryUpdateInstanceDataForInstancedMesh(*meshComponent);

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
