#include <chrono>
#include "./client_renderer.hpp"
#include "./hierarchy.hpp"
#include "./web_content.hpp"

namespace builtin_scene
{
  using namespace std;

  void Renderer::initializeMesh3d(shared_ptr<Mesh3d> mesh3d)
  {
    auto vao = glContext_->createVertexArray();
    auto vbo = glContext_->createBuffer();
    auto ebo = glContext_->createBuffer();

    {
      // Bind the vertex array object, vertex buffer object, and element buffer object.
      client_graphics::WebGLVertexArrayScope vaoScope(glContext_, vao);
      glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, vbo);
      glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
    }
    mesh3d->initialize(glContext_, vao);

    // Create the instanced vbo for the instanced mesh.
    if (mesh3d->isInstancedMesh())
      mesh3d->getHandleCheckedAsRef<InstancedMeshBase>()
          .setup(glContext_, glContext_->createBuffer());
  }

  void Renderer::configureMeshVertexData(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    auto vao = mesh3d->vertexArrayObject();
    client_graphics::WebGLVertexArrayScope vaoScope(glContext_, vao);

    // Configure the vertex attributes
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
    mesh3d->iterateEnabledAttributes(program, configureAttribute);

    // Configure the vertex buffer data
    auto &vertexBufferData = mesh3d->vertexBuffer().data();
    glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                           vertexBufferData.size(),
                           const_cast<uint8_t *>(vertexBufferData.data()),
                           client_graphics::WebGLBufferUsage::kStaticDraw);

    // Configure the element buffer object
    auto indices = mesh3d->indices();
    glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer,
                           indices.dataSize(),
                           indices.dataBuffer(),
                           client_graphics::WebGLBufferUsage::kStaticDraw);

    // Configure the instance vbo and related attributes if it's an instanced mesh.
    if (mesh3d->isInstancedMesh())
    {
      auto &instancedMesh = mesh3d->getHandleCheckedAsRef<InstancedMeshBase>();
      glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, instancedMesh.instanceVbo_);
      glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, 0, nullptr,
                             client_graphics::WebGLBufferUsage::kDynamicDraw);

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
      instancedMesh.iterateInstanceAttributes(program, configureInstanceAttribute);
    }
  }

  void Renderer::updateMeshVertexData(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (mesh3d->isInstancedMesh())
    {
      auto &instancedMesh = mesh3d->getHandleCheckedAsRef<InstancedMeshBase>();
      instancedMesh.uploadInstanceData();
    }
  }

  void Renderer::initializeMeshMaterial3d(shared_ptr<Mesh3d> mesh3d, shared_ptr<MeshMaterial3d> meshMaterial3d)
  {
    auto program = glContext_->createProgram();
    auto vertexShader = glContext_->createShader(client_graphics::WebGLShaderType::kVertex);
    auto fragmentShader = glContext_->createShader(client_graphics::WebGLShaderType::kFragment);
    glContext_->shaderSource(vertexShader,
                             meshMaterial3d->getShaderSource(client_graphics::WebGLShaderType::kVertex));
    glContext_->shaderSource(fragmentShader,
                             meshMaterial3d->getShaderSource(client_graphics::WebGLShaderType::kFragment));
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
      client_graphics::WebGLProgramScope programScope(glContext_, program);
      updateTransformationMatrix(program, nullptr, nullptr, true); // forcily update the transformation matrix.

      meshMaterial3d->initialize(glContext_, program, mesh3d);
    }
  }

  void Renderer::tryUpdateMeshMaterial3d(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<MeshMaterial3d> meshMaterial3d)
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
                            std::optional<XRRenderTarget> renderTarget)
  {
    glContext_->enable(WEBGL_DEPTH_TEST);
    glContext_->depthMask(true);

    assert(mesh != nullptr && material != nullptr);
    assert(mesh->initialized());
    assert(material->initialized());
    client_graphics::WebGLProgramScope programScope(glContext_, material->program());

    // Call lifecycle methods
    material->onBeforeDrawMesh(mesh);

    // Update matrices
    updateViewProjectionMatrix(programScope.program(), renderTarget);
    auto newMatrix = updateTransformationMatrix(programScope.program(), transform, parentTransform);

    // Draw the mesh
    {
      client_graphics::WebGLVertexArrayScope vaoScope(glContext_, mesh->vertexArrayObject());
      if (mesh->isInstancedMesh())
      {
        auto &instancedMesh = mesh->getHandleCheckedAsRef<InstancedMeshBase>();
        if (instancedMesh.instanceCount() > 0)
        {
          glContext_->drawElementsInstanced(mesh->primitiveTopology(),
                                            mesh->indices().size(),
                                            WEBGL_UNSIGNED_INT,
                                            0,
                                            instancedMesh.instanceCount());
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

  void Renderer::updateViewProjectionMatrix(shared_ptr<client_graphics::WebGLProgram> program,
                                            std::optional<XRRenderTarget> renderTarget)
  {
    assert(program != nullptr);

    auto viewProjection = glContext_->getUniformLocation(program, "viewProjection");
    if (!viewProjection.has_value())
      throw runtime_error("The viewProjection uniform location is not found.");

    auto handedness = MatrixHandedness::MATRIX_RIGHT_HANDED; // focily set to right-handed.
    if (renderTarget != std::nullopt)
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
      shared_ptr<client_graphics::WebGLProgram> program,
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
    render(*renderer, std::nullopt);
  }

  void RenderSystem::render(Renderer &renderer, std::optional<Renderer::XRRenderTarget> renderTarget)
  {
    auto roots = queryEntities<hierarchy::Root>();
    if (roots.size() <= 0) // No root entities to render
      return;

    if (renderTarget != std::nullopt)
    {
      if (!renderTarget->isMultiview())
        renderer.setViewport(renderTarget->view()->viewport());
    }

    for (auto root : roots)
    {
      if (getComponentChecked<hierarchy::Root>(root).renderable == true)
        traverseAndRender(root, renderer, renderTarget);
    }
  }

  void RenderSystem::traverseAndRender(ecs::EntityId entity, Renderer &renderer,
                                       std::optional<Renderer::XRRenderTarget> renderTarget)
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
                                std::optional<Renderer::XRRenderTarget> renderTarget)
  {
    auto materialComponent = getComponent<MeshMaterial3d>(entity);
    if (materialComponent == nullptr)
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
    if (meshComponent->isInstancedMesh())
    {
      auto &instancedMesh = meshComponent->getHandleCheckedAsRef<InstancedMeshBase>();
      auto updateTransform = [this](ecs::EntityId id, Instance &instance) -> bool
      {
        auto transform = getComponent<Transform>(id);
        if (transform != nullptr)
        {
          instance.setTransform(transform->matrix());
          return true;
        }
        else
        {
          return false;
        }
      };
      instancedMesh.iterateInstances(updateTransform);
    }

    // Draw
    std::shared_ptr<Transform> parentTransform = nullptr;
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
