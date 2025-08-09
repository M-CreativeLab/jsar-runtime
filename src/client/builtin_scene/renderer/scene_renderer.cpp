#include <array>
#include <chrono>
#include <client/builtin_scene/materials.hpp>
#include <client/builtin_scene/materials/web_content_instanced.hpp>
#include "./scene_renderer.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace ecs;
  using namespace client_graphics;
  using namespace client_xr;

  SceneRenderer::SceneRenderer(shared_ptr<WebGL2Context> glContext, math::Size3 volumeSize)
      : glContext_(glContext)
      , volumeSize_(volumeSize)
  {
    glContext_->enable(WEBGL_CULL_FACE);
  }

  void SceneRenderer::setViewport(XRViewport &viewport)
  {
    glContext_->viewport(viewport.x,
                         viewport.y,
                         viewport.width,
                         viewport.height);
  }

  void SceneRenderer::setPolygonOffset(bool enabled)
  {
    if (enabled)
    {
      glContext_->enable(WEBGL_POLYGON_OFFSET_FILL);
      glContext_->polygonOffset(-1.0f, -1.0f);
    }
    else
    {
      glContext_->disable(WEBGL_POLYGON_OFFSET_FILL);
    }
  }

  void SceneRenderer::initializeMesh3d(shared_ptr<Mesh3d> mesh3d)
  {
    auto vao = glContext_->createVertexArray();
    auto vbo = glContext_->createBuffer();
    auto ebo = glContext_->createBuffer();

    {
      // Bind the vertex array object, vertex buffer object, and element buffer object.
      WebGLVertexArrayScope vaoScope(glContext_, vao);
      glContext_->bindBuffer(WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
    }
    mesh3d->initialize(glContext_, vao, vbo, ebo);
  }

  void SceneRenderer::configureMeshVertexData(shared_ptr<Mesh3d> mesh3d, shared_ptr<WebGLProgram> program)
  {
    /**
     * Configure the vertext attributes.
     */
    auto configureAttribute = [this](const IVertexAttribute &attrib,
                                     int index,
                                     size_t stride,
                                     size_t offset)
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
      /**
       * Configure the instance attributes.
       */
      auto configureInstanceAttribute = [this](const IVertexAttribute &attrib,
                                               int index,
                                               size_t stride,
                                               size_t offset)
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

      // Handle GaussianSplatsMesh specifically
      if (mesh3d->is<GaussianSplatsMesh>())
      {
        auto &gaussianMesh = mesh3d->getHandleCheckedAsRef<GaussianSplatsMesh>();

        // Configure for Gaussian splats (they use the main VAO)
        WebGLVertexArrayScope vaoScope(glContext_, mesh3d->vertexArrayObject());

        // First configure vertex attributes
        glContext_->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, mesh3d->vertexBufferObject());
        mesh3d->iterateEnabledAttributes(program, configureAttribute);

        // Then configure instance attributes
        gaussianMesh.setupSplatBuffer(glContext_, mesh3d->vertexArrayObject());

        // Bind the splat instance buffer and configure instance attributes
        auto splatBuffer = gaussianMesh.getSplatInstanceBuffer();
        if (splatBuffer)
        {
          glContext_->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, splatBuffer);
          gaussianMesh.iterateInstanceAttributes(program, configureInstanceAttribute);
        }
      }
      else
      {
        // Handle regular InstancedMeshBase
        auto &instancedMesh = mesh3d->getHandleCheckedAsRef<InstancedMeshBase>();

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
  }

  void SceneRenderer::updateMeshVertexData(shared_ptr<Mesh3d> mesh3d, shared_ptr<WebGLProgram> program)
  {
  }

  void SceneRenderer::initializeMeshMaterial3d(shared_ptr<Mesh3d> mesh3d, shared_ptr<MeshMaterial3d> meshMaterial3d)
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

  void SceneRenderer::tryUpdateMeshMaterial3d(shared_ptr<Mesh3d> mesh3d, shared_ptr<MeshMaterial3d> meshMaterial3d)
  {
    auto program = meshMaterial3d->program();
    if (program == nullptr) [[unlikely]]
      return;

    // Update the vertex data if it's dirty
    updateMeshVertexData(mesh3d, program);
    // TODO: update the instance data
  }

  void SceneRenderer::drawMesh3d(const EntityId &entity,
                                 shared_ptr<Mesh3d> mesh,
                                 shared_ptr<MeshMaterial3d> material,
                                 shared_ptr<Transform> transform,
                                 shared_ptr<Transform> parentTransform,
                                 RenderPass renderPass,
                                 optional<XRRenderTarget> renderTarget)
  {
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
      material->drawMeshImpl(mesh, renderPass, renderTarget);
    }

    // Call lifecycle methods
    material->onAfterDrawMesh(mesh);
  }

  void SceneRenderer::updateViewProjectionMatrix(shared_ptr<WebGLProgram> program,
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

        if (view->eye() == XREye::kRight)
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

  optional<glm::mat4> SceneRenderer::updateTransformationMatrix(shared_ptr<WebGLProgram> program,
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
        matToUpdate = glm::mat4(1.0f);
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

  void SceneRenderer::addVolumeMask(function<void(EntityId, SceneRenderer &)> drawMaskGeometry)
  {
    assert(drawMaskGeometry != nullptr);
    assert(volumeMask_.has_value());

    // Clear the stencil buffer before drawing the volume mask.
    glContext_->clearStencil(0x00);
    glContext_->clear(WEBGL_STENCIL_BUFFER_BIT);

    glContext_->enable(WEBGL_DEPTH_TEST);
    glContext_->colorMask(false, false, false, false);
    glContext_->depthMask(false);

    glContext_->enable(WEBGL_STENCIL_TEST);
    glContext_->stencilFunc(WEBGL_ALWAYS, volumeMaskStencilRef_, 0xff);
    glContext_->stencilOp(WEBGL_KEEP,
                          WEBGL_KEEP,
                          WEBGL_REPLACE);
    glContext_->stencilMask(0xff);
    {
      drawMaskGeometry(volumeMask_.value(), *this);
    }
    glContext_->colorMask(true, true, true, true);
    glContext_->depthMask(true);
    glContext_->disable(WEBGL_STENCIL_TEST);
  }

  void SceneRenderer::removeVolumeMask()
  {
    assert(volumeMask_.has_value());
    glContext_->stencilMask(0x00);
  }

  void SceneRenderer::enableVolumeMask()
  {
    glContext_->enable(WEBGL_STENCIL_TEST);
    glContext_->stencilFunc(WEBGL_EQUAL, volumeMaskStencilRef_, 0xff);
    glContext_->stencilOp(WEBGL_KEEP, WEBGL_KEEP, WEBGL_KEEP);
    glContext_->stencilMask(0x00);
  }

  void SceneRenderer::disableVolumeMask()
  {
    glContext_->disable(WEBGL_STENCIL_TEST);
  }
}
