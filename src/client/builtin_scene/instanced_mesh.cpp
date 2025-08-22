#include "./instanced_mesh-inl.hpp"
#include "./css_border_data_texture.hpp"

namespace builtin_scene
{
  class CSSBorderDataTexture;
}

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  void Instance::randomColor()
  {
    glm::vec4 color(1.0f);
    color.r = glm::linearRand(0.0f, 1.0f);
    color.g = glm::linearRand(0.0f, 1.0f);
    color.b = glm::linearRand(0.0f, 1.0f);
    color.a = 1.0f;
    setColor(color);
  }

  bool Instance::setColor(const glm::vec4 &color)
  {
    if (data_.color.r == color.r &&
        data_.color.g == color.g &&
        data_.color.b == color.b &&
        data_.color.a == color.a)
      return false;

    data_.color = color;
    setMaybeInvisible();
    notifyBufferDataChanged();
    return true;
  }

  void Instance::translate(float tx, float ty, float tz)
  {
    auto &transform = data_.transform;
    transform = glm::translate(transform, glm::vec3(tx, ty, tz));
    notifyBufferDataChanged();
  }

  void Instance::scale(float sx, float sy, float sz)
  {
    auto &transform = data_.transform;
    transform = glm::scale(transform, glm::vec3(sx, sy, sz));
    notifyBufferDataChanged();
  }

  void Instance::setTransform(const glm::mat4 &transformationMatrix)
  {
    auto &transform = data_.transform;
    if (transform == transformationMatrix)
      return; // Skip if there is no change.

    data_.transform = transformationMatrix;
    notifyBufferDataChanged();
  }

  void Instance::setTexture(TextureOffset uvOffset,
                            TextureOffset uvOffsetR,
                            TextureScale uvScale,
                            uint32_t layerIndex)
  {
    if (data_.texUvOffset == uvOffset &&
        data_.texUvOffsetR == uvOffsetR &&
        data_.texUvScale == uvScale &&
        data_.texLayerIndex == layerIndex)
    {
      return; // Skip if there is no change.
    }

    data_.texUvOffset = uvOffset;
    data_.texUvOffsetR = uvOffsetR;
    data_.texUvScale = uvScale;
    data_.texLayerIndex = layerIndex;
    setMaybeInvisible();
    notifyBufferDataChanged();
  }

  void Instance::disableTexture()
  {
    setTexture(TextureOffset(), TextureOffset(), TextureScale(), 0);
  }

  void Instance::setDimensions(float width, float height)
  {
    if (data_.dimensions.x == width && data_.dimensions.y == height)
      return; // Skip if there is no change.

    data_.dimensions = glm::vec2(width, height);
    notifyBufferDataChanged();
  }

  void Instance::setBorderRadius(glm::vec4 borderRadius)
  {
    if (data_.borderRadius == borderRadius)
      return; // Skip if there is no change.

    data_.borderRadius = borderRadius;
    notifyBufferDataChanged();
  }

  void Instance::setBorderRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
  {
    setBorderRadius(glm::vec4(topLeft, topRight, bottomRight, bottomLeft));
  }

  void Instance::setBorderWidth(glm::vec4 borderWidth)
  {
    if (borderWidths_ == borderWidth)
      return; // Skip if there is no change.

    borderWidths_ = borderWidth;
    setMaybeInvisible();
    notifyTextureDataChanged();
  }

  void Instance::setBorderWidth(float top, float right, float bottom, float left)
  {
    setBorderWidth(glm::vec4(top, right, bottom, left));
  }

  void Instance::setBorderColor(glm::vec4 borderColor)
  {
    // Set the same color for all four sides for now
    bool anyChanged = false;
    for (int i = 0; i < 4; ++i)
    {
      if (borderColors_[i] != borderColor)
      {
        borderColors_[i] = borderColor;
        anyChanged = true;
      }
    }

    if (anyChanged)
    {
      setMaybeInvisible();
      notifyTextureDataChanged();
    }
  }

  void Instance::setBorderColor(float r, float g, float b, float a)
  {
    setBorderColor(glm::vec4(r, g, b, a));
  }

  void Instance::setBorderStyle(float borderStyle)
  {
    if (data_.borderStyle == borderStyle)
      return; // Skip if there is no change.

    data_.borderStyle = borderStyle;
    setMaybeInvisible();
    notifyBufferDataChanged();
  }

  void Instance::setSDFTextureEnabled(bool enabled)
  {
    float value = enabled ? 1.0f : 0.0f;
    if (data_.enableSDFTexture == value)
      return; // Skip if there is no change.

    data_.enableSDFTexture = value;
    notifyBufferDataChanged();
  }

  bool Instance::hasNoBorders() const
  {
    // Fast check for none border style.
    if (data_.isBorderNone())
      return true;

    // Check if all border widths are zero and all border colors are transparent.
    if (borderWidths_ == glm::vec4(0.0f) &&
        borderColors_[0].a == 0.0f &&
        borderColors_[1].a == 0.0f &&
        borderColors_[2].a == 0.0f &&
        borderColors_[3].a == 0.0f)
    {
      return true;
    }

    // Otherwise, there are borders.
    return false;
  }

  void Instance::addHolder(shared_ptr<InstanceListBase> holder)
  {
    // Check if the holder is already added.
    for (auto &h : holders_)
    {
      if (h.lock() == holder)
        return;
    }

    // Add the holder.
    holders_.push_back(holder);
  }

  void Instance::removeHolder(shared_ptr<InstanceListBase> holder)
  {
    holders_.erase(remove_if(holders_.begin(), holders_.end(), [holder](const weak_ptr<InstanceListBase> &h)
                             { return h.lock() == holder; }),
                   holders_.end());
  }

  void Instance::notifyBufferDataChanged()
  {
    for (auto &holder : holders_)
    {
      if (auto holderPtr = holder.lock())
        holderPtr->markBufferAsDirty();
    }
  }

  void Instance::notifyTextureDataChanged()
  {
    for (auto &holder : holders_)
    {
      if (auto holderPtr = holder.lock())
      {
        if (holderPtr->isContentInstancesList())
          dynamic_pointer_cast<ContentInstancesList>(holderPtr)->markTextureDataAsDirty();
      }
    }
  }

  bool Instance::skipToDraw() const
  {
    // Skip if the instance is disabled or maybe invisible.
    if (!enabled_ || maybeInvisible_)
      return true;

    // Otherwise, the instance is ready to draw.
    return false;
  }

  void Instance::setMaybeInvisible()
  {
    // The instance should be invisible if it has transparent color, no texture, and no borders to draw.
    maybeInvisible_ = data_.isTransparent() &&
                      !data_.ownTexture() &&
                      hasNoBorders();
  }

  InstanceListBase::InstanceListBase(shared_ptr<WebGLVertexArray> vao,
                                     shared_ptr<WebGLBuffer> instanceVbo)
      : vao(vao)
      , instanceVbo(instanceVbo)
      , bufferDataDirty_(true)
  {
  }

  size_t InstanceListBase::configureAttribs(shared_ptr<WebGL2Context> glContext,
                                            shared_ptr<WebGLProgram> program,
                                            shared_ptr<Mesh3d> mesh3d)
  {
    WebGLVertexArrayScope vaoScope(glContext, vao);

    glContext->bindBuffer(WebGLBufferBindingTarget::kElementArrayBuffer, mesh3d->elementBufferObject());
    glContext->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, mesh3d->vertexBufferObject());
    auto configureVertexAttrib = [&](const IVertexAttribute &attrib, int index, size_t stride, size_t offset)
    {
      glContext->vertexAttribPointer(index,
                                     attrib.size(),
                                     attrib.type(),
                                     attrib.normalized(),
                                     stride,
                                     offset);
      glContext->enableVertexAttribArray(index);
    };
    mesh3d->iterateEnabledAttributes(program, configureVertexAttrib);

    // Make sure the `vbo` is currently bound.
    glContext->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, instanceVbo);
    auto configureInstanceAttrib = [&glContext](const IVertexAttribute &attrib,
                                                int index,
                                                size_t stride,
                                                size_t offset)
    {
      glContext->enableVertexAttribArray(index);
      glContext->vertexAttribPointer(index,
                                     attrib.size(),
                                     attrib.type(),
                                     attrib.normalized(),
                                     stride,
                                     offset);
      glContext->vertexAttribDivisor(index, 1);
    };

    // Iterate through the instance attributes and call `configureInstanceAttrib` to configure them.
    size_t attribsCount = 0;
    size_t offset = 0;
    for (size_t i = 0; i < InstancedMeshBase::INSTANCE_ATTRIBUTES.size(); i++)
    {
      auto &name = InstancedMeshBase::INSTANCE_ATTRIBUTES[i];
      auto attribLocation = glContext->getAttribLocation(program, name);
      if (attribLocation.has_value())
      {
        auto instanceIndex = attribLocation.value().index.value_or(-1);
        if (name == "instanceTransform")
        {
          for (int i = 0; i < 4; i++)
          {
            auto matrixRowIndex = instanceIndex + i;
            VertexAttribute<float, 4> attrib(name, matrixRowIndex, VertexFormat::kFloat32x4);
            configureInstanceAttrib(attrib, matrixRowIndex, InstancedMeshBase::STRIDE, offset);
            offset += attrib.byteLength();
            attribsCount += 1;
          }
        }
        else
        {
          unique_ptr<IVertexAttribute> attrib = nullptr;
          // 1u
          if (name == "instanceLayerIndex" ||
              name == "instanceBorderStyle")
          {
            attrib = make_unique<VertexAttribute<uint32_t, 1>>(name, instanceIndex, VertexFormat::kUint32);
          }
          // 1f
          else if (name == "instanceUseSDFTexture")
          {
            attrib = make_unique<VertexAttribute<float, 1>>(name, instanceIndex, VertexFormat::kFloat32);
          }
          // 2f
          else if (name == "instanceTexUvOffset" ||
                   name == "instanceTexUvScale" ||
                   name == "instanceTexUvOffsetR" ||
                   name == "instanceDimensions")
          {
            attrib = make_unique<VertexAttribute<float, 2>>(name, instanceIndex, VertexFormat::kFloat32x2);
          }
          // 4f
          else if (name == "instanceColor" ||
                   name == "instanceBorderRadius")
          {
            attrib = make_unique<VertexAttribute<float, 4>>(name, instanceIndex, VertexFormat::kFloat32x4);
          }
          else
          {
            assert(false && "Unknown instance attribute name.");
          }

          assert(attrib != nullptr);
          configureInstanceAttrib(*attrib, instanceIndex, InstancedMeshBase::STRIDE, offset);
          offset += attrib->byteLength();
          attribsCount += 1;
        }
      }
      else
      {
        cerr << "The instance attribute " << name << " is not found." << endl;
      }
    }
    return attribsCount;
  }

  void ContentInstancesList::update(const InstanceMap &instances, SortingOrder sortingOrder)
  {
    clearInstances(); // Clear the instances first.

    for (auto &[id, instance] : instances)
    {
      if (instance == nullptr) [[unlikely]]
        continue;
      if (instance->skipToDraw())
        continue;

      if (filter == InstanceFilter::kAll)
      {
        addInstance(instance);
      }
      else if (filter == InstanceFilter::kOpaque)
      {
        if (instance->isOpaque_)
          addInstance(instance);
      }
      else if (filter == InstanceFilter::kTransparent)
      {
        if (!instance->isOpaque_)
          addInstance(instance);
      }
    }

    sortInstances(sortingOrder);
    markTextureDataAsDirty();
  }

  void ContentInstancesList::beforeInstancedDraw(client_graphics::WebGL2Context &glContext,
                                                 CSSBorderDataTexture *borderDataTexture)
  {
    InstanceListBase::beforeInstancedDraw(glContext);

    // Update border data texture if border data is dirty
    if (textureDataDirty_ &&
        borderDataTexture != nullptr &&
        borderDataTexture->isInitialized())
    {
      borderDataTexture->updateBorderData(getInstances());
      textureDataDirty_ = false;
    }
  }

  void ContentInstancesList::clearInstances()
  {
    InstanceListBase::clearInstances();
    textureDataDirty_ = true;
  }

  void ContentInstancesList::sortInstances(SortingOrder sortingOrder)
  {
    if (sortingOrder != SortingOrder::kNone && list_.size() > 1)
    {
      // Sorting the instances by z-index and the sorting order.
      auto sortInstances = [sortingOrder](const weak_ptr<Instance> &a, const weak_ptr<Instance> &b)
      {
        if (auto aPtr = a.lock(); aPtr != nullptr)
        {
          if (auto bPtr = b.lock(); bPtr != nullptr)
          {
            if (sortingOrder == SortingOrder::kFrontToBack)
              return aPtr->renderQueue_ < bPtr->renderQueue_;
            else if (sortingOrder == SortingOrder::kBackToFront)
              return aPtr->renderQueue_ > bPtr->renderQueue_;
          }
        }
        return false;
      };
      sort(list_.begin(), list_.end(), sortInstances);
    }
  }

  void ContainerInstance::setInstance(std::shared_ptr<Instance> instance)
  {
    clearInstances();
    if (instance != nullptr)
      addInstance(instance);
    markBufferAsDirty();
  }

  size_t InstanceListBase::copyToArrayData(vector<InstanceData> &dst)
  {
    size_t len = 0;
    for (auto &instance : list_)
    {
      if (TR_UNLIKELY(instance.expired()))
        continue;
      auto instancePtr = instance.lock();
      if (TR_UNLIKELY(instancePtr == nullptr))
        continue;
      dst.push_back(instancePtr->data_);
      len += 1;
    }
    return len * sizeof(InstanceData);
  }

  void InstanceListBase::beforeInstancedDraw(WebGL2Context &glContext)
  {
    // Update instance VBO if structure is dirty
    if (bufferDataDirty_)
    {
      size_t len = 0;
      vector<InstanceData> array;
      if ((len = copyToArrayData(array)) > 0)
      {
        glContext.bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, instanceVbo);
        glContext.bufferData(WebGLBufferBindingTarget::kArrayBuffer,
                             len,
                             array.data(),
                             WebGLBufferUsage::kDynamicDraw);
      }
      bufferDataDirty_ = false;
    }
  }

  void InstanceListBase::afterInstancedDraw(WebGL2Context &glContext)
  {
  }

  vector<shared_ptr<Instance>> InstanceListBase::getInstances() const
  {
    vector<shared_ptr<Instance>> instances;
    for (const auto &weakInstance : list_)
    {
      if (!weakInstance.expired())
      {
        auto instance = weakInstance.lock();
        if (instance)
        {
          instances.push_back(instance);
        }
      }
    }
    return instances;
  }

  void InstanceListBase::clearInstances()
  {
    for (auto &instance : list_)
    {
      if (TR_UNLIKELY(instance.expired()))
        continue;
      auto instancePtr = instance.lock();
      if (TR_UNLIKELY(instancePtr == nullptr))
        continue;
      instancePtr->removeHolder(shared_from_this());
    }
    list_.clear();
    bufferDataDirty_ = true;
  }

  void InstanceListBase::addInstance(shared_ptr<Instance> instance)
  {
    if (TR_UNLIKELY(instance == nullptr))
      return;
    list_.push_back(instance);
    instance->addHolder(shared_from_this());
    bufferDataDirty_ = true;
  }

  size_t InstancedMeshBase::iterateInstanceAttributes(shared_ptr<WebGLProgram> program,
                                                      function<void(const IVertexAttribute &,
                                                                    int,
                                                                    size_t,
                                                                    size_t)> callback) const
  {
    auto glContext = glContext_.lock();
    if (glContext == nullptr)
      return 0;

    size_t attribsCount = 0;
    size_t offset = 0;
    for (size_t i = 0; i < INSTANCE_ATTRIBUTES.size(); i++)
    {
      auto &name = INSTANCE_ATTRIBUTES[i];
      auto attribLocation = glContext->getAttribLocation(program, name);
      if (attribLocation.has_value())
      {
        auto instanceIndex = attribLocation.value().index.value_or(-1);
        if (name == "instanceTransform")
        {
          for (int i = 0; i < 4; i++)
          {
            auto matrixRowIndex = instanceIndex + i;
            VertexAttribute<float, 4> attrib(name, matrixRowIndex, VertexFormat::kFloat32x4);
            callback(attrib, matrixRowIndex, STRIDE, offset);
            offset += attrib.byteLength();
            attribsCount += 1;
          }
        }
        else
        {
          unique_ptr<IVertexAttribute> attrib = nullptr;
          // 1u
          if (name == "instanceLayerIndex" ||
              name == "instanceBorderStyle")
          {
            attrib = make_unique<VertexAttribute<uint32_t, 1>>(name, instanceIndex, VertexFormat::kUint32);
          }
          // 1f
          else if (name == "instanceUseSDFTexture")
          {
            attrib = make_unique<VertexAttribute<float, 1>>(name, instanceIndex, VertexFormat::kFloat32);
          }
          // 2f
          else if (name == "instanceTexUvOffset" ||
                   name == "instanceTexUvScale" ||
                   name == "instanceTexUvOffsetR" ||
                   name == "instanceDimensions")
          {
            attrib = make_unique<VertexAttribute<float, 2>>(name, instanceIndex, VertexFormat::kFloat32x2);
          }
          // 4f
          else if (name == "instanceColor" ||
                   name == "instanceBorderRadius")
          {
            attrib = make_unique<VertexAttribute<float, 4>>(name, instanceIndex, VertexFormat::kFloat32x4);
          }
          else
          {
            assert(false && "Unknown instance attribute name.");
          }

          assert(attrib != nullptr);
          callback(*attrib, instanceIndex, STRIDE, offset);
          offset += attrib->byteLength();
          attribsCount += 1;
        }
      }
      else
      {
        cerr << "The instance attribute " << name << " is not found." << endl;
      }
    }
    return attribsCount;
  }

  int InstancedMeshBase::instanceCount() const
  {
    shared_lock<shared_mutex> lock(mutex_);
    return idToInstanceMap_.size();
  }

  void InstancedMeshBase::iterateInstances(function<bool(ecs::EntityId, Instance &)> callback)
  {
    shared_lock<shared_mutex> lock(mutex_);
    for (auto &[id, instance] : idToInstanceMap_)
    {
      auto needsUpdate = callback(id, *instance);
      if (needsUpdate)
        markStructureAsDirty();
    }
  }

  Instance &InstancedMeshBase::getInstance(ecs::EntityId id)
  {
    shared_lock<shared_mutex> lock(mutex_);
    auto it = idToInstanceMap_.find(id);
    if (it == idToInstanceMap_.end())
      throw invalid_argument("The instance with the given entity id does not exist.");

    assert(it->second != nullptr);
    return *it->second;
  }

  const Instance &InstancedMeshBase::getInstance(ecs::EntityId id) const
  {
    shared_lock<shared_mutex> lock(mutex_);
    auto it = idToInstanceMap_.find(id);
    if (it == idToInstanceMap_.end())
      throw invalid_argument("The instance with the given entity id does not exist.");

    assert(it->second != nullptr);
    return *it->second;
  }

  Instance &InstancedMeshBase::addInstance(ecs::EntityId id)
  {
    unique_lock<shared_mutex> lock(mutex_);
    if (idToInstanceMap_.find(id) != idToInstanceMap_.end())
      throw invalid_argument("The instance with the given entity id already exists.");

    auto &instance = idToInstanceMap_[id] = make_shared<Instance>();
    markStructureAsDirty();
    return *instance;
  }

  bool InstancedMeshBase::removeInstance(ecs::EntityId id)
  {
    unique_lock<shared_mutex> lock(mutex_);
    bool removed = idToInstanceMap_.erase(id) > 0;
    if (removed)
      markStructureAsDirty();
    return removed;
  }

  void InstancedMeshBase::setup(shared_ptr<WebGL2Context> glContext, shared_ptr<Mesh3d> mesh3d)
  {
    assert(glContext != nullptr && "WebGL2 context must not be null.");
    assert(mesh3d != nullptr && "Mesh3d must not be null.");

    glContext_ = glContext;
    mesh3d_ = mesh3d;
    depthOnlyInstances_ = make_shared<ContentInstancesList>(InstanceFilter::kTransparent,
                                                            glContext->createVertexArray(),
                                                            glContext->createBuffer());
  }

  void InstancedMeshBase::configureInstanceAttribs(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh3d)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr && "WebGL2 context must not be null.");
    if (program != nullptr)
      depthOnlyInstances_->configureAttribs(glContext, program, mesh3d);
  }

  void InstancedMeshBase::updateInstancesList(shared_ptr<client_graphics::WebGLProgram> program, bool ignoreDirty)
  {
    if (!isStructureDirty_ && !ignoreDirty)
      return;

    auto glContext = glContext_.lock();
    auto mesh3d = mesh3d_.lock();
    if (glContext == nullptr || mesh3d == nullptr)
    {
      cerr << "InstancedMeshBase::updateInstancesList(): WebGL2 context or Mesh3d is not set."
           << endl;
      return;
    }

    shared_lock<shared_mutex> lock(mutex_);

    // Update layered instances based on RenderLayer from instances
    set<RenderLayer> allLayers;
    map<RenderLayer, InstanceMap> contentInstanceMaps;
    map<RenderLayer, InstanceMap> containerInstanceMaps;
    for (auto &[id, instance] : idToInstanceMap_)
    {
      if (instance != nullptr)
      {
        RenderLayer layer = instance->renderLayer_;
        bool shouldInsertLayer = false;
        if (!instance->skipToDraw())
        {
          contentInstanceMaps[layer][id] = instance;
          shouldInsertLayer = true;
        }
        if (instance->isContainer_)
        {
          containerInstanceMaps[layer][id] = instance;
          shouldInsertLayer = true;
        }
        if (shouldInsertLayer)
          allLayers.insert(layer);
      }
    }

    // Clear existing layered instances and rebuild from scratch for vector structure
    layeredInstances_.clear();

    // Create LayeredInstancesData entries per layer
    // TODO: Per-container logic will be implemented later
    for (const auto &layer : allLayers)
    {
      unordered_map<uint32_t, LayeredInstancesData> layeredDataMap;
      optional<LayeredInstancesData> defaultLayeredData; // The default layer (no container)

      bool ownsContainer = containerInstanceMaps.find(layer) != containerInstanceMaps.end();
      if (ownsContainer)
      {
        for (const auto &[id, instance] : containerInstanceMaps[layer])
        {
          LayeredInstancesData layerData(layer);
          layerData.containerInstance = make_shared<ContainerInstance>(id,
                                                                       glContext->createVertexArray(),
                                                                       glContext->createBuffer());
          layerData.containerInstance->configureAttribs(glContext, program, mesh3d);
          layerData.containerInstance->setInstance(instance);
          layeredDataMap[id] = move(layerData);
        }
      }

      // Create or update renderable instances list (if layer has regular instances)
      if (contentInstanceMaps.find(layer) != contentInstanceMaps.end())
      {
        for (const auto &[id, instance] : contentInstanceMaps[layer])
        {
          auto belongsToContainer = layeredDataMap.find(instance->belongsToContainerId_) != layeredDataMap.end();
          if (!belongsToContainer)
          {
            LayeredInstancesData layerData(layer);
            layerData.containerInstance = nullptr; // No container for this instance
            layerData.contentInstances = make_shared<ContentInstancesList>(InstanceFilter::kTransparent,
                                                                           glContext->createVertexArray(),
                                                                           glContext->createBuffer());
            layerData.contentInstances->configureAttribs(glContext, program, mesh3d);
            layerData.contentInstances->addInstance(instance);
            defaultLayeredData = move(layerData);
          }
          else
          {
            auto &layerData = layeredDataMap[instance->belongsToContainerId_];
            if (!layerData.contentInstances)
            {
              // If the layer does not have a content instance list, create one
              layerData.contentInstances = make_shared<ContentInstancesList>(InstanceFilter::kTransparent,
                                                                             glContext->createVertexArray(),
                                                                             glContext->createBuffer());
              layerData.contentInstances->configureAttribs(glContext, program, mesh3d);
            }
            layerData.contentInstances->addInstance(instance);
          }
        }
      }

      // Append the layer data to the layered instances vector
      if (defaultLayeredData.has_value())
      {
        defaultLayeredData->sortContentInstances();
        if (defaultLayeredData->contentInstances != nullptr)
          defaultLayeredData->contentInstances->markTextureDataAsDirty();
        layeredInstances_.push_back(defaultLayeredData.value());
      }
      for (auto &[_, layerData] : layeredDataMap)
      {
        layerData.sortContentInstances();
        if (layerData.contentInstances != nullptr)
          layerData.contentInstances->markTextureDataAsDirty();
        layeredInstances_.push_back(move(layerData));
      }
    }

    // Update depth-only instances with all instances for use in `DepthOnlyPass`.
    //
    // Sorting is disabled for depth-only instances because the depth pass does not require front-to-back or
    // back-to-front ordering. This improves performance, as sorting is unnecessary when only depth information is
    // written and no blending occurs.
    depthOnlyInstances_->update(idToInstanceMap_, ContentInstancesList::SortingOrder::kNone /* Disable sorting */);

    // Mark the structure as clean after updating.
    isStructureDirty_ = false;
  }
}
