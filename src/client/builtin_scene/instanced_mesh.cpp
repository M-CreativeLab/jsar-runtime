#include "./instanced_mesh-inl.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  void Instance::randomColor()
  {
    auto &color = data_.color;
    color.r = glm::linearRand(0.0f, 1.0f);
    color.g = glm::linearRand(0.0f, 1.0f);
    color.b = glm::linearRand(0.0f, 1.0f);
    color.a = 1.0f;
    notifyHolders();
  }

  bool Instance::setColor(const glm::vec4 &color, bool &hasChanged)
  {
    if (data_.color.r == color.r &&
        data_.color.g == color.g &&
        data_.color.b == color.b &&
        data_.color.a == color.a)
      return false;

    data_.color = color;
    notifyHolders();
    hasChanged = true;
    return true;
  }

  void Instance::translate(float tx, float ty, float tz)
  {
    auto &transform = data_.transform;
    transform = glm::translate(transform, glm::vec3(tx, ty, tz));
    notifyHolders();
  }

  void Instance::scale(float sx, float sy, float sz)
  {
    auto &transform = data_.transform;
    transform = glm::scale(transform, glm::vec3(sx, sy, sz));
    notifyHolders();
  }

  void Instance::setTransform(const glm::mat4 &transformationMatrix)
  {
    auto &transform = data_.transform;
    transform = transformationMatrix;
    notifyHolders();
  }

  void Instance::setTexture(array<float, 2> uvOffset,
                            array<float, 2> uvScale,
                            uint32_t layerIndex,
                            bool &hasChanged)
  {
    if (data_.texUvOffset.x == uvOffset[0] &&
        data_.texUvOffset.y == uvOffset[1] &&
        data_.texUvScale.x == uvScale[0] &&
        data_.texUvScale.y == uvScale[1] &&
        data_.texLayerIndex == layerIndex)
      return; // Skip if there is no change.

    data_.texUvOffset = glm::vec2(uvOffset[0], uvOffset[1]);
    data_.texUvScale = glm::vec2(uvScale[0], uvScale[1]);
    data_.texLayerIndex = layerIndex;
    notifyHolders();
    hasChanged = true;
  }

  void Instance::disableTexture(bool &hasChanged)
  {
    setTexture({0.0f, 0.0f}, {0.0f, 0.0f}, 0, hasChanged);
  }

  void Instance::addHolder(std::shared_ptr<RenderableInstancesList> holder)
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

  void Instance::removeHolder(std::shared_ptr<RenderableInstancesList> holder)
  {
    holders_.erase(remove_if(holders_.begin(), holders_.end(),
                             [holder](const weak_ptr<RenderableInstancesList> &h)
                             { return h.lock() == holder; }),
                   holders_.end());
  }

  void Instance::notifyHolders()
  {
    for (auto &holder : holders_)
    {
      if (auto holderPtr = holder.lock())
        holderPtr->markAsDirty();
    }
  }

  RenderableInstancesList::RenderableInstancesList(InstanceFilter filter,
                                                   shared_ptr<WebGLVertexArray> vao,
                                                   shared_ptr<WebGLBuffer> instanceVbo)
      : filter(filter),
        vao(vao),
        instanceVbo(instanceVbo),
        isDirty_(true)
  {
    assert(filter != InstanceFilter::kAll);
  }

  void RenderableInstancesList::update(const InstanceMap &instances, SortingOrder sortingOrder)
  {
    clearInstances(); // Clear the instances first.

    for (auto &[id, instance] : instances)
    {
      if (TR_UNLIKELY(instance == nullptr))
        continue;
      if (!instance->enabled_)
        continue;

      if (filter == InstanceFilter::kAll)
        addInstance(instance);
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

    if (sortingOrder != SortingOrder::kNone && list_.size() > 1)
    {
      // Sorting the instances by z-index and the sorting order.
      sort(list_.begin(), list_.end(),
           [sortingOrder](const weak_ptr<Instance> &a, const weak_ptr<Instance> &b)
           {
             if (auto aPtr = a.lock(); aPtr != nullptr)
             {
               if (auto bPtr = b.lock(); bPtr != nullptr)
               {
                 if (sortingOrder == SortingOrder::kFrontToBack)
                   return aPtr->zIndex_ < bPtr->zIndex_;
                 else if (sortingOrder == SortingOrder::kBackToFront)
                   return aPtr->zIndex_ > bPtr->zIndex_;
               }
             }
             return false;
           });
    }
    isDirty_ = true;
  }

  size_t RenderableInstancesList::copyToArrayData(vector<InstanceData> &dst)
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

  void RenderableInstancesList::beforeInstancedDraw(WebGL2Context &glContext)
  {
    if (!isDirty_)
      return;

    size_t len = 0;
    vector<InstanceData> array;
    if ((len = copyToArrayData(array)) > 0)
    {
      glContext.bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, instanceVbo);
      glContext.bufferData(WebGLBufferBindingTarget::kArrayBuffer, len, array.data(),
                           WebGLBufferUsage::kDynamicDraw);
    }
    isDirty_ = false;
  }

  void RenderableInstancesList::afterInstancedDraw(WebGL2Context &glContext)
  {
  }

  void RenderableInstancesList::clearInstances()
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
    isDirty_ = true;
  }

  void RenderableInstancesList::addInstance(std::shared_ptr<Instance> instance)
  {
    if (TR_UNLIKELY(instance == nullptr))
      return;
    list_.push_back(instance);
    instance->addHolder(shared_from_this());
    isDirty_ = true;
  }

  size_t InstancedMeshBase::iterateInstanceAttributes(shared_ptr<WebGLProgram> program,
                                                      function<void(const IVertexAttribute &, int, size_t, size_t)> callback) const
  {
    auto glContext = glContext_.lock();
    if (glContext == nullptr)
      return 0;

    size_t attribsCount = 0;
    size_t offset = 0;
    for (size_t i = 0; i < INSTANCE_ATTRIBUTES.size(); i++)
    {
      auto &name = INSTANCE_ATTRIBUTES[i];
      auto index = glContext->getAttribLocation(program, name);
      if (index.has_value())
      {
        auto instanceIndex = index.value();
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
          if (name == "instanceTexUvOffset" ||
              name == "instanceTexUvScale")
            attrib = make_unique<VertexAttribute<float, 2>>(name, instanceIndex, VertexFormat::kFloat32x2);
          else if (name == "instanceLayerIndex")
            attrib = make_unique<VertexAttribute<uint32_t, 1>>(name, instanceIndex, VertexFormat::kUint32);
          else
            attrib = make_unique<VertexAttribute<float, 4>>(name, instanceIndex, VertexFormat::kFloat32x4);

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
      auto hasChanged = callback(id, *instance);
      if (hasChanged)
        markAsDirty();
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
    markAsDirty();
    return *instance;
  }

  bool InstancedMeshBase::removeInstance(ecs::EntityId id)
  {
    unique_lock<shared_mutex> lock(mutex_);
    bool removed = idToInstanceMap_.erase(id) > 0;
    if (removed)
      markAsDirty();
    return removed;
  }

  void InstancedMeshBase::setup(shared_ptr<WebGL2Context> glContext,
                                shared_ptr<WebGLVertexArray> opaqueVao,
                                shared_ptr<WebGLBuffer> opaqueInstanceVbo,
                                shared_ptr<WebGLVertexArray> transparentVao,
                                shared_ptr<WebGLBuffer> transparentInstanceVbo)
  {
    assert(glContext != nullptr);
    glContext_ = glContext;
    opaqueInstances_ = make_shared<RenderableInstancesList>(
        InstanceFilter::kOpaque, opaqueVao, opaqueInstanceVbo);
    transparentInstances_ = make_shared<RenderableInstancesList>(
        InstanceFilter::kTransparent, transparentVao, transparentInstanceVbo);
  }

  void InstancedMeshBase::updateRenderQueues(bool ignoreDirty)
  {
    if (!isDirty_ && !ignoreDirty)
      return;

    shared_lock<shared_mutex> lock(mutex_);
    opaqueInstances_->update(idToInstanceMap_);
    transparentInstances_->update(idToInstanceMap_,
                                  RenderableInstancesList::SortingOrder::kFrontToBack);
    isDirty_ = false;
  }
}
