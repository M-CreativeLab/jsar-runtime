#include <glm/gtc/random.hpp>
#include "./instanced_mesh-inl.hpp"

namespace builtin_scene
{
  using namespace std;

  size_t InstancedMeshBase::iterateInstanceAttributes(shared_ptr<client_graphics::WebGLProgram> program,
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
      auto hasChanged = callback(id, instance);
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
    return it->second;
  }

  const Instance &InstancedMeshBase::getInstance(ecs::EntityId id) const
  {
    shared_lock<shared_mutex> lock(mutex_);
    auto it = idToInstanceMap_.find(id);
    if (it == idToInstanceMap_.end())
      throw invalid_argument("The instance with the given entity id does not exist.");
    return it->second;
  }

  Instance &InstancedMeshBase::addInstance(ecs::EntityId id)
  {
    unique_lock<shared_mutex> lock(mutex_);
    if (idToInstanceMap_.find(id) != idToInstanceMap_.end())
      throw invalid_argument("The instance with the given entity id already exists.");

    auto newInstance = Instance();
    newInstance.color.r = glm::linearRand(0.0f, 1.0f);
    newInstance.color.g = glm::linearRand(0.0f, 1.0f);
    newInstance.color.b = glm::linearRand(0.0f, 1.0f);
    newInstance.color.a = 0.2f;
    idToInstanceMap_[id] = newInstance;

    markAsDirty();
    return idToInstanceMap_[id];
  }

  bool InstancedMeshBase::removeInstance(ecs::EntityId id)
  {
    unique_lock<shared_mutex> lock(mutex_);
    bool removed = idToInstanceMap_.erase(id) > 0;
    if (removed)
      markAsDirty();
    return removed;
  }

  void InstancedMeshBase::setup(shared_ptr<client_graphics::WebGL2Context> glContext,
                                shared_ptr<client_graphics::WebGLBuffer> instanceVbo)
  {
    assert(glContext != nullptr);
    assert(instanceVbo != nullptr);
    glContext_ = glContext;
    instanceVbo_ = instanceVbo;
  }

  vector<Instance> InstancedMeshBase::instancesArray() const
  {
    vector<Instance> instances;
    {
      shared_lock<shared_mutex> lock(mutex_);
      for (auto &[id, instance] : idToInstanceMap_)
        instances.push_back(instance);
    }
    return instances;
  }

  bool InstancedMeshBase::uploadInstanceData()
  {
    if (!isDirty_)
      return false;

    auto glContext = glContext_.lock();
    if (glContext != nullptr)
    {
      auto array = instancesArray();
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, instanceVbo_);
      glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                            array.size() * sizeof(Instance),
                            array.data(),
                            client_graphics::WebGLBufferUsage::kDynamicDraw);
    }
    isDirty_ = false;
    return true;
  }
}
